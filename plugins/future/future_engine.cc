//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#include "future_engine.h"
#include "index_engine.h"
#include "data_engine.h"
#include "static_engine.h"
#include "future_value_buf.h"
#include "logic/logic_comm.h"
#include "logic/logic_unit.h"
#include "proto/symbol_dynam_market.pb.h"
#include "basic/template.h"
#include "file/file_path.h"
#include "file/file_util.h"
namespace future_logic {

FutureEngine* FutureEngine::schduler_engine_ = NULL;
FutureManager* FutureEngine::schduler_mgr_ = NULL;

FutureManager::FutureManager() {
  IndexEngine::GetSchdulerManager();
  IndexEngine::GetIndexEngine();

  StaticEngine::GetSchdulerManager();
  StaticEngine::GetIndexEngine();

  DataEngine::GetSchdulerManager();
  DataEngine::GetDataEngine();
}

FutureManager::~FutureManager() {

  IndexEngine::FreeSchdulerManager();
  IndexEngine::FreeIndexEngine();

  StaticEngine::FreeSchdulerManager();
  StaticEngine::FreeIndexEngine();

  DataEngine::FreeSchdulerManager();
  DataEngine::FreeDataEngine();
}

bool FutureManager::OnDynaTick(const int socket, const int64 uid,
                               const std::string& token,
                               const std::string& field, const int32 net_code,
                               const std::string& sec_symbol,
                               const STK_TYPE& stk_type,
                               const std::string& start_time,
                               const std::string& end_time) {
  bool r = false;
  HIS_DATA_TYPE data_type = _DYNA_DATA;
  size_t start_pos = sec_symbol.find(".");
  std::string symbol = sec_symbol.substr(0, start_pos);
  std::string sec = sec_symbol.substr(
      start_pos + 1, sec_symbol.length() - symbol.length() - 1);

  future_infos::TickTimePos start_time_pos;
  future_infos::TickTimePos end_time_pos;

  int32 max_count = 0;

  if (net_code / BASE_NET_TYPE == HTTP_TYPE)
    max_count = 100;
  else
    max_count = 280;

  r = IndexEngine::GetSchdulerManager()->OnFetchIndexPos(sec, symbol, data_type,
                                                         start_time, end_time,
                                                         start_time_pos,
                                                         end_time_pos);

  std::list<future_infos::StaticInfo> static_list;
  r = StaticEngine::GetSchdulerManager()->OnFetchStaticInfo(sec, symbol, FUTURE,
                                                            start_time_pos,
                                                            end_time_pos,
                                                            static_list);

  /*ULOG_DEBUG2("static_list :%d start:%d end:%d", static_list.size(),
   start_time_pos.time_index(), end_time_pos.time_index());*/

  std::map<int32, future_infos::DayMarket> market_hash;
  r = DataEngine::GetSchdulerManager()->OnLoadData(data_type, stk_type,
                                                   static_list, market_hash);

  //ULOG_DEBUG2("%d", market_hash.size());
  /*ExtractDynamMarket(socket, start_time_pos, end_time_pos, static_list,
   market_hash);*/

  if (!static_list.empty()) {
    net_reply::DynaTick dyna_tick;
    SendDynamMarket(start_time_pos, end_time_pos, max_count, static_list,
                    market_hash, dyna_tick);
    if(dyna_tick.next_time() > 0)
      dyna_tick.check_minute();
    base_logic::DictionaryValue* value = dyna_tick.get();
    send_value(socket, value);
  } else {
    send_error(socket, TIME_NO_DATA, net_error(TIME_NO_DATA));
  }
  return r;
}

bool FutureManager::SendDynamMarket(
    future_infos::TickTimePos& start_pos, future_infos::TickTimePos& end_pos,
    const int32 max_count, std::list<future_infos::StaticInfo>& static_list,
    std::map<int32, future_infos::DayMarket>& market_hash,
    net_reply::DynaTick& dyna_tick) {
  future_infos::TimeUnit start_time_unit(start_pos.time_index());
  future_infos::TimeUnit end_time_unit(end_pos.time_index());
  bool r = false;

  int32 index_pos = 0;
  for (std::list<future_infos::StaticInfo>::iterator it = static_list.begin();
      it != static_list.end(); it++) {
    future_infos::StaticInfo static_info = (*it);
    //读取数据
    future_infos::DayMarket day_market;
    r = base::MapGet<std::map<int32, future_infos::DayMarket>,
        std::map<int32, future_infos::DayMarket>::iterator, int32,
        future_infos::DayMarket>(market_hash,
                                 static_info.static_info().market_date(),
                                 day_market);
    if (!r)
      continue;

    if (start_time_unit.full_day() == end_time_unit.full_day()) {  //开始结束同一天
      CalcuDynamMarket(day_market.market_data().c_str() + start_pos.start_pos(),
                       end_pos.end_pos() - start_pos.start_pos(), index_pos,
                       max_count, static_info, dyna_tick);
    } else if (start_time_unit.full_day() != end_time_unit.full_day()) {
      if (static_info.static_info().market_date()
          == start_time_unit.full_day()) {  //开始日期
        CalcuDynamMarket(
            day_market.market_data().c_str() + start_pos.start_pos(),
            day_market.market_data().length() - start_pos.start_pos(),
            index_pos, max_count, static_info, dyna_tick);
      } else if (static_info.static_info().market_date()
          == end_time_unit.full_day()) {  //结束日期
        CalcuDynamMarket(day_market.market_data().c_str(), end_pos.end_pos(),
                         index_pos, max_count, static_info, dyna_tick);
      } else {
        CalcuDynamMarket(day_market.market_data().c_str(),
                         day_market.market_data().length(), index_pos,
                         max_count, static_info, dyna_tick);
      }
    }
  }

  return true;
}

bool FutureManager::CalcuDynamMarket(const char* raw_data,
                                     const size_t raw_data_length,
                                     int32& index_pos, const int32 max_count,
                                     future_infos::StaticInfo& static_info,
                                     net_reply::DynaTick& dyna_tick) {
  size_t pos = 0;
  int price_digit = GetPriceMul(static_info.static_info().price_digit());
  int vol_unit = static_info.static_info().vol_unit();
  int64 last_time = 0;
  int64 next_time = 0;
  ULOG_DEBUG("================>");
  if (index_pos >= max_count)
    return true;
  while (pos < raw_data_length && index_pos < max_count) {
    int16 packet_length = *(int16*) (raw_data + pos);
    std::string packet;
    packet.assign(raw_data + pos + sizeof(int16),
                  packet_length - sizeof(int16));
    pos += packet_length;
    chaos_data::SymbolDynamMarket dynma_market;
    dynma_market.ParseFromString(packet);
    net_reply::DynaTickUnit* r_dyna_tick_unit = new net_reply::DynaTickUnit;
    r_dyna_tick_unit->set_current_time(dynma_market.current_time());
    r_dyna_tick_unit->set_open_price(dynma_market.open_price() * price_digit);
    r_dyna_tick_unit->set_high_price(dynma_market.high_price() * price_digit);
    r_dyna_tick_unit->set_low_price(dynma_market.low_price() * price_digit);
    r_dyna_tick_unit->set_new_price(dynma_market.new_price() * price_digit);
    r_dyna_tick_unit->set_volume(dynma_market.volume() * vol_unit);
    r_dyna_tick_unit->set_amount(dynma_market.amount());
    r_dyna_tick_unit->set_inner_vol(dynma_market.inner_vol() * vol_unit);
    r_dyna_tick_unit->set_tick_count(dynma_market.tick_count());

    r_dyna_tick_unit->set_buy_price_one(
        dynma_market.buy_price(0) * price_digit);
    r_dyna_tick_unit->set_buy_price_two(
        dynma_market.buy_price(1) * price_digit);
    r_dyna_tick_unit->set_buy_price_three(
        dynma_market.buy_price(2) * price_digit);
    r_dyna_tick_unit->set_buy_price_four(
        dynma_market.buy_price(3) * price_digit);
    r_dyna_tick_unit->set_buy_price_five(
        dynma_market.buy_price(4) * price_digit);

    r_dyna_tick_unit->set_buy_vol_one(dynma_market.buy_vol(0) * vol_unit);
    r_dyna_tick_unit->set_buy_vol_two(dynma_market.buy_vol(1) * vol_unit);
    r_dyna_tick_unit->set_buy_vol_three(dynma_market.buy_vol(2) * vol_unit);
    r_dyna_tick_unit->set_buy_vol_four(dynma_market.buy_vol(3) * vol_unit);
    r_dyna_tick_unit->set_buy_vol_five(dynma_market.buy_vol(4) * vol_unit);

    r_dyna_tick_unit->set_sell_price_one(
        dynma_market.sell_price(0) * price_digit);
    r_dyna_tick_unit->set_sell_price_two(
        dynma_market.sell_price(1) * price_digit);
    r_dyna_tick_unit->set_sell_price_three(
        dynma_market.sell_price(2) * price_digit);
    r_dyna_tick_unit->set_sell_price_four(
        dynma_market.sell_price(3) * price_digit);
    r_dyna_tick_unit->set_sell_price_five(
        dynma_market.sell_price(4) * price_digit);

    r_dyna_tick_unit->set_sell_vol_one(dynma_market.sell_vol(0) * vol_unit);
    r_dyna_tick_unit->set_sell_vol_two(dynma_market.sell_vol(1) * vol_unit);
    r_dyna_tick_unit->set_sell_vol_three(dynma_market.sell_vol(2) * vol_unit);
    r_dyna_tick_unit->set_sell_vol_four(dynma_market.sell_vol(3) * vol_unit);
    r_dyna_tick_unit->set_sell_vol_five(dynma_market.sell_vol(4) * vol_unit);

    r_dyna_tick_unit->set_open_interest(
        dynma_market.open_interest() * vol_unit);
    r_dyna_tick_unit->set_settle_price(
        dynma_market.settle_price() * price_digit);

    dyna_tick.set_unit(r_dyna_tick_unit->get());
    index_pos++;
    last_time = dynma_market.current_time();
  }

  if (pos < raw_data_length) {  //有未完成的數據
    int16 packet_length = *(int16*) (raw_data + pos);
    std::string packet;
    packet.assign(raw_data + pos + sizeof(int16),
                  packet_length - sizeof(int16));
    chaos_data::SymbolDynamMarket dynma_market;
    dynma_market.ParseFromString(packet);
    next_time = dynma_market.current_time();
    pos += packet_length;
    ULOG_DEBUG2("next_time:%d", next_time);
  }

  ULOG_DEBUG2("last_time:%d next_time:%d", last_time, next_time);
  dyna_tick.set_last_time(last_time);
  dyna_tick.set_next_time(next_time);
  ULOG_DEBUG2("%d---->%d---->%d", index_pos, dyna_tick.Size(), max_count);
  return true;
}

bool FutureManager::ExtractDynamMarket(
    const int socket, future_infos::TickTimePos& start_pos,
    future_infos::TickTimePos& end_pos,
    std::list<future_infos::StaticInfo>& static_list,
    std::map<int32, future_infos::DayMarket>& market_hash) {

  future_infos::TimeUnit start_time_unit(start_pos.time_index());
  future_infos::TimeUnit end_time_unit(end_pos.time_index());
  bool r = false;
  std::list<chaos_data::SymbolDynamMarket> dynam_list;

  for (std::list<future_infos::StaticInfo>::iterator it = static_list.begin();
      it != static_list.end(); it++) {
    future_infos::StaticInfo static_info = (*it);
    //读取数据
    future_infos::DayMarket day_market;
    r = base::MapGet<std::map<int32, future_infos::DayMarket>,
        std::map<int32, future_infos::DayMarket>::iterator, int32,
        future_infos::DayMarket>(market_hash,
                                 static_info.static_info().market_date(),
                                 day_market);
    if (!r)
      continue;

    if (start_time_unit.full_day() == end_time_unit.full_day()) {  //开始结束同一天
      CalcuDynamMarket(day_market.market_data().c_str() + start_pos.start_pos(),
                       end_pos.end_pos() - start_pos.start_pos(), static_info,
                       dynam_list);
    } else if (start_time_unit.full_day() != end_time_unit.full_day()) {
      if (static_info.static_info().market_date()
          == start_time_unit.full_day()) {  //开始日期
        CalcuDynamMarket(
            day_market.market_data().c_str() + start_pos.start_pos(),
            day_market.market_data().length() - start_pos.start_pos(),
            static_info, dynam_list);
      } else if (static_info.static_info().market_date()
          == end_time_unit.full_day()) {  //结束日期
        CalcuDynamMarket(day_market.market_data().c_str(), end_pos.end_pos(),
                         static_info, dynam_list);
      } else {
        CalcuDynamMarket(day_market.market_data().c_str(),
                         day_market.market_data().length(), static_info,
                         dynam_list);
      }
    }
  }

  ULOG_DEBUG2("%d", dynam_list.size());
  return true;
}

bool FutureManager::CalcuDynamMarket(
    const char* raw_data, const size_t raw_data_length,
    future_infos::StaticInfo& static_info,
    std::list<chaos_data::SymbolDynamMarket>& dynam_list) {
  size_t pos = 0;
  int price_digit = GetPriceMul(static_info.static_info().price_digit());
  int vol_unit = static_info.static_info().vol_unit();
  while (pos < raw_data_length) {
    int16 packet_length = *(int16*) (raw_data + pos);
    std::string packet;
    packet.assign(raw_data + pos + sizeof(int16),
                  packet_length - sizeof(int16));
    pos += packet_length;
    chaos_data::SymbolDynamMarket dynma_market;
    dynma_market.ParseFromString(packet);
    dynma_market.set_open_price(dynma_market.open_price() * price_digit);
    dynma_market.set_high_price(dynma_market.high_price() * price_digit);
    dynma_market.set_low_price(dynma_market.low_price() * price_digit);
    dynma_market.set_new_price(dynma_market.new_price() * price_digit);
    dynma_market.set_volume(dynma_market.volume() * vol_unit);
    dynma_market.set_inner_vol(dynma_market.inner_vol() * vol_unit);

    dynma_market.set_buy_price(0, dynma_market.buy_price(0) * price_digit);
    dynma_market.set_buy_price(1, dynma_market.buy_price(1) * price_digit);
    dynma_market.set_buy_price(2, dynma_market.buy_price(2) * price_digit);
    dynma_market.set_buy_price(3, dynma_market.buy_price(3) * price_digit);
    dynma_market.set_buy_price(4, dynma_market.buy_price(4) * price_digit);

    dynma_market.set_sell_price(0, dynma_market.sell_price(0) * price_digit);
    dynma_market.set_sell_price(1, dynma_market.sell_price(1) * price_digit);
    dynma_market.set_sell_price(2, dynma_market.sell_price(2) * price_digit);
    dynma_market.set_sell_price(3, dynma_market.sell_price(3) * price_digit);
    dynma_market.set_sell_price(4, dynma_market.sell_price(4) * price_digit);

    dynma_market.set_buy_vol(0, dynma_market.buy_vol(0) * vol_unit);
    dynma_market.set_buy_vol(1, dynma_market.buy_vol(1) * vol_unit);
    dynma_market.set_buy_vol(2, dynma_market.buy_vol(2) * vol_unit);
    dynma_market.set_buy_vol(3, dynma_market.buy_vol(3) * vol_unit);
    dynma_market.set_buy_vol(4, dynma_market.buy_vol(4) * vol_unit);

    dynma_market.set_sell_vol(0, dynma_market.sell_vol(0) * vol_unit);
    dynma_market.set_sell_vol(1, dynma_market.sell_vol(1) * vol_unit);
    dynma_market.set_sell_vol(2, dynma_market.sell_vol(2) * vol_unit);
    dynma_market.set_sell_vol(3, dynma_market.sell_vol(3) * vol_unit);
    dynma_market.set_sell_vol(4, dynma_market.sell_vol(4) * vol_unit);

    dynma_market.set_open_interest(dynma_market.open_interest() * vol_unit);
    dynma_market.set_settle_price(dynma_market.settle_price() * price_digit);

    future_infos::TimeUnit time_unit(dynma_market.current_time());
    ULOG_DEBUG2(
        "%d-%d-%d %d:%d:%d\t->open_price:%6d\t,high_price:%6d\t,low_price:%6d\t,new_price:%6d\t,volume:%6d\t,inner_vol:%6d\t,buy_one_price:%6d\t,sell_one_price:%6d\t,buy_one_vol:%6d\t,sell_one_vol:%6d\t,open_interest:%6d\t,settle_price:%6d\t",
        time_unit.exploded().year, time_unit.exploded().month,
        time_unit.exploded().day_of_month, time_unit.exploded().hour,
        time_unit.exploded().minute, time_unit.exploded().second,
        dynma_market.open_price(), dynma_market.high_price(),
        dynma_market.low_price(), dynma_market.new_price(),
        dynma_market.volume(), dynma_market.inner_vol(),
        dynma_market.buy_price(0), dynma_market.sell_price(0),
        dynma_market.buy_vol(0), dynma_market.sell_vol(0),
        dynma_market.open_interest(), dynma_market.settle_price());
    dynam_list.push_back(dynma_market);
  }
  return true;
}

int FutureManager::GetPriceMul(const uint8 price_digit) const {
  int nMul = std::min(6, (int) price_digit);
  int n = 1;
  for (int i = 0; i < nMul; i++)
    n *= 10;
  return n;
}

}


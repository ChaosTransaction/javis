//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#include "future_engine.h"
#include "index_engine.h"
#include "data_engine.h"
#include "static_engine.h"
#include "logic/logic_comm.h"
#include "proto/symbol_dynam_market.pb.h"
#include "basic/template.h"
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
  IndexEngine::FreeIndexEngine();
  IndexEngine::FreeSchdulerManager();

  StaticEngine::FreeIndexEngine();
  StaticEngine::FreeSchdulerManager();
}

bool FutureManager::OnDynaTick(const int socket, const int64 uid,
                               const std::string& token,
                               const std::string& field,
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

  r = IndexEngine::GetSchdulerManager()->OnFetchIndexPos(sec, symbol, data_type,
                                                         start_time, end_time,
                                                         start_time_pos,
                                                         end_time_pos);
  if (!r)
    return false;

  std::list<future_infos::StaticInfo> static_list;
  r = StaticEngine::GetSchdulerManager()->OnFetchStaticInfo(sec, symbol, FUTURE,
                                                            start_time_pos,
                                                            end_time_pos,
                                                            static_list);

  if (!r)
    return false;

  std::map<int32, future_infos::DayMarket> market_hash;
  r = DataEngine::GetSchdulerManager()->OnLoadData(data_type, stk_type,
                                                   static_list, market_hash);

  //ULOG_DEBUG2("%d", market_hash.size());
  ExtractDynamMarket(socket,start_time_pos, end_time_pos, static_list, market_hash);
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
    ULOG_DEBUG2("%d-%d-%d %d:%d:%d\t->open_price:%6d\t,high_price:%6d\t,low_price:%6d\t,new_price:%6d\t,volume:%6d\t,inner_vol:%6d\t,buy_one_price:%6d\t,sell_one_price:%6d\t,buy_one_vol:%6d\t,sell_one_vol:%6d\t,open_interest:%6d\t,settle_price:%6d\t",
                   time_unit.exploded().year, time_unit.exploded().month,
                   time_unit.exploded().day_of_month, time_unit.exploded().hour,
                   time_unit.exploded().minute,time_unit.exploded().second,
                   dynma_market.open_price(),dynma_market.high_price(),
                   dynma_market.low_price(),dynma_market.new_price(),
                   dynma_market.volume(),dynma_market.inner_vol(),
                   dynma_market.buy_price(0),dynma_market.sell_price(0),
                   dynma_market.buy_vol(0),dynma_market.sell_vol(0),
                   dynma_market.open_interest(),dynma_market.settle_price());
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


//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "future_engine.h"
#include "logic/logic_comm.h"
#include "basic/template.h"
namespace future_logic {

FutureEngine* FutureEngine::schduler_engine_ = NULL;
FutureManager* FutureEngine::schduler_mgr_ = NULL;

FutureManager::FutureManager() {
  Init();
  InitLock();
}

FutureManager::~FutureManager() {
  Deinit();
  DeinitLock();
}

void FutureManager::Init() {
  future_cache_ = new FutureCache();
  future_lock_ = new FutureLock();
}

void FutureManager::Deinit() {
  if (future_cache_) {
    delete future_cache_;
    future_cache_ = NULL;
  }
  if (future_lock_) {
    delete future_lock_;
    future_lock_ = NULL;
  }
}

void FutureManager::InitLock() {
  InitThreadrw(&future_lock_->zc_future_lock_);
  InitThreadrw(&future_lock_->sc_future_lock_);
  InitThreadrw(&future_lock_->dc_future_lock_);
  InitThreadrw(&future_lock_->zc_ftr_lock_);
  InitThreadrw(&future_lock_->sc_ftr_lock_);
  InitThreadrw(&future_lock_->dc_ftr_lock_);
}

void FutureManager::DeinitLock() {
  DeinitThreadrw(future_lock_->zc_future_lock_);
  DeinitThreadrw(future_lock_->sc_future_lock_);
  DeinitThreadrw(future_lock_->dc_future_lock_);
  DeinitThreadrw(future_lock_->zc_ftr_lock_);
  DeinitThreadrw(future_lock_->sc_ftr_lock_);
  DeinitThreadrw(future_lock_->dc_ftr_lock_);
}

bool FutureManager::OnFetchIndexPos(const int socket, const std::string& sec,
                                    const std::string& symbol,
                                    const HIS_DATA_TYPE& data_type,
                                    const std::string& start_time,
                                    const std::string& end_time) {
  future_infos::TimeFrame time_frame(start_time, end_time);
  bool r = false;
  future_infos::TickTimePos start_time_pos;
  future_infos::TickTimePos end_time_pos;

  DATETYPE_MAP start_type_pos_map;
  DATETYPE_MAP end_type_pos_map;

  DAYPOS_MAP start_day_pos_map;
  DAYPOS_MAP end_day_pos_map;

  HOURPOS_MAP start_hour_pos_map;
  HOURPOS_MAP end_hour_pos_map;

  MINUTEPOS_MAP start_minute_pos_map;
  MINUTEPOS_MAP end_minute_pos_map;

  LOADERROR load_erron;
  {
    base_logic::RLockGd lk(future_lock_->zc_future_lock_);
    load_erron = GetCompareTimeTickPos<SYMBOL_MAP, SYMBOL_MAP::iterator,
        const std::string, DATETYPE_MAP>(future_cache_->zc_future_,
                                         future_cache_->zc_future_, symbol,
                                         symbol, start_type_pos_map,
                                         end_type_pos_map);
  }

  if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
    OnLoadIndex(time_frame.start_time(), sec, symbol, data_type,
                future_cache_->zc_future_, start_type_pos_map,
                start_day_pos_map, start_hour_pos_map, start_minute_pos_map);
  else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
    OnLoadIndex(time_frame.end_time(), sec, symbol, data_type,
                future_cache_->zc_future_, end_type_pos_map, end_day_pos_map,
                end_hour_pos_map, end_minute_pos_map);

  {
    base_logic::RLockGd lk(future_lock_->zc_future_lock_);
    load_erron = GetCompareTimeTickPos<DATETYPE_MAP, DATETYPE_MAP::iterator,
        const HIS_DATA_TYPE, DAYPOS_MAP>(start_type_pos_map, end_type_pos_map,
                                   data_type, data_type, start_day_pos_map,
                                   end_day_pos_map);
  }

  if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
    OnLoadIndex(time_frame.start_time(), sec, symbol, data_type,
                future_cache_->zc_future_, start_type_pos_map,
                start_day_pos_map, start_hour_pos_map, start_minute_pos_map);
  else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
    OnLoadIndex(time_frame.end_time(), sec, symbol, data_type,
                future_cache_->zc_future_, end_type_pos_map, end_day_pos_map,
                end_hour_pos_map, end_minute_pos_map);

  {
    base_logic::RLockGd lk(future_lock_->zc_future_lock_);
    load_erron = GetCompareTimeTickPos<DAYPOS_MAP, DAYPOS_MAP::iterator,
        const int32, HOURPOS_MAP>(start_day_pos_map, end_day_pos_map,
                                  time_frame.start_full_day(),
                                  time_frame.end_full_day(), start_hour_pos_map,
                                  end_hour_pos_map);
  }

  if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
    OnLoadIndex(time_frame.start_time(), sec, symbol, data_type,
                future_cache_->zc_future_, start_type_pos_map,
                start_day_pos_map, start_hour_pos_map, start_minute_pos_map);
  else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
    OnLoadIndex(time_frame.end_time(), "ZC", symbol, data_type,
                future_cache_->zc_future_, end_type_pos_map, end_day_pos_map,
                end_hour_pos_map, end_minute_pos_map);

  {
    base_logic::RLockGd lk(future_lock_->zc_future_lock_);
    load_erron = GetCompareTimeTickPos<HOURPOS_MAP, HOURPOS_MAP::iterator,
        const int32, MINUTEPOS_MAP>(start_hour_pos_map, end_hour_pos_map,
                                    time_frame.start_exploded().hour,
                                    time_frame.end_exploded().hour,
                                    start_minute_pos_map, end_minute_pos_map);
  }

  if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
    OnLoadIndex(time_frame.start_time(), "ZC", symbol, data_type,
                future_cache_->zc_future_, start_type_pos_map,
                start_day_pos_map, start_hour_pos_map, start_minute_pos_map);
  else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
    OnLoadIndex(time_frame.end_time(), "ZC", symbol, data_type,
                future_cache_->zc_future_, end_type_pos_map, end_day_pos_map,
                end_hour_pos_map, end_minute_pos_map);

  {
    base_logic::RLockGd lk(future_lock_->zc_future_lock_);
    int32 start_unix_time = time_frame.start_time()->ToUnixTime() / 60 * 60;
    int32 end_unix_time = time_frame.end_time()->ToUnixTime() / 60 * 60;
    load_erron = GetCompareTimeTickPos<MINUTEPOS_MAP, MINUTEPOS_MAP::iterator,
        const int32, future_infos::TickTimePos>(
        start_minute_pos_map, end_minute_pos_map,
        start_unix_time, 
        end_unix_time,
        start_time_pos, end_time_pos);
  }
  return true;
}

void FutureManager::OnLoadIndex(future_infos::TimeUnit* time_unit,
                                const std::string& sec,
                                const std::string& symbol,
                                const HIS_DATA_TYPE& data_type,
                                SYMBOL_MAP& symbol_map, DATETYPE_MAP& type_map,
                                DAYPOS_MAP& day_map, HOURPOS_MAP& hour_map,
                                MINUTEPOS_MAP& minute_map) {

  OnLoadLoaclPos(sec, symbol, data_type, time_unit->exploded().year,
                 time_unit->exploded().month, time_unit->exploded().day_of_month,
                 minute_map);

  SetIndexPos(symbol_map, symbol, type_map, data_type, day_map,
              time_unit->full_day(), hour_map, time_unit->exploded().hour,
              minute_map);
}

template<typename MapType, typename MapITType, typename KeyType,
    typename ValType>
LOADERROR FutureManager::GetCompareTimeTickPos(MapType& ss_start_map,
                                               MapType& se_end_map,
                                               KeyType& start_key,
                                               KeyType& end_key,
                                               ValType& start_val,
                                               ValType& end_val) {

  LOADERROR load_error = LOAD_SUCCESS;
  bool r = base::MapGet<MapType, MapITType, KeyType, ValType>(ss_start_map,
                                                              start_key,
                                                              start_val);
  if (!r)
    load_error = START_NOT_EXITS;

  if (start_key == end_key) {
    end_val = start_val;
  } else {
    r = base::MapGet<MapType, MapITType, KeyType, ValType>(se_end_map, end_key,
                                                           end_val);
    if (!r) {
      if (load_error == START_NOT_EXITS)
        load_error = BOTH_NOT_EXITS;
      else
        load_error = END_NOT_EXITS;
    }
  }

  return load_error;
  //return true;
}

bool FutureManager::OnLoadLoaclPos(const std::string& sec,
                                   const std::string& symbol,
                                   const HIS_DATA_TYPE& data_type,
                                   const int32 year, const int32 month,
                                   const int32 day,
                                   MINUTEPOS_MAP& min_pos_map) {
  std::string content;
  bool r = FutureFile::ReadFile(sec, s_stk_type[data_type],
                                ".ipos", symbol, year,
                                month, day, &content);
  if (!r)
    return r;

  const char* raw_data = content.c_str();
  const size_t raw_data_length = content.length();
  size_t pos = 0;
  while (pos < raw_data_length) {
    int16 packet_length = *(int16*) (raw_data + pos);
    std::string packet;
    packet.assign(raw_data + pos + sizeof(int16),
                  packet_length - sizeof(int16));
    pos += packet_length;
    chaos_data::SymbolPosIndex last_pos_index;
    last_pos_index.ParseFromString(packet);
    future_infos::TickTimePos time_pos(last_pos_index.time_index(),
                                       last_pos_index.start_pos(),
                                       last_pos_index.end_pos());
    min_pos_map[last_pos_index.time_index()] = time_pos;
  }

  return r;
}

void FutureManager::SetIndexPos(SYMBOL_MAP& symbol_map,
                                const std::string& symbol_key,
                                DATETYPE_MAP& type_map,
                                const HIS_DATA_TYPE type_key,
                                DAYPOS_MAP& day_map, const int32 day_key,
                                HOURPOS_MAP& hour_map, const int32 hour_key,
                                MINUTEPOS_MAP& minute_map) {
  hour_map[hour_key] = minute_map;
  day_map[day_key] = hour_map;
  type_map[type_key] = day_map;
  symbol_map[symbol_key] = type_map;
}
/*
 bool FutureManager::LoadLocalIndexPosInfo(const std::string& sec,
 const std::string& data_type,
 const std::string& shuffix,
 const std::string& symbol,
 const int32 year, const int32 month,
 const int32 day) {
 std::string content;
 bool r = FutureFile::ReadFile(sec, data_type, shuffix, symbol, year, month,
 day, &content);
 if (!r)
 return r;

 //遍历读取位置信息
 const char* raw_data = content.c_str();
 const size_t raw_data_length = content.length();
 size_t pos = 0;
 while (pos < raw_data_length) {
 int16 packet_length = *(int16*) (raw_data + pos);
 std::string packet;
 packet.assign(raw_data + pos + sizeof(int16),
 packet_length - sizeof(int16));
 pos += packet_length;
 chaos_data::SymbolPosIndex last_pos_index;
 last_pos_index.ParseFromString(packet);
 struct tm tm;
 time_t tick = last_pos_index.time_index();
 char s[100];
 tm = *localtime(&tick);
 strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
 LOG_MSG2("time:%s, start_pos:%d, end_post:%d",
 s,last_pos_index.start_pos(),last_pos_index.end_pos());
 }

 return r;
 }*/

}


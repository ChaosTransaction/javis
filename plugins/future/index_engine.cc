//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "logic/logic_comm.h"
#include "basic/template.h"

#include "index_engine.h"
namespace future_logic {

IndexEngine* IndexEngine::schduler_engine_ = NULL;
IndexManager* IndexEngine::schduler_mgr_ = NULL;

IndexManager::IndexManager() {
  Init();
  InitLock();
}

IndexManager::~IndexManager() {
  DeinitLock();
  Deinit();

}

void IndexManager::Init() {
  index_cache_ = new IndexCache();
  index_lock_ = new IndexLock();
}

void IndexManager::Deinit() {
  if (index_cache_) {
    delete index_cache_;
    index_cache_ = NULL;
  }
  if (index_lock_) {
    delete index_lock_;
    index_lock_ = NULL;
  }
}

void IndexManager::InitLock() {
  InitThreadrw(&index_lock_->zc_future_lock_);
  InitThreadrw(&index_lock_->sc_future_lock_);
  InitThreadrw(&index_lock_->dc_future_lock_);
  InitThreadrw(&index_lock_->zc_ftr_lock_);
  InitThreadrw(&index_lock_->sc_ftr_lock_);
  InitThreadrw(&index_lock_->dc_ftr_lock_);
}

void IndexManager::DeinitLock() {
  DeinitThreadrw(index_lock_->zc_future_lock_);
  DeinitThreadrw(index_lock_->sc_future_lock_);
  DeinitThreadrw(index_lock_->dc_future_lock_);
  DeinitThreadrw(index_lock_->zc_ftr_lock_);
  DeinitThreadrw(index_lock_->sc_ftr_lock_);
  DeinitThreadrw(index_lock_->dc_ftr_lock_);
}

bool IndexManager::OnFetchIndexPos(
    const std::string& sec,  //000001.CZCE
    const std::string& symbol, const HIS_DATA_TYPE& data_type,
    const std::string& start_time, const std::string& end_time,
    future_infos::TickTimePos& start_time_pos,
    future_infos::TickTimePos& end_time_pos) {
  bool r = false;
  if (sec == "CZCE") {
    r = OnFetchIndexPos(index_cache_->zc_future_, index_lock_->zc_future_lock_,
                        sec, symbol, data_type, FUTURE, start_time, end_time,
                        start_time_pos, end_time_pos);
  } else if (sec == "DCE") {
    r = OnFetchIndexPos(index_cache_->dc_future_, index_lock_->dc_future_lock_,
                        sec, symbol, data_type, FUTURE, start_time, end_time,
                        start_time_pos, end_time_pos);
  } else if (sec == "SHFE") {
    r = OnFetchIndexPos(index_cache_->sc_future_, index_lock_->sc_future_lock_,
                        sec, symbol, data_type, FUTURE, start_time, end_time,
                        start_time_pos, end_time_pos);
  }
  return r;
}

bool IndexManager::OnFetchIndexPos(SYMBOL_MAP& symbol_map,
                                   struct threadrw_t* lock,
                                   const std::string& sec,
                                   const std::string& symbol,
                                   const HIS_DATA_TYPE& data_type,
                                   const STK_TYPE& stk_type,
                                   const std::string& start_time,
                                   const std::string& end_time,
                                   future_infos::TickTimePos& start_time_pos,
                                   future_infos::TickTimePos& end_time_pos) {
  future_infos::TimeFrame time_frame(start_time, end_time);
  //bool r = false;
  /*future_infos::TickTimePos start_time_pos;
   future_infos::TickTimePos end_time_pos;*/

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
    base_logic::RLockGd lk(lock);
    load_erron = GetCompareTimeTickPos<SYMBOL_MAP, SYMBOL_MAP::iterator,
        const std::string, const std::string, DATETYPE_MAP>(symbol_map,
                                                            symbol_map, symbol,
                                                            symbol, symbol,
                                                            symbol,
                                                            start_type_pos_map,
                                                            end_type_pos_map);
  }

  if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
    OnLoadIndex(time_frame.start_time(), sec, lock, symbol, data_type, stk_type,
                symbol_map, start_type_pos_map, start_day_pos_map,
                start_hour_pos_map, start_minute_pos_map);
  else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
    OnLoadIndex(time_frame.end_time(), sec, lock, symbol, data_type, stk_type,
                symbol_map, end_type_pos_map, end_day_pos_map, end_hour_pos_map,
                end_minute_pos_map);

  {
    base_logic::RLockGd lk(lock);
    load_erron = GetCompareTimeTickPos<DATETYPE_MAP, DATETYPE_MAP::iterator,
        const HIS_DATA_TYPE, const std::string, DAYPOS_MAP>(start_type_pos_map,
                                                            end_type_pos_map,
                                                            symbol, symbol,
                                                            data_type,
                                                            data_type,
                                                            start_day_pos_map,
                                                            end_day_pos_map);
  }

  if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
    OnLoadIndex(time_frame.start_time(), sec, lock, symbol, data_type, stk_type,
                symbol_map, start_type_pos_map, start_day_pos_map,
                start_hour_pos_map, start_minute_pos_map);
  else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
    OnLoadIndex(time_frame.end_time(), sec, lock, symbol, data_type, stk_type,
                symbol_map, end_type_pos_map, end_day_pos_map, end_hour_pos_map,
                end_minute_pos_map);

  /*{
   base_logic::RLockGd lk(lock);
   load_erron = GetCompareTimeTickPos<DAYPOS_MAP, DAYPOS_MAP::iterator,
   const int32, const HIS_DATA_TYPE, HOURPOS_MAP>(
   start_day_pos_map, end_day_pos_map, data_type, data_type,
   time_frame.start_full_day(), time_frame.end_full_day(),
   start_hour_pos_map, end_hour_pos_map);
   }

   if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
   OnLoadIndex(time_frame.start_time(), sec, lock, symbol, data_type, stk_type,
   symbol_map, start_type_pos_map, start_day_pos_map,
   start_hour_pos_map, start_minute_pos_map);
   else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
   OnLoadIndex(time_frame.end_time(), sec, lock, symbol, data_type, stk_type,
   symbol_map, end_type_pos_map, end_day_pos_map, end_hour_pos_map,
   end_minute_pos_map);*/
  GetCompareDayPos(lock, sec, symbol, data_type, stk_type, start_day_pos_map,
                   end_day_pos_map, time_frame, start_hour_pos_map,
                   end_hour_pos_map);

  SetIndexPos(lock, symbol_map, symbol, start_type_pos_map, data_type,
              start_day_pos_map, time_frame.start_full_day(),
              start_hour_pos_map, time_frame.start_exploded().hour,
              start_minute_pos_map);

  SetIndexPos(lock, symbol_map, symbol, end_type_pos_map, data_type,
              end_day_pos_map, time_frame.end_full_day(), end_hour_pos_map,
              time_frame.end_exploded().hour, end_minute_pos_map);

  {
    base_logic::RLockGd lk(lock);
    load_erron = GetCompareTimeTickPos<HOURPOS_MAP, HOURPOS_MAP::iterator,
        const int32, const int32, MINUTEPOS_MAP>(
        start_hour_pos_map, end_hour_pos_map, time_frame.start_full_day(),
        time_frame.end_full_day(), time_frame.start_exploded().hour,
        time_frame.end_exploded().hour, start_minute_pos_map,
        end_minute_pos_map);
  }

  if (load_erron == BOTH_NOT_EXITS || load_erron == START_NOT_EXITS)
    OnLoadIndex(time_frame.start_time(), sec, lock, symbol, data_type, stk_type,
                symbol_map, start_type_pos_map, start_day_pos_map,
                start_hour_pos_map, start_minute_pos_map);
  else if (load_erron == BOTH_NOT_EXITS || load_erron == END_NOT_EXITS)
    OnLoadIndex(time_frame.end_time(), sec, lock, symbol, data_type, stk_type,
                symbol_map, end_type_pos_map, end_day_pos_map, end_hour_pos_map,
                end_minute_pos_map);

  {
    base_logic::RLockGd lk(lock);
    int32 start_unix_time = time_frame.start_time()->ToUnixTime() / 60 * 60;
    int32 end_unix_time = time_frame.end_time()->ToUnixTime() / 60 * 60;
    load_erron = GetCompareMintuePos(start_minute_pos_map, end_minute_pos_map,
                                     start_unix_time, end_unix_time,
                                     start_time_pos, end_time_pos);
  }
  return true;
}

void IndexManager::OnLoadIndex(future_infos::TimeUnit* time_unit,
                               const std::string& sec, struct threadrw_t* lock,
                               const std::string& symbol,
                               const HIS_DATA_TYPE& data_type,
                               const STK_TYPE& stk_type, SYMBOL_MAP& symbol_map,
                               DATETYPE_MAP& type_map, DAYPOS_MAP& day_map,
                               HOURPOS_MAP& hour_map,
                               MINUTEPOS_MAP& minute_map) {

  OnLoadLoaclPos(sec, symbol, data_type, stk_type, time_unit->exploded().year,
                 time_unit->exploded().month,
                 time_unit->exploded().day_of_month, minute_map);

  SetIndexPos(lock, symbol_map, symbol, type_map, data_type, day_map,
              time_unit->full_day(), hour_map, time_unit->exploded().hour,
              minute_map);
}

LOADERROR IndexManager::GetCompareMintuePos(
    MINUTEPOS_MAP& ss_start_map, MINUTEPOS_MAP& se_end_map,
    const int32& start_key, const int32& end_key,
    future_infos::TickTimePos& start_val, future_infos::TickTimePos& end_val) {
  LOADERROR load_error = LOAD_SUCCESS;
  bool r = false;
  //开始时间向前移40分钟
  int32 start_unix = start_key;
  int32 max_start_unix = start_key - 40 * 60;
  int32 end_unix = end_key;
  int32 max_end_unix = end_key + 40 * 60;
  while (!r && start_unix > max_start_unix) {
    r = base::MapGet<MINUTEPOS_MAP, MINUTEPOS_MAP::iterator, int32,
        future_infos::TickTimePos>(ss_start_map, start_unix, start_val);
    start_unix -= 60;
  }

  if (!r)
    load_error = START_NOT_EXITS;

  if (start_key == end_key) {
    end_val = start_val;
  } else {  //结束时间向后移40分钟
    r = false;
    while (!r && max_end_unix > end_unix) {
      r = base::MapGet<MINUTEPOS_MAP, MINUTEPOS_MAP::iterator, int32,
          future_infos::TickTimePos>(se_end_map, end_unix, end_val);
      end_unix += 60;
    }
    if (!r) {
      if (load_error == START_NOT_EXITS)
        load_error = BOTH_NOT_EXITS;
      else
        load_error = END_NOT_EXITS;
    }
  }

  return load_error;
}

LOADERROR IndexManager::GetCompareDayPos(struct threadrw_t* lock, const std::string& sec,
                           const std::string& symbol,
                           const HIS_DATA_TYPE& data_type,
                           const STK_TYPE& stk_type,
                           DAYPOS_MAP& start_day_pos_map,
                           DAYPOS_MAP& end_day_pos_map,
                           future_infos::TimeFrame& time_frame,
                           HOURPOS_MAP& start_hour_map,
                           HOURPOS_MAP& end_hour_map) {
  LOADERROR load_error = LOAD_SUCCESS;
  bool r = false;
  r = GetDayPos(sec, symbol, data_type, stk_type, start_day_pos_map,
                time_frame.start_time()->ToUnixTime(), start_hour_map);
  if (time_frame.start_full_day() == time_frame.end_full_day()) {
    end_hour_map = start_hour_map;
  } else {
    r = GetDayPos(sec, symbol, data_type, stk_type, end_day_pos_map,
                  time_frame.end_time()->ToUnixTime(), end_hour_map);
  }
  return load_error;
}

bool IndexManager::GetDayPos(struct threadrw_t* lock, const std::string& sec,
                             const std::string& symbol,
                             const HIS_DATA_TYPE& data_type,
                             const STK_TYPE& stk_type, DAYPOS_MAP& day_pos_map,
                             const int64 unix_time,
                             HOURPOS_MAP& start_hour_map) {
  bool r = false;
  int64 start_time = unix_time;
  do {
    future_infos::TimeUnit time_unit(start_time);
    {
      base_logic::RLockGd lk(lock);
      r = base::MapGet<DAYPOS_MAP, DAYPOS_MAP::iterator, const int32,
          HOURPOS_MAP>(day_pos_map, time_unit.full_day(), start_hour_map);
    }
    if (!r)
      r = OnLoadLoaclPos(sec, symbol, data_type, stk_type,
                         time_unit.exploded().year, time_unit.exploded().month,
                         time_unit.exploded().day_of_month, start_hour_map);
    start_time -= 60 * 60 * 24;
  } while (r);
  return true;
}

template<typename MapType, typename MapITType, typename KeyType,
    typename LastKeyType, typename ValType>
LOADERROR IndexManager::GetCompareTimeTickPos(MapType& ss_start_map,
                                              MapType& se_end_map,
                                              LastKeyType& last_start_key,
                                              LastKeyType& last_end_key,
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

  if ((start_key == end_key) && (last_start_key == last_end_key)) {
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

bool IndexManager::OnLoadLoaclPos(const std::string& sec,
                                  const std::string& symbol,
                                  const HIS_DATA_TYPE& data_type,
                                  const STK_TYPE& stk_type, const int32 year,
                                  const int32 month, const int32 day,
                                  MINUTEPOS_MAP& min_pos_map) {
  std::string content;
  //std::string str_data_type = std::string(s_stk_type[data_type]);
  //std::string str_stk_type = std::string(g_stk_type[stk_type]);
  bool r = FutureFile::ReadFile(sec, s_stk_type[data_type],
                                g_stk_type[stk_type], ".ipos", symbol, year,
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

void IndexManager::SetIndexPos(struct threadrw_t* lock, SYMBOL_MAP& symbol_map,
                               const std::string& symbol_key,
                               DATETYPE_MAP& type_map,
                               const HIS_DATA_TYPE type_key,
                               DAYPOS_MAP& day_map, const int32 day_key,
                               HOURPOS_MAP& hour_map, const int32 hour_key,
                               MINUTEPOS_MAP& minute_map) {

  base_logic::WLockGd lk(lock);
  hour_map[hour_key] = minute_map;
  day_map[day_key] = hour_map;
  type_map[type_key] = day_map;
  symbol_map[symbol_key] = type_map;
}
/*
 bool IndexManager::LoadLocalIndexPosInfo(const std::string& sec,
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


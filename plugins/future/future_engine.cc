//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "future_engine.h"
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

bool FutureManager::OnFetchTick(const int socket, const std::string& sec,
                                const std::string& symbol,
                                const std::string& field,
                                const std::string& start_time,
                                const std::string& end_time) {
  future_infos::TimeFrame time_frame(start_time, end_time);
  bool r = false;
  future_infos::TickTimePos start_time_pos;
  future_infos::TickTimePos end_time_pos;

  DAYPOS_MAP start_day_pos_map;
  DAYPOS_MAP end_day_pos_map;
  r = GetCompareTimeTickPos<SYMBOL_MAP, SYMBOL_MAP::iterator, const std::string,
      DAYPOS_MAP>(future_cache_->zc_future_, future_cache_->zc_future_, symbol,
                  symbol, start_day_pos_map, end_day_pos_map);

  HOURPOS_MAP start_hour_pos_map;
  HOURPOS_MAP end_hour_pos_map;
  r = GetCompareTimeTickPos<DAYPOS_MAP, DAYPOS_MAP::iterator, const int32,
      HOURPOS_MAP>(start_day_pos_map, end_day_pos_map,
                   time_frame.start_full_day(), time_frame.end_full_day(),
                   start_hour_pos_map, end_hour_pos_map);

  MINUTEPOS_MAP start_minute_pos_map;
  MINUTEPOS_MAP end_minute_pos_map;
  r = GetCompareTimeTickPos<HOURPOS_MAP, HOURPOS_MAP::iterator, const int32,
      MINUTEPOS_MAP>(start_hour_pos_map, end_hour_pos_map,
                     time_frame.start_hour(), time_frame.end_hour(),
                     start_minute_pos_map, end_minute_pos_map);

  r = GetCompareTimeTickPos<MINUTEPOS_MAP, MINUTEPOS_MAP::iterator, const int32,
      future_infos::TickTimePos>(start_minute_pos_map, end_minute_pos_map,
                                 time_frame.start_minute(),
                                 time_frame.end_minute(), start_time_pos,
                                 end_time_pos);
  return true;
}

template<typename MapType, typename MapITType, typename KeyType,
    typename ValType>
bool FutureManager::GetCompareTimeTickPos(MapType& ss_start_map,
                                          MapType& se_end_map,
                                          KeyType& start_key, KeyType& end_key,
                                          ValType& start_val,
                                          ValType& end_val) {
  bool r = base::MapGet<MapType, MapITType, KeyType, ValType>(ss_start_map,
                                                              start_key,
                                                              start_val);
  if (!r) {  //读取本地信息
    return true;
  }

  if (start_key == end_key) {
    end_val = start_val;
  } else {
    r = base::MapGet<MapType, MapITType, KeyType, ValType>(se_end_map, end_key,
                                                           end_val);
  }
  return r;
  //return true;
}

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
  char* data = NULL;
  size_t pos = 0;
  while (pos < raw_data_length) {
    int16 packet_length = *(int16*) (raw_data + pos);
    std::string packet;
    packet.assgin(raw_data + pos + sizeof(int16),
                  packet_length - sizeof(sizeof(int16)));
    pos += packet_length;
    chaos_data::SymbolPosIndex last_pos_index;
    last_pos_index.ParseFromString(packet);
  }

  return r;
}

}


//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "future_engine.h"

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
  r = GetCompareTimeTickPos<SYMBOL_MAP, std::string, DAYPOS_MAP>
    (future_cache_->zc_future_,future_cache_->zc_future_,
     symbol,symbol,start_day_pos_map,end_day_pos_map);


  HOURPOS_MAP start_hour_pos_map;
  HOURPOS_MAP end_hour_pos_map;
  r = GetCompareTimeTickPos<DAYPOS_MAP,int32,HOURPOS_MAP>
    (start_day_pos_map,end_day_pos_map,time_frame.start_full_day(),
     time_frame.end_full_day(),start_hour_pos_map,end_hour_pos_map);


  MINUTEPOS_MAP start_minute_pos_map;
  MINUTEPOS_MAP end_minute_pos_map;
  r = GetCompareTimeTick<HOURPOS_MAP,int32,MINUTEPOS_MAP>
    (start_hour_pos_map,end_hour_pos_map,time_frame.start_hour(),
     time_frame.end_hour(),start_minute_pos_map,end_minute_pos_map);



  r = GetCompareTimeTick<MINUTEPOS_MAP,int32,future_infos::TickTimePos>
    (start_minute_pos_map,end_minute_pos_map,time_frame.start_minute(),
     time_frame.end_minute(),start_time_pos,end_time_pos);
  /*
  DAYPOS_MAP day_pos_map;
  HOURPOS_MAP hour_pos_map;
  MINUTEPOS_MAP minute_map;
  future_infos::TickTimePos tick_time_pos;
  r = base::MapGet<SYMBOL_MAP, SYMBOL_MAP::iterator, std::string, DAYPOS_MAP>(
      future_cache_->zc_future_, time_frame.start_full_day(), day_pos_map);
  if (!r)
    return false;

  //比对天数

  if (time_frame.start_full_day() == time_frame.end_full_day()) {  //相同日期
    r = base::MapGet<DAYPOS_MAP, DAYPOS_MAP::iterator, int64, HOURPOS_MAP>(
        day_pos_map, time_frame.start_hour(), hour_pos_map);
    if (r)
      return false;

    //比对小时
    if (time_frame.start_hour() == time_frame.end_hour()) {  //相同小时

      r = base::MapGet<HOURPOS_MAP, HOURPOS_MAP::iterator, MINUTEPOS_MAP>(
          hour_pos_map, time_frame.start_minute(), minute_map);

      if (r)
        return false;

      if (time_frame.start_minute()== time_frame.end_minute()){ //相同分钟
        r = base::MapGet<MINUTEPOS_MAP,MINUTEPOS::iterator,future_infos::TickTimePos>
            (minture_map,time_frame.start_minute(),tick_time_pos);

      }
    }else { //不同小时

    }
  } else {  //不同天数

    //开始
    r = base::MapGet<DAYPOS_MAP, DAYPOS_MAP::iterator, int64, HOURPOS_MAP>(
        day_pos_map, time_frame.start_hour(), hour_pos_map);
    if (r)
      return false;

    r = base::MapGet<HOURPOS_MAP, HOURPOS_MAP::iterator, MINUTEPOS_MAP>(
        hour_pos_map, time_frame.start_minute(), minute_map);

    if (r)
      return false;

    //结束
  }*/

  //不存在的话读取本地文件
  return true;
}


template <typename MapType,typename KeyType,typename ValType>
bool FutureManager::GetCompareTimeTickPos(MapType& ss_start_map,MapType& se_end_map,
                      const KeyType start_key,const KeyType end_key,
                      ValType& start_val,ValType& end_val) {
  bool r = base::MapGet<MapType,MapType::iterator,KeyType,ValType>
      (ss_start_map,start_key,start_val);
  if(!r){ //读取本地信息
    return true;
  }

  if(start_time == end_time){
    end_val = start_val;
  }else{
    r = base::MapGet<MapType, MapType::iterator, KeyType, ValType>
      (se_end_map,end_key,end_val);
  }
  return r;
}


}


//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#include "static_engine.h"
#include "future_file.h"
#include "logic/logic_comm.h"
#include "basic/template.h"

namespace future_logic {

StaticEngine* StaticEngine::schduler_engine_ = NULL;
StaticManager* StaticEngine::schduler_mgr_ = NULL;

StaticManager::StaticManager() {
  Init();
  InitLock();
}

StaticManager::~StaticManager() {
  DeinitLock();
  Deinit();
}

void StaticManager::Init() {
  static_cache_ = new StaticCache();
  static_lock_ = new StaticLock();

}

void StaticManager::Deinit() {

  static_cache_->zc_future_.clear();
  static_cache_->zc_ftr_idx_.clear();
  static_cache_->dc_ftr_idx_.clear();
  static_cache_->dc_future_.clear();
  static_cache_->sc_future_.clear();
  static_cache_->sc_ftr_idx_.clear();

  if (static_cache_) {
    delete static_cache_;
    static_cache_ = NULL;
  }
  if (static_lock_) {
    delete static_lock_;
    static_lock_ = NULL;
  }
}

void StaticManager::InitLock() {
  InitThreadrw(&static_lock_->zc_future_lock_);
  InitThreadrw(&static_lock_->sc_future_lock_);
  InitThreadrw(&static_lock_->dc_future_lock_);
  InitThreadrw(&static_lock_->zc_ftr_lock_);
  InitThreadrw(&static_lock_->sc_ftr_lock_);
  InitThreadrw(&static_lock_->dc_ftr_lock_);
}

void StaticManager::DeinitLock() {
  DeinitThreadrw(static_lock_->zc_future_lock_);
  DeinitThreadrw(static_lock_->sc_future_lock_);
  DeinitThreadrw(static_lock_->dc_future_lock_);
  DeinitThreadrw(static_lock_->zc_ftr_lock_);
  DeinitThreadrw(static_lock_->sc_ftr_lock_);
  DeinitThreadrw(static_lock_->dc_ftr_lock_);
}

bool StaticManager::OnFetchStaticInfo(
    const std::string& sec, const std::string& symbol, const STK_TYPE& stk_type,
    future_infos::TickTimePos& start_time_pos,
    future_infos::TickTimePos& end_time_pos,
    std::list<future_infos::StaticInfo>& static_list) {

  bool r = false;
  if (sec == "CZCE") {
    r = OnGetStaticInfo(sec, symbol, static_cache_->zc_future_,
                        static_lock_->zc_future_lock_, stk_type, start_time_pos,
                        end_time_pos, static_list);
  } else if (sec == "DCE") {
    r = OnGetStaticInfo(sec, symbol, static_cache_->dc_future_,
                        static_lock_->zc_future_lock_, stk_type, start_time_pos,
                        end_time_pos, static_list);
  } else if (sec == "SHFE") {
    r = OnGetStaticInfo(sec, symbol, static_cache_->sc_future_,
                        static_lock_->zc_future_lock_, stk_type, start_time_pos,
                        end_time_pos, static_list);
  }

  return r;
}

bool StaticManager::OnGetStaticInfo(
    const std::string& sec, const std::string& symbol,
    SYMBOL_STATIC_MAP& symbol_map, struct threadrw_t* lock,
    const STK_TYPE& stk_type, future_infos::TickTimePos& start_time_pos,
    future_infos::TickTimePos& end_time_pos,
    std::list<future_infos::StaticInfo>& static_list) {

  bool r = false;
  STATIC_MAP static_map;
  {
    base_logic::RLockGd lk(lock);
    r = base::MapGet<SYMBOL_STATIC_MAP, SYMBOL_STATIC_MAP::iterator,
        const std::string, STATIC_MAP>(symbol_map, symbol, static_map);
  }
  //if (!r)
  //    return false;

  std::list<future_infos::TimeUnit> time_unit_list;
 /* int64 start_time = ((start_time_pos.time_index() / 24 / 60 / 60) * 60 * 60
      * 24) - 8 * 60 * 60;
  int64 end_time = ((end_time_pos.time_index() / 24 / 60 / 60) * 60 * 60 * 24)
      - 8 * 60 * 60;*/
  future_infos::TimeUnit start_unit_time(start_time_pos.market_date());
  future_infos::TimeUnit end_unit_time(end_time_pos.market_date());
  int64 start_time = start_unit_time.ToUnixTime();
  int64 end_time = end_unit_time.ToUnixTime();
  //future_infos::TimeUnit s_time_unit(start_time);
  //time_unit_list.push_back(s_time_unit);
  while (start_time <= end_time) {
    future_infos::TimeUnit time_unit(start_time);
    time_unit_list.push_back(time_unit);
    start_time += 60 * 60 * 24;
  }

  //future_infos::TimeUnit e_time_unit(start_time);
  //time_unit_list.push_back(e_time_unit);
  GetStaticInfo(static_map, symbol, sec, stk_type, lock, time_unit_list,
                static_list);
  {
    base_logic::WLockGd lk(lock);
    symbol_map[symbol] = static_map;
  }
  return true;
}

void StaticManager::GetStaticInfo(
    STATIC_MAP& static_map, const std::string& symbol, const std::string& sec,
    const STK_TYPE& stk_type, struct threadrw_t* lock,
    std::list<future_infos::TimeUnit>& time_list,
    std::list<future_infos::StaticInfo>& static_list) {
  bool r = false;
  while (time_list.size() > 0) {
    future_infos::TimeUnit time_unit = time_list.front();
    time_list.pop_front();
    future_infos::StaticInfo static_info;
    {
      base_logic::RLockGd lk(lock);
      r = base::MapGet<STATIC_MAP, STATIC_MAP::iterator, int32,
          future_infos::StaticInfo>(static_map, time_unit.full_day(),
                                    static_info);
    }

    if (!r) {
      std::string content;
      bool ret = future_logic::FutureFile::ReadStaticFile(
          sec, symbol, g_stk_type[stk_type], time_unit.exploded().year,
          time_unit.exploded().month, time_unit.exploded().day_of_month,
          &content);
      if (ret) {
        future_infos::StaticInfo stc_info(content);
        {
          base_logic::WLockGd lk(lock);
          static_map[time_unit.full_day()] = stc_info;
          static_list.push_back(stc_info);
        }
      }
    } else {
      static_list.push_back(static_info);
    }
  }
}

}

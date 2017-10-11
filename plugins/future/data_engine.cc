//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月9日 Author: kerry

#include "logic/logic_comm.h"
#include "basic/template.h"

#include "data_engine.h"
namespace future_logic {

DataEngine* DataEngine::schduler_engine_ = NULL;
DataManager* DataEngine::schduler_mgr_ = NULL;

DataManager::DataManager() {
  Init();
  InitLock();
}

DataManager::~DataManager() {
  DeinitLock();
  Deinit();

}

void DataManager::Init() {
  data_cache_ = new DataCache();
  data_lock_ = new DataLock();
}

void DataManager::Deinit() {
  if (data_cache_) {
    delete data_cache_;
    data_cache_ = NULL;
  }
  if (data_lock_) {
    delete data_lock_;
    data_lock_ = NULL;
  }
}

void DataManager::InitLock() {
  InitThreadrw(&data_lock_->zc_future_lock_);
  InitThreadrw(&data_lock_->sc_future_lock_);
  InitThreadrw(&data_lock_->dc_future_lock_);
  InitThreadrw(&data_lock_->zc_ftr_lock_);
  InitThreadrw(&data_lock_->sc_ftr_lock_);
  InitThreadrw(&data_lock_->dc_ftr_lock_);
}

void DataManager::DeinitLock() {
  DeinitThreadrw(data_lock_->zc_future_lock_);
  DeinitThreadrw(data_lock_->sc_future_lock_);
  DeinitThreadrw(data_lock_->dc_future_lock_);
  DeinitThreadrw(data_lock_->zc_ftr_lock_);
  DeinitThreadrw(data_lock_->sc_ftr_lock_);
  DeinitThreadrw(data_lock_->dc_ftr_lock_);
}

bool DataManager::OnLoadData(
    const HIS_DATA_TYPE& data_type, const STK_TYPE& stk_type,
    std::list<future_infos::StaticInfo>& static_list,
    std::map<int32, future_infos::DayMarket>& market_hash) {
  bool r = false;
  for (std::list<future_infos::StaticInfo>::iterator it = static_list.begin();
      it != static_list.end(); it++) {
    future_infos::StaticInfo static_info = (*it);
    const std::string sec = static_info.static_info().market_mtk();
    const std::string symbol = static_info.static_info().symbol();
    int32 market_date = static_info.static_info().market_date();
    future_infos::DayMarket dym;
    if (sec == "CZCE") {
      r = LoadData(sec, symbol, market_date, data_lock_->zc_future_lock_,
                   data_type, stk_type, data_cache_->zc_future_, dym);
    } else if (sec == "DCE") {
      r = LoadData(sec, symbol, market_date, data_lock_->dc_future_lock_,
                   data_type, stk_type, data_cache_->dc_future_, dym);
    } else if (sec == "SHFE") {
      r = LoadData(sec, symbol, market_date, data_lock_->sc_future_lock_,
                   data_type, stk_type, data_cache_->sc_future_, dym);
    }
    market_hash[market_date] = dym;
  }
  return r;
}

bool DataManager::LoadData(const std::string& sec, const std::string& symbol,
                           int32& market_date, struct threadrw_t* lock,
                           const HIS_DATA_TYPE& data_type,
                           const STK_TYPE& stk_type, DAYSYMBOL_MAP& symbol_map,
                           future_infos::DayMarket& dym) {
  bool r = false;
  DAYTYPE_MAP day_type_map;
  DAY_MARKET_MAP day_market_map;

  {
    base_logic::RLockGd lk(lock);
    r = base::MapGet<DAYSYMBOL_MAP, DAYSYMBOL_MAP::iterator, std::string,
        DAYTYPE_MAP>(symbol_map, symbol, day_type_map);
  }
  if (!r)
    LoadData(sec, symbol, data_type, stk_type, market_date, lock, symbol_map,
             day_type_map, day_market_map, dym);

  {
    base_logic::RLockGd lk(lock);
    r = base::MapGet<DAYTYPE_MAP, DAYTYPE_MAP::iterator, HIS_DATA_TYPE,
        DAY_MARKET_MAP>(day_type_map, data_type, day_market_map);
  }
  if (!r)
    LoadData(sec, symbol, data_type, stk_type, market_date, lock, symbol_map,
             day_type_map, day_market_map, dym);
  {
    base_logic::RLockGd lk(lock);
    r = base::MapGet<DAY_MARKET_MAP, DAY_MARKET_MAP::iterator, int32,
        future_infos::DayMarket>(day_market_map, data_type, dym);
  }
  if (!r)
    r = LoadData(sec, symbol, data_type, stk_type, market_date, lock,
                 symbol_map, day_type_map, day_market_map, dym);
  return r;
}

bool DataManager::Test(const HIS_DATA_TYPE& data_type, const STK_TYPE& stk_type,
                       std::list<future_infos::StaticInfo>& static_list) {

  /*  std::map<int32, future_infos::DayMarket> market_hash;
   while (static_list.size() > 0) {
   future_infos::StaticInfo static_info = static_list.front();
   static_list.pop_front();
   //读取数据
   bool r = false;
   std::string sec = static_info.static_info().market_mtk();
   std::string symbol = static_info.static_info().symbol();
   int32 market_date = static_info.static_info().market_date();

   {
   DAYTYPE_MAP day_type_map;
   DAY_MARKET_MAP day_market_map;
   future_infos::DayMarket dymarket;

   {
   base_logic::RLockGd lk(data_lock_->zc_future_lock_);
   r = base::MapGet<DAYSYMBOL_MAP, DAYSYMBOL_MAP::iterator, std::string,
   DAYTYPE_MAP>(data_cache_->zc_future_, symbol, day_type_map);
   }
   if (!r)
   LoadData(sec, symbol, data_type, stk_type, market_date,
   data_lock_->zc_future_lock_, data_cache_->zc_future_,
   day_type_map, day_market_map, dymarket);

   {
   base_logic::RLockGd lk(data_lock_->zc_future_lock_);
   r = base::MapGet<DAYTYPE_MAP, DAYTYPE_MAP::iterator, HIS_DATA_TYPE,
   DAY_MARKET_MAP>(day_type_map, data_type, day_market_map);
   }
   if (!r)
   LoadData(sec, symbol, data_type, stk_type, market_date,
   data_lock_->zc_future_lock_, data_cache_->zc_future_,
   day_type_map, day_market_map, dymarket);
   {
   base_logic::RLockGd lk(data_lock_->zc_future_lock_);
   r = base::MapGet<DAY_MARKET_MAP, DAY_MARKET_MAP::iterator, int32,
   future_infos::DayMarket>(day_market_map, data_type, dymarket);
   }
   if (!r)
   r = LoadData(sec, symbol, data_type, stk_type, market_date,
   data_lock_->zc_future_lock_, data_cache_->zc_future_,
   day_type_map, day_market_map, dymarket);

   if (r)
   market_hash[market_date] = dymarket;
   }
   }*/
  return true;
}

bool DataManager::LoadLocal(const std::string& sec, const std::string& symbol,
                            const HIS_DATA_TYPE& data_type,
                            const STK_TYPE& stk_type, int32 market_date,
                            std::string& content) {
  bool r = FutureFile::ReadFile(sec, s_stk_type[data_type],
                                g_stk_type[stk_type],
                                g_his_data_suffix[data_type], symbol,
                                market_date / 10000, (market_date / 100) % 100,
                                market_date % 100, &content);
  return r;
}

bool DataManager::LoadData(const std::string& sec, const std::string& symbol,
                           const HIS_DATA_TYPE& data_type,
                           const STK_TYPE& stk_type, int32 market_date,
                           struct threadrw_t* lock,
                           DAYSYMBOL_MAP& daysymbol_map,
                           DAYTYPE_MAP& day_type_map,
                           DAY_MARKET_MAP& day_market_map,
                           future_infos::DayMarket& dymarket) {
  std::string content;
  bool r = LoadLocal(sec, symbol, data_type, stk_type, market_date, content);
  if (r) {
    base_logic::WLockGd lk(lock);
    future_infos::DayMarket temp_dymarket(market_date, content);
    dymarket = temp_dymarket;
    day_market_map[market_date] = temp_dymarket;
    day_type_map[data_type] = day_market_map;
    data_cache_->zc_future_[symbol] = day_type_map;
  }
  return r;
}

}


//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月9日 Author: kerry
#ifndef FUTURE_INDEX_ENGINE_H_
#define FUTURE_INDEX_ENGINE_H_

#include "future_infos.h"
#include "future_file.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"

namespace future_logic {
typedef std::map<int32, future_infos::DayMarket> DAY_MARKET_MAP;  //20150911--DayMarket
typedef std::map<HIS_DATA_TYPE, DAY_MARKET_MAP> DAYTYPE_MAP;
typedef std::map<std::string, DAYTYPE_MAP> DAYSYMBOL_MAP;

class DataCache {
 public:
  DAYSYMBOL_MAP zc_future_;  //郑州期货
  DAYSYMBOL_MAP sc_future_;  //上海期货
  DAYSYMBOL_MAP dc_future_;  //大连期货
  DAYSYMBOL_MAP zc_ftr_idx_;  //郑州期指
  DAYSYMBOL_MAP sc_ftr_idx_;  //上海期指
  DAYSYMBOL_MAP dc_ftr_idx_;  //大连期指
};

class DataLock {
 public:
  struct threadrw_t* zc_future_lock_;
  struct threadrw_t* sc_future_lock_;
  struct threadrw_t* dc_future_lock_;
  struct threadrw_t* zc_ftr_lock_;
  struct threadrw_t* sc_ftr_lock_;
  struct threadrw_t* dc_ftr_lock_;
};

class DataManager {
  friend class Futurelogic;
 protected:
  DataManager();
  virtual ~DataManager();
 protected:
  bool Test();
 private:
  bool LoadData(const std::string& sec,
                const std::string& symbol,
                const HIS_DATA_TYPE& data_type,
                const STK_TYPE& stk_type,
                int32 market_date,
                struct threadrw_t* lock,
                DAYSYMBOL_MAP& daysymbol_map,
                DAYTYPE_MAP day_type_map,
                DAY_MARKET_MAP day_market_map,
                future_infos::DayMarket dymarket
                );

  bool LoadLocal(const std::string& sec,
                 const std::string& symbol,
                 const HIS_DATA_TYPE& data_type,
                 const STK_TYPE& stk_type,
                 int32 market_date,
                 std::string& content);
 private:
  void Init();
  void Deinit();
  void InitLock();
  void DeinitLock();
 private:
  DataLock* data_lock_;
  DataCache* data_cache_;
};

class DataEngine {
  friend class Futurelogic;
 private:
  static DataEngine* schduler_engine_;
  static DataManager* schduler_mgr_;

 protected:
  DataEngine() {
  }
  virtual ~DataEngine() {
  }

 protected:
  static DataManager* GetSchdulerManager() {
    if (schduler_mgr_ == NULL)
      schduler_mgr_ = new DataManager();
    return schduler_mgr_;
  }

  static DataEngine* GetIndexEngine() {
    if (schduler_engine_ == NULL)
      schduler_engine_ = new DataEngine();
    return schduler_engine_;
  }

  static void FreeSchdulerManager() {
    if (schduler_mgr_) {
      delete schduler_mgr_;
      schduler_mgr_ = NULL;
    }
  }

  static void FreeDataEngine() {
    if (schduler_engine_) {
      delete schduler_engine_;
      schduler_engine_ = NULL;
    }
  }
};

}

#endif /* PLUGINS_FUTURE_DATA_ENGINE_H_ */

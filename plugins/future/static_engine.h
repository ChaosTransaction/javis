//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#ifndef FUTURE_STATIC_ENGINE_H_
#define FUTURE_STATIC_ENGINE_H_

#include "future_infos.h"
#include <map>
namespace future_logic {

typedef std::map<int32, future_infos::StaticInfo> STATIC_MAP;  //完整日期 20150112
typedef std::map<const std::string, STATIC_MAP> SYMBOL_STATIC_MAP;

class StaticCache {
 public:
  SYMBOL_STATIC_MAP zc_future_;  //郑州期货
  SYMBOL_STATIC_MAP sc_future_;  //上海期货
  SYMBOL_STATIC_MAP dc_future_;  //大连期货
  SYMBOL_STATIC_MAP zc_ftr_idx_;  //郑州期指
  SYMBOL_STATIC_MAP sc_ftr_idx_;  //上海期指
  SYMBOL_STATIC_MAP dc_ftr_idx_;  //大连期指
};

class StaticLock {
 public:
  struct threadrw_t* zc_future_lock_;
  struct threadrw_t* sc_future_lock_;
  struct threadrw_t* dc_future_lock_;
  struct threadrw_t* zc_ftr_lock_;
  struct threadrw_t* sc_ftr_lock_;
  struct threadrw_t* dc_ftr_lock_;
};

class StaticManager {
 protected:
  StaticManager();
  virtual ~StaticManager();

 protected:
  bool OnFetchStaticInfo(const std::string& sec, const std::string& symbol,
                         const STK_TYPE& stk_type,
                         future_infos::TickTimePos& start_time_pos,
                         future_infos::TickTimePos& end_time_pos,
                         std::list<future_infos::StaticInfo>& static_list);

  bool OnGetStaticInfo(const std::string& symbol,
                       const std::string& sec,
                       SYMBOL_STATIC_MAP& symbol_map,
                       struct threadrw_t* lock, const STK_TYPE& stk_type,
                       future_infos::TickTimePos& start_time_pos,
                       future_infos::TickTimePos& end_time_pos,
                       std::list<future_infos::StaticInfo>& static_list,
                       std::list<future_infos::StaticInfo>& static_list);

  void GetStaticInfo(STATIC_MAP& static_map,
                     const std::string& symbol,
                     const std::string& sec,
                     const STK_TYPE& stk_type,
                     struct threadrw_t* lock,
                     std::list<future_infos::TimeUnit>& time_unit_list);
 private:
  void Init();
  void Deinit();
  void InitLock();
  void DeinitLock();
 private:
  StaticCache* static_cache_;
  StaticLock* static_lock_;
};

class StaticEngine {
  friend class Futurelogic;
 private:
  static StaticEngine* schduler_engine_;
  static StaticManager* schduler_mgr_;

 protected:
  StaticEngine() {
  }
  virtual ~StaticEngine() {
  }

 protected:
  static StaticManager* GetSchdulerManager() {
    if (schduler_mgr_ == NULL)
      schduler_mgr_ = new IndexManager();
    return schduler_mgr_;
  }

  static StaticEngine* GetIndexEngine() {
    if (schduler_engine_ == NULL)
      schduler_engine_ = new IndexEngine();
    return schduler_engine_;
  }

  static void FreeSchdulerManager() {
    if (schduler_mgr_) {
      delete schduler_mgr_;
      schduler_mgr_ = NULL;
    }
  }

  static void FreeIndexEngine() {
    if (schduler_engine_) {
      delete schduler_engine_;
      schduler_engine_ = NULL;
    }
  }
};

}

#endif /* PLUGINS_FUTURE_STATIC_ENGINE_H_ */

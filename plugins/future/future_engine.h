//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef FUTURE_FUTURE_ENGINE_H_
#define FUTURE_FUTURE_ENGINE_H_

#include "future_infos.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"
#include <map>

namespace future_logic {

typedef std::map<int64, future_infos::TickTimePos> MINUTEPOS_MAP;  //每分钟行情的位置 /key:分钟
typedef std::map<int64, MINUTEPOS_MAP> HOURPOS_MAP;  //每小时行情位置 /key:小时
typedef std::map<int64, HOURPOS_MAP> DAYPOS_MAP;  //每天行情位置 //key:天
typedef std::map<std::string, DAYPOS_MAP> SYMBOL_MAP;  //合约行情 //key:合约代码
//typedef std::map<std::string, SYMBOL_MAP> MKT_MAP;//交易所行情 //交易所编号

class FutureCache {
 public:
  SYMBOL_MAP zc_future_;  //郑州期货
  SYMBOL_MAP sc_future_;  //上海期货
  SYMBOL_MAP dc_future_;  //大连期货
  SYMBOL_MAP zc_ftr_idx_;  //郑州期指
  SYMBOL_MAP sc_ftr_idx_;  //上海期指
  SYMBOL_MAP dc_ftr_idx_;  //大连期指

};

class FutureLock {
 public:
  struct threadrw_t* zc_future_lock_;
  struct threadrw_t* sc_future_lock_;
  struct threadrw_t* dc_future_lock_;
  struct threadrw_t* zc_ftr_lock_;
  struct threadrw_t* sc_ftr_lock_;
  struct threadrw_t* dc_ftr_lock_;
};

class FutureManager {
  friend class FutureEngine;
 protected:
  FutureManager();
  virtual ~FutureManager();
 private:
  void Init();
  void Deinit();
  void InitLock();
  void DeinitLock();
 private:
  FutureCache* future_cache_;
  FutureLock* future_lock_;
};

class FutureEngine {
  friend class Futurelogic;
 private:
  static FutureEngine* schduler_engine_;
  static FutureManager* schduler_mgr_;

 protected:
  FutureEngine() {
  }
  virtual ~FutureEngine() {
  }

 protected:
  static FutureManager* GetSchdulerManager() {
    if (schduler_mgr_ == NULL)
      schduler_mgr_ = new FutureManager();
    return schduler_mgr_;
  }

  static FutureEngine* GetFutureEngine() {
    if (schduler_engine_ == NULL)
      schduler_engine_ = new FutureEngine();
    return schduler_engine_;
  }

  static void FreeSchdulerManager() {
    if (schduler_mgr_) {
      delete schduler_mgr_;
      schduler_mgr_ = NULL;
    }
  }

  static void FreeFutureEngine() {
    if (schduler_engine_) {
      delete schduler_engine_;
      schduler_engine_ = NULL;
    }
  }
};
}

#endif /* FUTURE_FUTURE_ENGINE_H_ */

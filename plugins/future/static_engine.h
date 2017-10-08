//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#ifndef FUTURE_STATIC_ENGINE_H_
#define FUTURE_STATIC_ENGINE_H_

#include "future_infos.h"
#include <map>
namespace future_logic {

typedef std::map<int32, future_infos::StaticInfo> STATIC_MAP;
typedef std::map<std::string, STATIC_MAP> SYMBOL_STATIC_MAP;

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
  bool OnFetchStaticInfo(future_infos::TimeFrame& time_frame,
                         std::list<future_infos::StaticInfo>& static_list);
 private:
  void Init();
  void Deinit();
  void InitLock();
  void DeinitLock();
 private:
  StaticCache* static_cache_;
  StaticLock*  static_lock_;
};

}

#endif /* PLUGINS_FUTURE_STATIC_ENGINE_H_ */

//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "future_engine.h"

namespace future_logic{

FutureEngine*  FutureEngine::schduler_engine_ = NULL;
FutureManager* FutureEngine::schduler_mgr_ = NULL;

FutureManager::FutureManager(){
  Init();
  InitLock();
}

FutureManager::~FutureManager(){
  Deinit();
  DeinitLock();
}

void FutureManager::Init(){
  future_cache_ = new FutureCache();
  future_lock_ = new FutureLock();
}

void FutureManager::Deinit(){
  if (future_cache_) {delete future_cache_; future_cache_ = NULL;}
  if (future_lock_) {delete future_lock_; future_lock_ = NULL;}
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


}




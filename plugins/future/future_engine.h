//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#ifndef FUTURE_FUTURE_ENGINE_H_
#define FUTURE_FUTURE_ENGINE_H_

#include "future_infos.h"
#include "basic/basictypes.h"
#include <string>
#include <list>
namespace future_logic {

class FutureManager {
  friend class Futurelogic;
  friend class FutureEngine;
 protected:
  FutureManager();
  virtual ~FutureManager();
 protected:
  bool OnDynaTick(const int64 uid, const std::string& token,
                  const std::string&field, const std::string& sec_symbol,
                  const STK_TYPE& stk_type,const std::string& start_time, const std::string& end_time);
};

class FutureEngine {
  friend class Futurelogic;
 private:
  static FutureEngine* schduler_engine_;
  static FutureManager* schduler_mgr_;

 protected:
  FutureEngine(){}
  virtual ~FutureEngine(){}

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

#endif /* PLUGINS_FUTURE_FUTURE_ENGINE_H_ */

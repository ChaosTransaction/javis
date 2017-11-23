//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#ifndef FUTURE_FUTURE_ENGINE_H_
#define FUTURE_FUTURE_ENGINE_H_

#include "future_infos.h"
#include "basic/basictypes.h"
#include "future_value_buf.h"
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
  bool OnDynaTick(const int socket, const int64 uid, const std::string& token,
                  const std::string& field, const int32 net_code,
                  const std::string& sec_symbol, const STK_TYPE& stk_type,
                  const std::string& start_time, const std::string& end_time);

  bool OnDynaFile(const int socket, const int64 uid, const std::string& token,
                  const std::string& filed, const int32 net_code,
                  const std::string& sec_symbol, const STK_TYPE& stk_type,
                  const std::string& start_time, const std::string& endt_time);

  bool OnBasicFuture(const int socket, const int64 uid,
                     const std::string& token, const std::string& field,
                     const std::string& sec, const std::string& exchange,
                     FutureStatus& status);

 private:
  bool ExtractDynamMarket(
      const int socket, future_infos::TickTimePos& start_pos,
      future_infos::TickTimePos& end_pos,
      std::list<future_infos::StaticInfo>& static_list,
      std::map<int32, future_infos::DayMarket>& market_hash);

  bool CalcuDynamMarket(const char* raw_data, const size_t raw_data_length,
                        future_infos::StaticInfo& static_info,
                        std::list<chaos_data::SymbolDynamMarket>& dynam_list);

  bool WriteDynamMarket(future_infos::TickTimePos& start_pos,
                        future_infos::TickTimePos& end_pos,
                        std::list<future_infos::StaticInfo>& static_list,
                        std::map<int32, future_infos::DayMarket>& market_hash,
                        );

  bool SendDynamMarket(future_infos::TickTimePos& start_pos,
                       future_infos::TickTimePos& end_pos,
                       const int32 max_count,
                       std::list<future_infos::StaticInfo>& static_list,
                       std::map<int32, future_infos::DayMarket>& market_hash,
                       net_reply::DynaTick& dyna_tick);

  bool CalcuDynamMarket(const char* raw_data, const size_t raw_data_length,
                        int32& index, const int32 max_count,
                        future_infos::StaticInfo& static_info,
                        net_reply::DynaTick& dyna_tick);

  bool CalcuDynamMarket(const char* raw_data, const size_t raw_data_length,
                        future_infos::StaticInfo& static_info,
                        net_reply::DynaTick& dyna_tick);

  int GetPriceMul(const uint8 price_digit) const;
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

#endif /* PLUGINS_FUTURE_FUTURE_ENGINE_H_ */

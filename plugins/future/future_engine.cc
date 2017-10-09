//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年10月8日 Author: kerry

#include "future_engine.h"
#include "index_engine.h"
#include "data_engine.h"
#include "static_engine.h"
#include "logic/logic_comm.h"
namespace future_logic {

FutureEngine* FutureEngine::schduler_engine_ = NULL;
FutureManager* FutureEngine::schduler_mgr_ = NULL;

FutureManager::FutureManager() {
  IndexEngine::GetSchdulerManager();
  IndexEngine::GetIndexEngine();

  StaticEngine::GetSchdulerManager();
  StaticEngine::GetIndexEngine();

  DataEngine::GetSchdulerManager();
  DataEngine::GetDataEngine();
}

FutureManager::~FutureManager() {
  IndexEngine::FreeIndexEngine();
  IndexEngine::FreeSchdulerManager();

  StaticEngine::FreeIndexEngine();
  StaticEngine::FreeSchdulerManager();
}

bool FutureManager::OnDynaTick(const int64 uid, const std::string& token,
                               const std::string& field,
                               const std::string& sec_symbol,
                               const STK_TYPE& stk_type,
                               const std::string& start_time,
                               const std::string& end_time) {
  bool r = false;
  HIS_DATA_TYPE data_type = _DYNA_DATA;
  size_t start_pos = sec_symbol.find(".");
  std::string symbol = sec_symbol.substr(0, start_pos);
  std::string sec = sec_symbol.substr(
      start_pos + 1, sec_symbol.length() - symbol.length() - 1);

  future_infos::TickTimePos start_time_pos;
  future_infos::TickTimePos end_time_pos;

  r = IndexEngine::GetSchdulerManager()->OnFetchIndexPos(sec, symbol, data_type,
                                                         start_time, end_time,
                                                         start_time_pos,
                                                         end_time_pos);
  if (!r)
    return false;

  std::list<future_infos::StaticInfo> static_list;
  r = StaticEngine::GetSchdulerManager()->OnFetchStaticInfo(sec, symbol, FUTURE,
                                                            start_time_pos,
                                                            end_time_pos,
                                                            static_list);

  if (!r)
    return false;

  std::map<int32, future_infos::DayMarket> market_hash;
  r = DataEngine::GetSchdulerManager()->OnLoadData(data_type, stk_type,
                                                   static_list, market_hash);

  ULOG_DEBUG2("%d", market_hash.size());
}

}


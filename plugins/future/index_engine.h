//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef FUTURE_INDEX_ENGINE_H_
#define FUTURE_INDEX_ENGINE_H_

#include "future_infos.h"
#include "future_file.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"
#include "proto/symbol_dynam_market.pb.h"
#include "proto/symbol_pos_index.pb.h"
#include <map>

namespace future_logic {

typedef std::map<int32, future_infos::TickTimePos> MINUTEPOS_MAP;  //每分钟行情的位置 /key:分钟
typedef std::map<int32, MINUTEPOS_MAP> HOURPOS_MAP;  //每小时行情位置 /key:小时
typedef std::map<int32, HOURPOS_MAP> DAYPOS_MAP;  //每天行情位置 //key:天20150608
typedef std::map<HIS_DATA_TYPE, DAYPOS_MAP> DATETYPE_MAP;  //数据类型 参照 g_his_data_type_en
typedef std::map<std::string, DATETYPE_MAP> SYMBOL_MAP;  //合约行情 //key:合约代码
//typedef std::map<std::string, SYMBOL_MAP> MKT_MAP;//交易所行情 //交易所编号

enum LOADERROR {
  BOTH_NOT_EXITS = -3,
  START_NOT_EXITS = -2,
  END_NOT_EXITS = -1,
  LOAD_SUCCESS = 0
};

class IndexCache {
 public:
  SYMBOL_MAP zc_future_;  //郑州期货
  SYMBOL_MAP sc_future_;  //上海期货
  SYMBOL_MAP dc_future_;  //大连期货
  SYMBOL_MAP zc_ftr_idx_;  //郑州期指
  SYMBOL_MAP sc_ftr_idx_;  //上海期指
  SYMBOL_MAP dc_ftr_idx_;  //大连期指

};

class IndexLock {
 public:
  struct threadrw_t* zc_future_lock_;
  struct threadrw_t* sc_future_lock_;
  struct threadrw_t* dc_future_lock_;
  struct threadrw_t* zc_ftr_lock_;
  struct threadrw_t* sc_ftr_lock_;
  struct threadrw_t* dc_ftr_lock_;
};

class IndexManager {
  friend class IndexEngine;
  friend class Futurelogic;
 protected:
  IndexManager();
  virtual ~IndexManager();
 protected:
  bool OnFetchIndexPos(const std::string& sec_symbol, //000001.CZCE
                       const HIS_DATA_TYPE& data_type,
                       const std::string& start_time,
                       const std::string& end_time);
 private:
  template<typename MapType, typename MapITType, typename KeyType,
      typename ValType>
  LOADERROR GetCompareTimeTickPos(MapType& ss_start_map, MapType& se_end_map,
                                  KeyType& start_key, KeyType& end_key,
                                  ValType& start_val, ValType& end_val);

  LOADERROR GetCompareMintuePos(MINUTEPOS_MAP& ss_start_map,
                                MINUTEPOS_MAP& se_end_map,
                                const int32& start_key, const int32& end_key,
                                future_infos::TickTimePos& start_val,
                                future_infos::TickTimePos& end_val);

  void OnLoadIndex(future_infos::TimeUnit* time_unit, const std::string& sec, 
                   struct threadrw_t* lock, const std::string& symbol,
                   const HIS_DATA_TYPE& data_type, const STK_TYPE& stk_type,
                   SYMBOL_MAP& symbol_map, DATETYPE_MAP& type_map,
                   DAYPOS_MAP& day_map, HOURPOS_MAP& hour_map,
                   MINUTEPOS_MAP& minute_map);

  bool OnLoadLoaclPos(const std::string& sec, const std::string& symbol,
                      const HIS_DATA_TYPE& data_type, const STK_TYPE& stk_type,
                      const int32 year, const int32 month, const int32 day,
                      MINUTEPOS_MAP& min_pos_map);

  void SetIndexPos(struct threadrw_t* lock, SYMBOL_MAP& symbol_map,
                   const std::string& symbol_key, DATETYPE_MAP& type_map,
                   const HIS_DATA_TYPE type_key, DAYPOS_MAP& day_map,
                   const int32 day_key, HOURPOS_MAP& hour_map,
                   const int32 hour_key, MINUTEPOS_MAP& minute_map);

  bool OnFetchIndexPos(SYMBOL_MAP& symbol_map, struct threadrw_t* lock,
                       const std::string& sec, const std::string& symbol,
                       const HIS_DATA_TYPE& data_type, const STK_TYPE& stk_type,
                       const std::string& start_time,
                       const std::string& end_time);

  /*bool LoadLocalIndexPosInfo(const std::string& sec,
   const std::string& data_type,
   const std::string& shuffix,
   const std::string& symbol, const int32 year,
   const int32 month, const int32 day);*/
 private:
  void Init();
  void Deinit();
  void InitLock();
  void DeinitLock();
 private:
  IndexCache* index_cache_;
  IndexLock* index_lock_;
};

class IndexEngine {
  friend class Futurelogic;
 private:
  static IndexEngine* schduler_engine_;
  static IndexManager* schduler_mgr_;

 protected:
  IndexEngine() {
  }
  virtual ~IndexEngine() {
  }

 protected:
  static IndexManager* GetSchdulerManager() {
    if (schduler_mgr_ == NULL)
      schduler_mgr_ = new IndexManager();
    return schduler_mgr_;
  }

  static IndexEngine* GetIndexEngine() {
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

#endif /* FUTURE_FUTURE_ENGINE_H_ */

//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#include "future_infos.h"

const char* s_stk_type[HIS_DATA_TYPE_COUNT] = { "STATIC", "DYNA", "L2_MMPEX",
    "L2_REPORT", "L2_ORDER_STAT", "HIS_IOPV", "MatuYld" };

const char* g_his_data_suffix[HIS_DATA_TYPE_COUNT] = { ".stc", ".dya", ".l2mx",
    ".l2rt", ".l2ost", ".iopv", ".mad" };

const char* g_stk_type[STK_TYPE_COUNT] = { "index", "stock", "fund", "bond",
    "other_stock", "option", "exchange", "future", "ftr_idx", "RGZ", "ETF",
    "LOF", "cov_bond", "trust", "warrant", "repo", "comm" };
/*
 const char* g_sec[16] = { "SHFE", "DCE", "CZCE", "CFFEX", "CME", "CBOT", "LME",
 "LIFFE", "NYBOT", "ICE", "Eurex", "TOCOM", "SGX", "SICOM", "BMD", "HKEX" };
 */

namespace future_infos {

    StaticInfo::StaticInfo() {
        data_ = new Data();
    }
StaticInfo::StaticInfo(const std::string& str) {
  data_ = new Data(str);
}

StaticInfo::StaticInfo(const StaticInfo& static_info)
    : data_(static_info.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

StaticInfo& StaticInfo::operator =(const StaticInfo& static_info) {
  if (static_info.data_ != NULL) {
    static_info.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }

  data_ = static_info.data_;
  return (*this);
}

TimeUnit::TimeUnit(const std::string& str_time) {
  data_ = new Data(str_time);
}

TimeUnit::TimeUnit(const TimeUnit& time_unit)
    : data_(time_unit.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

    TimeUnit::TimeUnit(const int64 unix_time){
        data_ = new Data(unix_time);;    
    }

TimeUnit& TimeUnit::operator =(const TimeUnit& time_unit) {
  if (time_unit.data_ != NULL) {
    time_unit.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }

  data_ = time_unit.data_;
  return (*this);
}

TimeFrame::TimeFrame(const std::string& start_time,
                     const std::string& end_time) {
  data_ = new Data(start_time, end_time);
}

TimeFrame::TimeFrame(const TimeFrame& time_frame)
    : data_(time_frame.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

TimeFrame& TimeFrame::operator =(const TimeFrame& time_frame) {
  if (time_frame.data_ != NULL) {
    time_frame.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }

  data_ = time_frame.data_;
  return (*this);
}


TickTimePos::TickTimePos(const TickTimePos& tit_pos)
    : data_(tit_pos.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

TickTimePos::TickTimePos() {
  data_ = new Data();
}

TickTimePos::TickTimePos(const int64 t_time, const int32 start_pos,
                         const int32 end_pos) {
  data_ = new Data(t_time, start_pos, end_pos);
}

TickTimePos& TickTimePos::operator =(const TickTimePos& tit_pos) {
  if (tit_pos.data_ != NULL) {
    tit_pos.data_->AddRef();
  }

  if (data_ != NULL) {
    data_->Release();
  }
  data_ = tit_pos.data_;
  return (*this);
}

StaticDataInfo::StaticDataInfo(const StaticDataInfo& static_data)
    : data_(static_data.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}

StaticDataInfo& StaticDataInfo::operator =(const StaticDataInfo& static_data) {
  if (static_data.data_ != NULL) {
    static_data.data_->AddRef();
  }
  if (data_ != NULL) {
    data_->Release();
  }

  data_ = static_data.data_;
  return (*this);
}

StaticDataInfo::StaticDataInfo(const std::string& out_data) {
  data_ = new Data();
  bool r = /*data_->symbol_static_.ParseFromString(out_data)*/false;
  if (r)
    data_->alive_time_ = time(NULL) + ALIVE_TIME;  //解析成功方可赋于生命周期
}

}


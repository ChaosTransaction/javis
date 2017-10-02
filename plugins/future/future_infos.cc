//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#include "future_infos.h"

namespace future_infos {

TickTimePos::TickTimePos(const TickTimePos& tit_pos)
    : data_(tit_pos.data_) {
  if (data_ != NULL) {
    data_->AddRef();
  }
}
TickTimePos::TickTimePos(const int32 start_pos, const int32 end_pos) {
  data_ = new Data(start_pos, end_pos);
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
  bool r = data_->symbol_static_.ParseFromString(out_data);
  if (r)
    data_->alive_time_ = time(NULL) + ALIVE_TIME;  //解析成功方可赋于生命周期
}

}


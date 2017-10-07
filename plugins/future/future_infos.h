//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef FUTURE_FUTURE_INFOS_H_
#define FUTURE_FUTURE_INFOS_H_

#include "proto/symbol_static.pb.h"
#include "logic/time.h"
#include "basic/basictypes.h"
#include <string>

#define ALIVE_TIME 3600

enum HIS_DATA_TYPE {
  _STATIC_DATA = 0,     //  静态数据
  _DYNA_DATA,           //  动态行情数据
  _L2_MMPEX,            //  level2扩展买卖盘
  _L2_REPORT,           //  level2逐笔成交
  _L2_ORDER_STAT,         //  委托队列
  _IOPV,              //  动态净值估值
  _MATU_YLD,            //  债券到期收益率
  HIS_DATA_TYPE_COUNT,      //  历史数据类型总数
  HIS_DATA_TYPE_UNKNOWN     //  未知历史数据类型
};

extern const char* s_stk_type[];
namespace future_infos {

class TimeUnit {
 public:
  TimeUnit(const TimeUnit& time_unit);
  TimeUnit(const std::string& start_time);

  TimeUnit& operator =(const TimeUnit& time_unit);

  ~TimeUnit() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  int32 full_day() const {
    return 10000 * data_->time_explod_.year + 100 * data_->time_explod_.month
        + data_->time_explod_.day_of_month;
  }

  base::Time::Exploded& exploded() const {
    return data_->time_explod_;
  }

  class Data {
   public:
    Data()
        : refcount_(1) {
      base::Time empty_time;
      empty_time.LocalExplode(&time_explod_);
    }

    Data(const std::string& str_time)
        : refcount_(1) {
      //2015-07-10 10:10:10
      const char* format = "%d-%d-%d %d:%d:%d";
      base::Time s_time = base::Time::FromStringFormat(str_time.c_str(),
                                                       format);
      s_time.LocalExplode(&time_explod_);
    }

   public:
    base::Time::Exploded time_explod_;
    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }

    void Release() {
      __sync_fetch_and_sub(&refcount_, 1);
      if (!refcount_)
        delete this;
    }
   private:
    int refcount_;
  };
  Data* data_;
};

class TimeFrame {
 public:
  TimeFrame(const TimeFrame& time_frame);
  TimeFrame(const std::string& start_time, const std::string& end_time);

  TimeFrame& operator =(const TimeFrame& time_frame);

  ~TimeFrame() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  int32 start_full_day() const {
    return data_->start_time_->full_day();
  }

  int32 end_full_day() const {
    return data_->end_time_->full_day();
  }

  base::Time::Exploded& start_exploded() const {
    return data_->start_time_->exploded();
  }

  base::Time::Exploded& end_exploded() const {
    return data_->end_time_->exploded();
  }

  TimeUnit* start_time() const {
    return data_->start_time_;
  }

  TimeUnit* end_time() const {
    return data_->end_time_;
  }

  class Data {
   public:
    Data(const std::string& str_start_time, const std::string& str_end_time)
        : refcount_(1) {
      start_time_ = new TimeUnit(str_start_time);
      end_time_ = new TimeUnit(str_end_time);
    }
    ~Data(){
      if(start_time_){delete start_time_; start_time_ = NULL;}
      if(end_time_){delete end_time_; end_time_ = NULL;}
    }

   public:
    TimeUnit* start_time_;
    TimeUnit* end_time_;
    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }

    void Release() {
      __sync_fetch_and_sub(&refcount_, 1);
      if (!refcount_)
        delete this;
    }
   private:
    int refcount_;
  };
  Data* data_;
};



class TickTimePos {
 public:
  TickTimePos();
  TickTimePos(const TickTimePos& tit_pos);
  TickTimePos(const int64 t_time, const int32 start_pos, const int32 end_pos);

  TickTimePos& operator =(const TickTimePos& tit_pos);

  ~TickTimePos() {
    if (data_ != NULL) {
      data_->Release();
    }
  }
  class Data {
   public:
    Data()
        : tt_time_(0),
          start_pos_(0),
          end_pos_(0),
          refcount_(1) {
    }

    Data(const int64 t_time, const int64 start_pos, const int64 end_pos)
        : tt_time_(t_time),
          start_pos_(start_pos),
          end_pos_(end_pos),
          refcount_(1) {
    }

   public:
    const int64 tt_time_;
    const int32 start_pos_;
    const int32 end_pos_;
    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }

    void Release() {
      __sync_fetch_and_sub(&refcount_, 1);
      if (!refcount_)
        delete this;
    }
   private:
    int refcount_;
  };
  Data* data_;
};
//因出现一分钟有多条数据，保留开始位置和起始位置

class StaticDataInfo {
 public:
  StaticDataInfo(const StaticDataInfo& static_data);
  StaticDataInfo(const std::string& out_data);

  StaticDataInfo& operator =(const StaticDataInfo& static_data);

  ~StaticDataInfo() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  class Data {
   public:
    Data()
        : alive_time_(0),
          refcount_(1) {
    }

   public:
    const chaos_data::SymbolStatic symbol_static_;
    int64 alive_time_;
    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }

    void Release() {
      __sync_fetch_and_sub(&refcount_, 1);
      if (!refcount_)
        delete this;
    }
   private:
    int refcount_;
  };
  Data* data_;
};

}

#endif /* PLUGINS_FUTURE_FUTURE_INFOS_H_ */

//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef FUTURE_FUTURE_INFOS_H_
#define FUTURE_FUTURE_INFOS_H_

#include "proto/symbol_static.pb.h"
#include "logic/time.h"
#include "basic/basictypes.h"
#include <string>

#define ALIVE_TIME 3600
namespace future_infos {

class TimeFrame {
 public:
  TimeFrame(const TimeFrame& time_frame);
  TimeFrame(const std::string& start_time,
            const std::string& end_time);

  TimeFrame& operator =(const TimeFrame& time_frame);

  ~TimeFrame(){
    if (data_ != NULL){
      data_->Release();
    }
  }


  const int32 start_full_day(){
    return 10000 * start_time_.year +
         100 * start_time_.month + start_time.day_of_month;
  }

  const int32 end_full_day(){
    return 10000 * end_time_.year +
         100 * end_time_.month + end_time.day_of_month;
  }

  const int32 start_hour() {return start_time_.hour;}
  const int32 end_hour() {return end_time_.hour;}

  const int32 start_minute() {return start_time_.minute;}
  const int32 end_minute() {return end_time_.minute;}

  const int32 start_second() {return start_time_.second;}
  const int32 end_second() {return end_time_.second;}


  class Data {
   public:
    Data()
        : refcount_(1){
    }

    Data(const std::string& str_start_time, const std::string& str_end_time)
        : refcount_(1){
      //2015-07-10 10:10:10
      const char* format = "%d-%d-%d %d:%d:%d";
      base::Time s_time = base::Time::FromStringFormat(str_start_time.c_str(),format);
      base::Time e_time = base::Time::FromStringFormat(str_end_time.c_str(),format);
      s_time.LocalExplode(&s_time);
      e_time.LocalExplode(&e_time);
    }

   public:
    const base::Time::Exploded start_time_;
    const base::Time::Exploded end_time_;
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
  TickTimePos(const TickTimePos& tit_pos);
  TickTimePos(const int32 start_pos, const int32 end_pos);

  TickTimePos& operator =(const TickTimePos& tit_pos);

  ~TickTimePos() {
    if (data_ != NULL) {
      data_->Release();
    }
  }
  class Data {
   public:
    Data()
        : refcount_(1),
          start_pos_(0),
          end_pos_(0) {
    }

    Data(const int64 start_pos, const int64 end_pos)
        : refcount_(1),
          start_pos_(1),
          end_pos_(1) {
    }

   public:
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
        : refcount_(1),
          alive_time_(0) {
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

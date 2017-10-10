//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef FUTURE_FUTURE_INFOS_H_
#define FUTURE_FUTURE_INFOS_H_

#include "proto/symbol_static.pb.h"
#include "proto/symbol_dynam_market.pb.h"
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

enum STK_TYPE {
  INDEX = 0,          //指数
  STOCK = 1,          //股票
  FUND = 2,           //基金
  BOND = 3,           //债券
  OTHER_STOCK = 4,    //其它股票
  OPTION = 5,         //选择权
  EXCHANGE = 6,       //外汇
  FUTURE = 7,         //期货
  FTR_IDX = 8,        //期指
  RGZ = 9,            //认购证
  ETF = 10,           //ETF
  LOF = 11,           //LOF
  COV_BOND = 12,      //可转债
  TRUST = 13,         //信托
  WARRANT = 14,       //权证
  REPO = 15,          //回购
  COMM = 16,          //商品现货
  STK_TYPE_COUNT
};

extern const char* s_stk_type[HIS_DATA_TYPE_COUNT];
extern const char* g_his_data_suffix[HIS_DATA_TYPE_COUNT];
extern const char* g_stk_type[STK_TYPE_COUNT];
namespace future_infos {


class DayMarket {
public:
  DayMarket();
  DayMarket(const DayMarket& dym);
  DayMarket(const int32 market_date, const std::string& str);

  DayMarket& operator = (const DayMarket& dym);


  int32 market_date() const {return data_->market_date_;}
  std::string& market_data() const {return data_->market_data_;}
  ~DayMarket() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  class Data {
   public:
    Data(const int32 market_date, const std::string& str)
        : market_date_(market_date)
        , market_data_(str)
        , refcount_(1){
    }

    Data():market_date_(0)
      ,refcount_(1){}
   public:
    const int32 market_date_;
    const std::string market_data_;

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

class StaticInfo {
 public:
  StaticInfo();
  StaticInfo(const StaticInfo& static_info);
  StaticInfo(const std::string& str);

  StaticInfo& operator =(const StaticInfo& static_info);

  ~StaticInfo() {
    if (data_ != NULL) {
      data_->Release();
    }
  }


  chaos_data::SymbolStatic& static_info() const {
    return data_->static_info;
  }
  class Data {
   public:
    Data()
        : refcount_(1) {
    }

    Data(const std::string& str)
        : refcount_(1) {
      static_info.ParseFromString(str);
    }

   public:
    chaos_data::SymbolStatic static_info;
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

class TimeUnit {
 public:
  TimeUnit(const TimeUnit& time_unit);
  TimeUnit(const std::string& str_time);
  TimeUnit(const int64 unix_time);

  TimeUnit& operator =(const TimeUnit& time_unit);

  ~TimeUnit() {
    if (data_ != NULL) {
      data_->Release();
    }
  }

  void reset_time(const int64 unix_time) {
    data_->reset_time(unix_time);
  }

  int32 full_day() const {
    return 10000 * data_->time_explod_.year + 100 * data_->time_explod_.month
        + data_->time_explod_.day_of_month;
  }

  base::Time::Exploded& exploded() const {
    return data_->time_explod_;
  }

  int32 ToUnixTime() const {
    return data_->base_time_.ToTimeT();
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
      base_time_ = base::Time::FromStringFormat(str_time.c_str(), format);
      base_time_.LocalExplode(&time_explod_);
    }

    Data(const int64 unix_time)
        : refcount_(1) {
      base_time_ = base::Time::FromTimeT(unix_time);
      base_time_.LocalExplode(&time_explod_);
    }

    void reset_time(const int64 unix_time){
      base_time_ = base::Time::FromTimeT(unix_time);
      base_time_.LocalExplode(&time_explod_);
    }
   public:
    base::Time::Exploded time_explod_;
    base::Time base_time_;
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

  TimeUnit* mutable_start_time() {
    return data_->start_time_;
  }

  TimeUnit* mutable_end_time() {
    return data_->end_time_;
  }

  void set_start_time(TimeUnit* start_time_unit) {
    data_->start_time_ = start_time_unit;
  }

  void set_end_time(TimeUnit* end_time_unit) {
    data_->end_time_ = end_time_unit;
  }

  class Data {
   public:
    Data(const std::string& str_start_time, const std::string& str_end_time)
        : refcount_(1) {
      start_time_ = new TimeUnit(str_start_time);
      end_time_ = new TimeUnit(str_end_time);
    }
    ~Data() {
      if (start_time_) {
        delete start_time_;
        start_time_ = NULL;
      }
      if (end_time_) {
        delete end_time_;
        end_time_ = NULL;
      }
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

  int64 time_index() const {return data_->tt_time_;}
  int32 start_pos() const {return data_->start_pos_;}
  int32 end_pos() const {return data_->end_pos_;}

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

//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef FUTURE_FUTURE_VALUE_BUF_H_
#define FUTURE_FUTURE_VALUE_BUF_H_

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include <list>
#include <string>
#include "logic/logic_comm.h"

namespace future_logic {
namespace net_request {
class DynaTick {
 public:
  DynaTick()
      : access_token_(NULL),
        uid_(NULL),
        sec_id_(NULL),
        field_(NULL),
        start_time_(NULL),
        end_time_(NULL) {

  }

  ~DynaTick() {
    if (access_token_) {
      delete access_token_;
      access_token_ = NULL;
    }
    if (uid_) {
      delete uid_;
      uid_ = NULL;
    }
    if (sec_id_) {
      delete sec_id_;
      sec_id_ = NULL;
    }
    if (field_) {
      delete field_;
      field_ = NULL;
    }
    if (start_time_) {
      delete start_time_;
      start_time_ = NULL;
    }
    if (end_time_) {
      delete end_time_;
      end_time_ = NULL;
    }
  }

  bool set_http_packet(base_logic::DictionaryValue* value);

  void set_access_token(const std::string& access_token) {
    access_token_ = new base_logic::StringValue(access_token);
  }

  void set_uid(const int64 uid) {
    uid_ = new base_logic::StringValue(uid);
  }

  void set_sec_id(const std::string& sec_id) {
    sec_id_ = new base_logic::StringValue(sec_id);
  }

  void set_field(const std::string& field) {
    field_ = new base_logic::StringValue(field);
  }

  void set_start_time(const std::string& start_time) {
    start_time_ = new base_logic::StringValue(start_time);
  }

  void set_end_time(const std::string& end_time) {
    end_time_ = new base_logic::StringValue(end_time);
  }

  std::string access_token() const {
    std::string access_token;
    access_token_->GetAsString(&access_token);
    return access_token;
  }

  const int64 uid() const {
    int64 uid = 0;
    uid_->GetAsBigInteger(&uid);
    return uid;
  }

  std::string sec_id() const {
    std::string sec_id;
    sec_id_->GetAsString(&sec_id);
    return sec_id;
  }

  std::string field() const {
    std::string field;
    field_->GetAsString(&field);
    return field;
  }

  std::string start_time() const {
    std::string start_time;
    start_time_->GetAsString(&start_time);
    return start_time;
  }

  std::string end_time() const {
    std::string end_time;
    end_time_->GetAsString(&end_time);
    return end_time;
  }

 public:
  base_logic::StringValue* access_token_;
  base_logic::FundamentalValue* uid_;
  base_logic::StringValue* sec_id_;
  base_logic::StringValue* field_;
  base_logic::StringValue* start_time_;
  base_logic::StringValue* end_time_;
};

}

namespace net_reply{
class DynaTick {
 public:
  DynaTick()
      : dyna_tick_(NULL),
        value_(NULL) {
    dyna_tick_ = new base_logic::ListValue;
  }

  ~DynaTick() {
    if (value_) {
      delete value_;
      value_ = NULL;
    }
  }

  void set_unit(base_logic::DictionaryValue* value) {
    dyna_tick_->Append(value);
  }

  base_logic::DictionaryValue* get() {
    value_ = new base_logic::DictionaryValue();
    if (!dyna_tick_->empty()) {
      value_->Set(L"dynatick", dyna_tick_);
    } else {
      delete dyna_tick_;
      dyna_tick_ = NULL;
    }
    return value_;
  }

  void Reset() {
    if (value_) {
      delete value_;
      value_ = NULL;
    }
    dyna_tick_ = new base_logic::ListValue;
  }

  const int32 Size() {
    return dyna_tick_->GetSize();
  }

 private:
  base_logic::ListValue* dyna_tick_;
  base_logic::DictionaryValue* value_;
};

class DynaTickUnit {
 public:
  DynaTickUnit()
      : current_time_(NULL),
        open_price_(NULL),
        high_price_(NULL),
        low_price_(NULL),
        new_price_(NULL),
        volume_(NULL),
        amount_(NULL),
        inner_vol_(NULL),
        tick_count_(NULL),
        buy_price_one_(NULL),
        buy_price_two_(NULL),
        buy_price_three_(NULL),
        buy_price_four_(NULL),
        buy_price_five_(NULL),
        sell_price_one_(NULL),
        sell_price_two_(NULL),
        sell_price_three_(NULL),
        sell_price_four_(NULL),
        sell_price_five_(NULL),
        buy_vol_one_(NULL),
        buy_vol_two_(NULL),
        buy_vol_three_(NULL),
        buy_vol_four_(NULL),
        buy_vol_five_(NULL),
        sell_vol_one_(NULL),
        sell_vol_two_(NULL),
        sell_vol_three_(NULL),
        sell_vol_four_(NULL),
        sell_vol_five_(NULL),
        open_interest_(NULL),
        settle_price_(NULL),
        value_(NULL) {
  }

  void set_current_time(const int64 current_time) {
    current_time_ = new base_logic::FundamentalValue(current_time);
  }

  void set_open_price(const double open_price) {
    open_price_ = new base_logic::FundamentalValue(open_price);
  }

  void set_high_price(const double high_price) {
    high_price_ = new base_logic::FundamentalValue(high_price);
  }

  void set_low_price(const double low_price) {
    low_price_ = new base_logic::FundamentalValue(low_price);
  }

  void set_new_price(const double new_price) {
    new_price_ = new base_logic::FundamentalValue(new_price);
  }

  void set_volume(const int64 volume) {
    volume_ = new base_logic::FundamentalValue(volume);
  }

  void set_amount(const double amount) {
    amount_ = new base_logic::FundamentalValue(amount);
  }

  void set_inner_vol(const int64 inner_vol) {
    inner_vol_ = new base_logic::FundamentalValue(inner_vol);
  }

  void set_tick_count(const int64 tick_count) {
    tick_count_ = new base_logic::FundamentalValue(tick_count);
  }

  void set_buy_price_one(const double buy_price_one) {
    buy_price_one_ = new base_logic::FundamentalValue(buy_price_one);
  }

  void set_buy_price_two(const double buy_price_two) {
    buy_price_two_ = new base_logic::FundamentalValue(buy_price_two);
  }

  void set_buy_price_three(const double buy_price_three) {
    buy_price_three_ = new base_logic::FundamentalValue(buy_price_three);
  }

  void set_buy_price_four(const double buy_price_four) {
    buy_price_four_ = new base_logic::FundamentalValue(buy_price_four);
  }

  void set_buy_price_five(const double buy_price_five) {
    buy_price_five_ = new base_logic::FundamentalValue(buy_price_five);
  }

  void set_sell_price_one(const double sell_price_one) {
    sell_price_one_ = new base_logic::FundamentalValue(sell_price_one);
  }

  void set_sell_price_two(const double sell_price_two) {
    sell_price_two_ = new base_logic::FundamentalValue(sell_price_two);
  }

  void set_sell_price_three(const double sell_price_three) {
    sell_price_three_ = new base_logic::FundamentalValue(sell_price_three);
  }

  void set_sell_price_four(const double sell_price_four) {
    sell_price_four_ = new base_logic::FundamentalValue(sell_price_four);
  }

  void set_sell_price_five(const double sell_price_five) {
    sell_price_five_ = new base_logic::FundamentalValue(sell_price_five);
  }

  void set_buy_vol_one(const int64 buy_vol_one) {
    buy_vol_one_ = new base_logic::FundamentalValue(buy_vol_one);
  }

  void set_buy_vol_two(const int64 buy_vol_two) {
    buy_vol_two_ = new base_logic::FundamentalValue(buy_vol_two);
  }

  void set_buy_vol_three(const int64 buy_vol_three) {
    buy_vol_three_ = new base_logic::FundamentalValue(buy_vol_three);
  }

  void set_buy_vol_four(const int64 buy_vol_four) {
    buy_vol_four_ = new base_logic::FundamentalValue(buy_vol_four);
  }

  void set_buy_vol_five(const int64 buy_vol_five) {
    buy_vol_five_ = new base_logic::FundamentalValue(buy_vol_five);
  }

  void set_open_interset(const int64 open_interset) {
    open_interest_ = new base_logic::FundamentalValue(open_interset);
  }

  base_logic::DictionaryValue* get() {
    value_ = new base_logic::DictionaryValue();
    if (current_time_ != NULL)
      value_->Set(L"currentTime", current_time_);
    if (open_price_ != NULL)
      value_->Set(L"openPrice", open_price_);
    if (high_price_ != NULL)
      value_->Set(L"highPrice", high_price_);
    if (low_price_ != NULL)
      value_->Set(L"lowPrice", low_price_);
    if (new_price_ != NULL)
      value_->Set(L"newPrice", new_price_);
    if (volume_ != NULL)
      value_->Set(L"volume", volume_);
    if (amount_ != NULL)
      value_->Set(L"amount", amount_);
    if (inner_vol_ != NULL)
      value_->Set(L"innerVol", inner_vol_);
    if (tick_count_ != NULL)
      value_->Set(L"tickCount", tick_count_);

    if (buy_price_one_ != NULL)
      value_->Set(L"buyPriceOne", buy_price_one_);
    if (buy_price_two_ != NULL)
      value_->Set(L"buyPriceTwo", buy_price_two_);
    if (buy_price_three_ != NULL)
      value_->Set(L"buyPriceThree", buy_price_three_);
    if (buy_price_four_ != NULL)
      value_->Set(L"buyPriceFour", buy_price_four_);
    if (buy_price_five_ != NULL)
      value_->Set(L"buyPriceFive", buy_price_five_);

    if (sell_price_one_ != NULL)
      value_->Set(L"sellPriceOne", sell_price_one_);
    if (sell_price_two_ != NULL)
      value_->Set(L"sellPriceTwo", sell_price_two_);
    if (sell_price_three_ != NULL)
      value_->Set(L"sellPriceThree", sell_price_three_);
    if (sell_price_four_ != NULL)
      value_->Set(L"sellPriceFour", sell_price_four_);
    if (sell_price_five_ != NULL)
      value_->Set(L"sellPriceFive", sell_price_five_);

    if (buy_vol_one_ != NULL)
      value_->Set(L"buyVolOne", buy_vol_one_);
    if (buy_vol_two_ != NULL)
      value_->Set(L"buyVolTwo", buy_vol_two_);
    if (buy_vol_three_ != NULL)
      value_->Set(L"buyVolThree", buy_vol_three_);
    if (buy_vol_four_ != NULL)
      value_->Set(L"buyVolFour", buy_vol_four_);
    if (buy_vol_five_ != NULL)
      value_->Set(L"buyVolFive", buy_vol_five_);

    if (sell_vol_one_ != NULL)
      value_->Set(L"sellVolOne", sell_vol_one_);
    if (sell_vol_two_ != NULL)
      value_->Set(L"sellVolTwo", sell_vol_two_);
    if (sell_vol_three_ != NULL)
      value_->Set(L"sellVolThree", sell_vol_three_);
    if (sell_vol_four_ != NULL)
      value_->Set(L"sellVolFour", sell_vol_four_);
    if (sell_vol_five_ != NULL)
      value_->Set(L"sellVolFive", sell_vol_five_);

    if (open_interest_ != NULL)
      value_->Set(L"openInterest", open_interest_);
    if (settle_price_ != NULL)
      value_->Set(L"settlePrice", settle_price_);
    return value_;
  }

 public:
  base_logic::FundamentalValue* current_time_;
  base_logic::FundamentalValue* open_price_;
  base_logic::FundamentalValue* high_price_;
  base_logic::FundamentalValue* low_price_;
  base_logic::FundamentalValue* new_price_;
  base_logic::FundamentalValue* volume_;
  base_logic::FundamentalValue* amount_;
  base_logic::FundamentalValue* inner_vol_;
  base_logic::FundamentalValue* tick_count_;
  base_logic::FundamentalValue* buy_price_one_;
  base_logic::FundamentalValue* buy_price_two_;
  base_logic::FundamentalValue* buy_price_three_;
  base_logic::FundamentalValue* buy_price_four_;
  base_logic::FundamentalValue* buy_price_five_;
  base_logic::FundamentalValue* sell_price_one_;
  base_logic::FundamentalValue* sell_price_two_;
  base_logic::FundamentalValue* sell_price_three_;
  base_logic::FundamentalValue* sell_price_four_;
  base_logic::FundamentalValue* sell_price_five_;
  base_logic::FundamentalValue* buy_vol_one_;
  base_logic::FundamentalValue* buy_vol_two_;
  base_logic::FundamentalValue* buy_vol_three_;
  base_logic::FundamentalValue* buy_vol_four_;
  base_logic::FundamentalValue* buy_vol_five_;
  base_logic::FundamentalValue* sell_vol_one_;
  base_logic::FundamentalValue* sell_vol_two_;
  base_logic::FundamentalValue* sell_vol_three_;
  base_logic::FundamentalValue* sell_vol_four_;
  base_logic::FundamentalValue* sell_vol_five_;
  base_logic::FundamentalValue* open_interest_;
  base_logic::FundamentalValue* settle_price_;

  base_logic::DictionaryValue* value_;

};
}

}

#endif /* PLUGINS_FUTURE_FUTURE_VALUE_BUF_H_ */

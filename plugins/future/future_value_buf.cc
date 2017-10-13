//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "basic/basic_util.h"
#include "future_infos.h"
#include "future_value_buf.h"

namespace future_logic {

const char* net_error(NET_ERRNO nerrno) {
  switch (nerrno) {
    case EXCEPTION:
      return "exception error";
    case FORMAT_ERRNO:
      return "format error";
    case PACKET_LEN_ERRNO:
      return "packet length error";
    case TOKEN_ERRNO:
      return "access token error";
    case UID_ERRNO:
      return "uid error";
    case SEC_ERRNO:
      return "sec error";
    case START_TIME_ERRNO:
      return "start time errno";
    case END_TIME_ERRNO:
      return "end time errno";
    case TIME_NO_DATA:
      return "time frame no data";
    default:
      return "unkown";
  }
}
namespace net_request {

NET_ERRNO BaseValue::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
  std::string access_token;
  int64 uid = 0;
  int32 net_code = 0;
  int64 big_net_code = 0;
  std::string field;
  std::string address;
  r = value->GetString(L"access_token", &access_token);
  if (r)
    set_access_token(access_token);
  else
    return TOKEN_ERRNO;

  r = value->GetBigInteger(L"uid", &uid);
  if (r)
    set_uid(uid);
  else
    return UID_ERRNO;

  r = value->GetString(L"field", &field);
  if (!r)
    field = "format";
  set_field(field);

  r = value->GetString(L"address", &address);
  if (r)
    set_address(address);

  r = value->GetBigInteger(L"net_code", &big_net_code);
  if (r)
    net_code = big_net_code;
  else
    net_code = TCP_TYPE * BASE_NET_TYPE;

  set_net_code(net_code);
  return NO_ERRNO;
}

NET_ERRNO DynaTick::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
  std::string sec_id;
  std::string start_time;
  std::string end_time;
  NET_ERRNO net_errno;
  if (value == NULL)
    return FORMAT_ERRNO;

  net_errno = BaseValue::set_http_packet(value);

  if (net_errno != 0)
    return net_errno;

  r = value->GetString(L"sec_id", &sec_id);
  if (r)
    set_sec_id(sec_id);
  else
    return SEC_ERRNO;

  r = value->GetString(L"start_time", &start_time);
  if (r)
    set_start_time(start_time);
  else
    return START_TIME_ERRNO;

  r = value->GetString(L"end_time", &end_time);
  if (r)
    set_end_time(end_time);
  else
    return END_TIME_ERRNO;

  return NO_ERRNO;
}
}

namespace net_reply {
void DynaTick::check_minute() {
  //一次性讀取不完 倒序
  int32 dyna_count = dyna_tick_->GetSize();
  int64 last_time = 0;
  int64 cur_time = 0;
  last_time = cur_time = GetTime(dyna_count - 1);

  for (int index = dyna_count - 2; dyna_tick_->GetSize() > 0; index--) {
    cur_time = GetTime(index);
    future_infos::TimeUnit cur_time_unit(cur_time);
    future_infos::TimeUnit last_time_unit(last_time);
    ULOG_DEBUG2("cur_time minute:%d,last_time minute:%d",
                cur_time_unit.exploded().minute,
                last_time_unit.exploded().minute);
    Remove(index + 1);
    if (cur_time_unit.exploded().minute != last_time_unit.exploded().minute)
      break;
    last_time = cur_time;
  }
  set_last_time(cur_time);
  set_next_time(last_time);
}

}
}


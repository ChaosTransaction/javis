//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "basic/basic_util.h"
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
}


//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include "basic/basic_util.h"
#include "future_value_buf.h"

namespace future_logic {
namespace net_request {
bool DynaTick::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
  std::string access_token;
  int64 uid = 0;
  std::string sec_id;
  std::string field;
  std::string start_time;
  std::string end_time;
  if (value == NULL)
    return false;

  r = value->GetString(L"access_token", &access_token);
  if (r)
    set_access_token(access_token);

  r = value->GetBigInteger(L"uid", &uid);
  if (r)
    set_uid(uid);

  r = value->GetString(L"sec_id", &sec_id);
  if (r)
    set_sec_id(sec_id);

  r = value->GetString(L"field", &field);
  if (r)
    set_field(field);

  r = value->GetString(L"start_time", &start_time);
  if (r)
    set_start_time(start_time);

  r = value->GetString(L"end_time", &end_time);
  if (r)
    set_end_time(end_time);

  return r;
}

}
}


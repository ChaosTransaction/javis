//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef PUB_LOGIC_LOGIC_UNIT_H_
#define PUB_LOGIC_LOGIC_UNIT_H_

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"

namespace logic {

class SendUtils {
 public:
  SendUtils();
  virtual ~SendUtils();
 private:
  static SendUtils* instance_;
 public:
  static SendUtils* GetInstance();
  static void FreeInstance();

 public:
  size_t SendFull(const int socket, const char* buffer, size_t bytes);

  bool SendBytes(const int socket, const void* bytes, int32 len, const char* file,
                 int32 line);

  bool SendValue(const int socket, base_logic::DictionaryValue* value);

  bool SendErrno(const int socket, int err_code, const char* err_str);

  /*bool SendMessage(int socket, struct PacketHead* packet, const char* file,
   int32 line);*/
 private:
  struct threadrw_t* socket_lock_;
};

}

#define send_value(socket, packet)\
    logic::SendUtils::GetInstance()->SendValue(socket, packet)\

#define send_full(socket, buffer, len) \
  logic::SendUtils::GetInstance()->SendFull(socket, buffer, len)\

#define send_error(socket, err_code, err_str)\
  logic::SendUtils::GetInstance()->SendErrno(socket, err_code,err_str)\

/*#define send_error(socket, type, error_code, session) \
  do { \
    struct PacketControl packet_control; \
    MAKE_HEAD(packet_control, ERROR_TYPE, type, 0, session, 0); \
    base_logic::DictionaryValue dic; \
    dic.SetInteger(L"result", error_code); \
    packet_control.body_ = &dic; \
    send_message(socket, &packet_control); \
  } while(0)*/

#define closelockconnect(socket) \
    shutdown(socket, SHUT_RDWR);

#endif /* PUB_LOGIC_LOGIC_UNIT_H_ */

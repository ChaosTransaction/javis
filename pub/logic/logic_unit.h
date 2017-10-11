//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef PUB_LOGIC_LOGIC_UNIT_H_
#define PUB_LOGIC_LOGIC_UNIT_H_


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
  int32 SendFull(int socket, const char* buffer, size_t bytes);

  bool SendBytes(int socket, const void* bytes, int32 len, const char* file,
                 int32 line);

  bool SendValue(int socket, base_logic::DictionaryValue* value);

  /*bool SendMessage(int socket, struct PacketHead* packet, const char* file,
                   int32 line);*/
 private:
  struct threadrw_t* socket_lock_;
};

}

//#define send_message(socket, packet) \
 // logic::SendUtils::GetInstance()->SendMessage(socket, packet, __FILE__, __LINE__)\

#define send_value(socket, packet)\
    logic::SendUtils::GetInstance()->SendValue(socket, packet)\

#define send_full(socket, buffer, len) \
  logic::SendUtils::GetInstance()->SendFull(socket, buffer, len)\

#define send_error(socket, type, error_code, session) \
  do { \
    struct PacketControl packet_control; \
    MAKE_HEAD(packet_control, ERROR_TYPE, type, 0, session, 0); \
    base_logic::DictionaryValue dic; \
    dic.SetInteger(L"result", error_code); \
    packet_control.body_ = &dic; \
    send_message(socket, &packet_control); \
  } while(0)


#define closelockconnect(socket) \
    shutdown(socket, SHUT_RDWR);


#endif /* PUB_LOGIC_LOGIC_UNIT_H_ */

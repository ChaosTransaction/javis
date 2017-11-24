//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#ifndef JAVIS_FUTURE_FUTURE_LOGIC_
#define JAVIS_FUTURE_FUTURE_LOGIC_

#include "basic/basictypes.h"
#include "core/common.h"
#include "net/packet_processing.h"
namespace future_logic {

class Futurelogic {
 public:
  Futurelogic();
  virtual ~Futurelogic();
 private:
  static Futurelogic *instance_;

 public:
  static Futurelogic *GetInstance();
  static void FreeInstance();

  bool OnFutureConnect(struct server *srv, const int socket);

  bool OnFutureMessage(struct server *srv, const int socket, const void *msg,
                       const int len);

  bool OnFutureClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, const int socket,
                          const void *data, const int len);

  bool OnBroadcastMessage(struct server *srv, const int socket, const void *msg,
                          const int len);

  bool OnBroadcastClose(struct server *srv, const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, char *id, int opcode, int time);

 private:
  bool OnDynaTick(struct server* srv, int socket, struct PacketHead* packet);

  bool OnDynaFile(struct server* srv, int socket, struct PacketHead* packet);
 private:
  bool Init();
 private:

};
}  // namespace future_logic

#endif

//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#include "future/future_init.h"
#include "core/common.h"
#include "core/plugins.h"
#include "future/future_logic.h"

struct futureplugin {
  char *id;
  char *name;
  char *version;
  char *provider;
};

static void *OnFutureStart(struct server* srv) {
  signal(SIGPIPE, SIG_IGN);
  struct futureplugin *future = (struct futureplugin *) calloc(
      1, sizeof(struct futureplugin));
  future->id = "future";
  future->name = "future";
  future->version = "1.0.0";
  future->provider = "kerry";
  if (!future_logic::Futurelogic::GetInstance())
    assert(0);
  return future;
}

static handler_t OnFutureShutdown(struct server *srv, void *pd) {
  future_logic::Futurelogic::FreeInstance();

  return HANDLER_GO_ON;
}

static handler_t OnFutureConnect(struct server *srv, int fd, void *data,
                                 int len) {
  future_logic::Futurelogic::GetInstance()->OnFutureConnect(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnFutureMessage(struct server *srv, int fd, void *data,
                                 int len) {
  bool r = future_logic::Futurelogic::GetInstance()->OnFutureMessage(srv, fd,
                                                                     data, len);
  if (r)
    return HANDLER_GO_ON;
  else
    return HANDLER_FINISHED;
}

static handler_t OnFutureClose(struct server *srv, int fd) {
  future_logic::Futurelogic::GetInstance()->OnFutureClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnUnknow(struct server *srv, int fd, void *data, int len) {
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastConnect(struct server *srv, int fd, void *data,
                                    int len) {
  future_logic::Futurelogic::GetInstance()->OnBroadcastConnect(srv, fd, data,
                                                               len);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastClose(struct server *srv, int fd) {
  future_logic::Futurelogic::GetInstance()->OnBroadcastClose(srv, fd);
  return HANDLER_GO_ON;
}

static handler_t OnBroadcastMessage(struct server *srv, int fd, void *data,
                                    int len) {
  //LOG_ERROR("===============================onbroadcastmessage");
  future_logic::Futurelogic::GetInstance()->OnBroadcastMessage(srv, fd, data,
                                                               len);
  return HANDLER_GO_ON;
}

static handler_t OnIniTimer(struct server *srv) {
  future_logic::Futurelogic::GetInstance()->OnIniTimer(srv);
  return HANDLER_GO_ON;
}

static handler_t OnTimeOut(struct server *srv, char *id, int opcode, int time) {
  future_logic::Futurelogic::GetInstance()->OnTimeout(srv, id, opcode, time);
  return HANDLER_GO_ON;
}

int future_plugin_init(struct plugin *pl) {
  pl->init = OnFutureStart;
  pl->clean_up = OnFutureShutdown;
  pl->connection = OnFutureConnect;
  pl->connection_close = OnFutureClose;
  pl->connection_close_srv = OnBroadcastClose;
  pl->connection_srv = OnBroadcastConnect;
  pl->handler_init_time = OnIniTimer;
  pl->handler_read = OnFutureMessage;
  pl->handler_read_srv = OnBroadcastMessage;
  pl->handler_read_other = OnUnknow;
  pl->time_msg = OnTimeOut;
  pl->data = NULL;
  return 0;
}

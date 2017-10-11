//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#include "future/future_logic.h"
#include "operator_code.h"
#include "config/config.h"
#include "future_engine.h"
#define DEFAULT_CONFIG_PATH "./plugins/future/future_config.xml"

namespace future_logic {

Futurelogic *Futurelogic::instance_ = NULL;

Futurelogic::Futurelogic() {
  if (!Init())
    assert(0);
}

Futurelogic::~Futurelogic() {
  FutureEngine::FreeFutureEngine();
  FutureEngine::FreeSchdulerManager();
}

bool Futurelogic::Init() {
  bool r = false;
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig *config = config::FileConfig::GetFileConfig();
  if (config == NULL)
    return false;
  r = config->LoadConfig(path);

  FutureEngine::GetSchdulerManager();
  FutureEngine::GetFutureEngine();

  /*int64 uid = 10001;
   std::string token = "e12323123daqwe";
   std::string field = "json";
   int socket = 0;
   std::string sec_symbol = "WT001.CZCE";
   HIS_DATA_TYPE data_type = _DYNA_DATA;
   std::string start_time = "2009-12-14 10:40:22";
   std::string end_time = "2009-12-17 14:52:21";


   FutureEngine::GetSchdulerManager()->OnDynaTick(socket,uid,
   token, field, sec_symbol, FUTURE,
   start_time,end_time);*/

  /*size_t start_pos = sec_symbol.find(".");
   std::string symbol = sec_symbol.substr(0, start_pos);
   std::string sec = sec_symbol.substr(start_pos + 1,
   sec_symbol.length() - symbol.length() - 1);

   future_infos::TickTimePos start_time_pos;
   future_infos::TickTimePos end_time_pos;
   IndexEngine::GetSchdulerManager()->OnFetchIndexPos(sec, symbol, data_type,
   start_time, end_time,
   start_time_pos,
   end_time_pos);
   std::list<future_infos::StaticInfo> static_list;
   StaticEngine::GetSchdulerManager()->OnFetchStaticInfo(
   sec, symbol, FUTURE, start_time_pos,end_time_pos,static_list
   );
   
   DataEngine::GetSchdulerManager()->Test(data_type, FUTURE,
   static_list);

   */

  /*std::string sec = "ZC";
   std::string data_type = "INDEXPOS";
   std::string shuffix = ".ipos";
   std::string symbol = "WT0000";
   int32 year = 2009;
   int32 month = 12;
   int32 day = 2;
   FutureEngine::GetSchdulerManager()->LoadLocalIndexPosInfo(sec, data_type,
   shuffix, symbol, year, month, day);*/

  return true;
}

Futurelogic *Futurelogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new Futurelogic();
  return instance_;
}

void Futurelogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool Futurelogic::OnFutureConnect(struct server *srv, const int socket) {
  return true;
}

bool Futurelogic::OnFutureMessage(struct server *srv, const int socket,
                                  const void *msg, const int len) {
  bool r = false;

  return true;
}

bool Futurelogic::OnFutureClose(struct server *srv, const int socket) {
  return true;
}

bool Futurelogic::OnBroadcastConnect(struct server *srv, const int socket,
                                     const void *msg, const int len) {
  return true;
}

bool Futurelogic::OnBroadcastMessage(struct server *srv, const int socket,
                                     const void *msg, const int len) {
  bool r = false;
  struct PacketHead *packet = NULL;
  if (srv == NULL || socket < 0 || msg == NULL || len < PACKET_HEAD_LENGTH)
    return false;

  if (!net::PacketProsess::UnpackStream(msg, len, &packet)) {
    LOG_ERROR2("UnpackStream Error socket %d", socket);
    return false;
  }

  switch (packet->operate_code) {
    case R_FUTURE_DYNA_TICK: {
      OnDynaTick(srv, socket, packet);
      break;
    }
    default:
      break;
  }

  if (packet) {
    delete packet;
    packet = NULL;
  }
  return true;
}

bool Futurelogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool Futurelogic::OnIniTimer(struct server *srv) {
  if (srv->add_time_task != NULL) {
  }
  return true;
}

bool Futurelogic::OnTimeout(struct server *srv, char *id, int opcode,
                            int time) {
  switch (opcode) {
    default:
      break;
  }
  return true;
}

bool Futurelogic::OnDynaTick(struct server* srv, int socket,
                             struct PacketHead* packet) {
  future_logic::net_request::DynaTick dyna_tick;

  if (packet->packet_length <= PACKET_HEAD_LENGTH) {
    //send_error(socket, ERROR_TYPE, ERROR_TYPE, FORMAT_ERRNO);
    return false;
  }
  struct PacketControl* packet_control = (struct PacketControl*) (packet);
  bool r = cancel_order.set_http_packet(packet_control->body_);
  if (!r) {
    //send_error(socket, ERROR_TYPE, ERROR_TYPE, FORMAT_ERRNO);
    return false;
  }

  FutureEngine::GetSchdulerManager()->OnDynaTick(socket, dyna_tick.uid(),
                                                 dyna_tick.access_token(),
                                                 dyna_tick.field(),
                                                 dyna_tick.sec_id(), FUTURE,
                                                 dyna_tick.start_time(),
                                                 dyna_tick.end_time());
  return true;
}

}  // namespace future_logic

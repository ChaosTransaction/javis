//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry
#include <string>
#include <sys/socket.h>
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
namespace logic {
SendUtils *SendUtils::instance_ = NULL;

SendUtils::SendUtils() {
  InitThreadrw(&socket_lock_);
}
SendUtils::~SendUtils() {
  DeinitThreadrw(socket_lock_);
}

SendUtils *SendUtils::GetInstance() {
  if (instance_ == NULL)
    instance_ = new SendUtils();
  return instance_;
}

void SendUtils::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

size_t SendUtils::SendFull(const int socket, const char *buffer, size_t nbytes) {
  base_logic::WLockGd lk(socket_lock_);
  ssize_t amt = 0;
  ssize_t total = 0;
  const char *buf = buffer;
  do {
    amt = nbytes;
    amt = send(socket, buf, amt, 0);
    if (-1 == amt) {
      if (11 == errno)
        continue;
      else {
        break;
      }
    }
    buf = buf + amt;
    nbytes -= amt;
    total += amt;
  } while (nbytes > 0);
  return (amt == -1 ? amt : total);
}

bool SendUtils::SendBytes(const int socket, const void* bytes, int32 len,
                          const char* file, int32 line) {
  if (socket <= 0 || bytes == NULL || len <= 0)
    return false;

  int32 ret = SendFull(socket, reinterpret_cast<const char*>(bytes), len);
  if (ret != len) {
    LOG_ERROR("Send bytes failed");
    return false;
  }
  return true;
}

bool SendUtils::SendErrno(const int socket, int err_code,
               const char* err_str) {
  base_logic::DictionaryValue err_value;
  err_value.SetInteger(L"err_code",err_code);
  err_value.SetString(L"err_str",err_str);
  return SendValue(socket,&err_value);
}

bool SendUtils::SendValue(const int socket, base_logic::DictionaryValue* value) {
  base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON);
  std::string body_stream;
  bool r = engine->Serialize((*value), &body_stream);

  size_t ret = SendFull(socket, const_cast<char*>(body_stream.c_str()),
                     body_stream.length());


  if (engine) {
    delete engine;
    engine = NULL;
  }
  if(ret != body_stream.length())
    return false;
  return r;
}

/*
 bool SendUtils::SendMessage(int socket, struct PacketHead* packet,
 const char* file, int32 line) {
 bool r;
 void *packet_stream = NULL;
 int32_t packet_stream_length = 0;
 int ret = 0;
 bool r1 = false;
 if (socket <= 0 || packet == NULL)
 return false;

 if (net::PacketProsess::PacketStream(packet, &packet_stream,
 &packet_stream_length) == false) {
 LOG_ERROR2("Call PackStream failed in %s:%d", file, line);
 r = false;
 goto MEMFREE;
 }

 ret = SendFull(socket, reinterpret_cast<char*>(packet_stream),
 packet_stream_length);
 //net::PacketProsess::HexEncode(packet_stream, packet_stream_length);
 if (ret != packet_stream_length) {
 LOG_ERROR2("Sent msg failed in %s:%d", file, line);
 r = false;
 goto MEMFREE;
 } else {
 r = true;
 goto MEMFREE;
 }

 MEMFREE: char* stream = reinterpret_cast<char*>(packet_stream);
 if (stream) {
 //delete[] stream;
 free((void*) stream);
 stream = NULL;
 }
 return r;
 }*/

}


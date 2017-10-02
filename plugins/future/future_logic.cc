//  Copyright (c) 2017-2018 The Javis Authors. All rights reserved.
//  Created on: 2017年9月30日 Author: kerry

#include "future/future_logic.h"
#include "future/future_engine.h"
#include "config/config.h"


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

bool Futurelogic::OnTimeout(struct server *srv, char *id, int opcode, int time) {
    switch (opcode) {
    default:
        break;
    }
    return true;
}

}  // namespace future_logic

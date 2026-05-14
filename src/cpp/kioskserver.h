#ifndef KIOSKSERVER_H
#define KIOSKSERVER_H

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <mutex>
#include <algorithm>

// cpp-httplib
#include "cpp-httplib/httplib.h"

// nlohmann_json
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class KioskServer
{
public:
    KioskServer();
    ~KioskServer();

    bool loadConfig(const std::string &path);
    void start();

private:
    std::unique_ptr<httplib::Server> svr;
    json config;

    std::vector<httplib::ws::WebSocket*> ws_clients_;
    std::mutex ws_mutex_;

    bool isAuthorized(const httplib::Request &req);
    std::string loadFile(const std::string &path);
    void broadcastUpdate(const json &data);
};

#endif // KIOSKSERVER_H

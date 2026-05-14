#include "kioskserver.h"
#include <fstream>
#include <iostream>
#include <sstream>

KioskServer::KioskServer()
{
    svr = std::make_unique<httplib::Server>();
}

KioskServer::~KioskServer() {}

bool KioskServer::loadConfig(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        config = {
            {"identification", {{"name", "main_screen"}}},
            {"security",       {{"allowed_ips", {"*"}}, {"api_key", "123456"}}},
            {"server",         {{"host", "0.0.0.0"}, {"port", 8080}}}
        };
        std::ofstream out(path);
        if (out.is_open()) out << config.dump(4) << std::endl;
        return true;
    }
    try   { file >> config; return true; }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] config: " << e.what() << std::endl;
        return false;
    }
}

bool KioskServer::isAuthorized(const httplib::Request &req)
{
    auto key = req.get_header_value("Authorization");
    return !key.empty() && key == config["security"]["api_key"].get<std::string>();
}

std::string KioskServer::loadFile(const std::string &path)
{
    std::ifstream f(path);
    if (!f.is_open()) { std::cerr << "[WARNING] file not found: " << path << std::endl; return ""; }
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
}

void KioskServer::broadcastUpdate(const json &data)
{
    std::string msg = data.dump();
    std::lock_guard<std::mutex> lock(ws_mutex_);
    for (auto *ws : ws_clients_)
        if (ws && ws->is_open()) ws->send(msg);
    std::cout << "[WS] broadcast → " << ws_clients_.size() << " client(s)" << std::endl;
}

void KioskServer::start()
{
    if (!svr) { std::cerr << "[ERROR] server not initialized" << std::endl; return; }

    int         port = config["server"]["port"].get<int>();
    std::string host = config["server"]["host"].get<std::string>();

    // Inject bridge.mjs inline into HTML before </head>
    std::string bridge = loadFile("src/js/bridge.mjs");
    std::string html   = loadFile("web/index.html");

    const std::string inject = "<script>\n" + bridge + "\n</script>\n";
    const std::string needle = "</head>";
    auto pos = html.find(needle);
    if (pos != std::string::npos)
        html.insert(pos, inject);
    else
        std::cerr << "[WARNING] </head> not found — bridge not injected" << std::endl;

    // GET / → index.html (with bridge already inlined)
    svr->Get("/", [html](const httplib::Request &, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(html, "text/html; charset=utf-8");
    });

    // Static: web/** files accessible at /static/**
    svr->set_mount_point("/static", "web");

    // POST /event — generic relay
    // Required fields: event (string), message (object)
    // Security: Authorization header must match api_key
    svr->Post("/event", [this](const httplib::Request &req, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        if (!isAuthorized(req))
        {
            res.status = 401;
            res.set_content(json{{"status","error"},{"message","Unauthorized"}}.dump(), "application/json");
            return;
        }

        try
        {
            auto body = json::parse(req.body);

            if (!body.contains("event") || !body.contains("message"))
            {
                res.status = 400;
                res.set_content(json{{"status","error"},{"message","event and message fields are required"}}.dump(), "application/json");
                return;
            }

            std::string event = body["event"].get<std::string>();
            json        msg   = body["message"];

            std::cout << "[EVENT] " << event << " | " << msg.dump() << std::endl;

            broadcastUpdate({{"event", event}, {"message", msg}});

            res.set_content(json{{"status","ok"},{"event",event}}.dump(), "application/json");
        }
        catch (const std::exception &e)
        {
            res.status = 400;
            res.set_content(json{{"status","error"},{"message",e.what()}}.dump(), "application/json");
        }
    });

    // WebSocket /ws
    svr->WebSocket("/ws", [this](const httplib::Request &, httplib::ws::WebSocket &ws)
    {
        {
            std::lock_guard<std::mutex> lock(ws_mutex_);
            ws_clients_.push_back(&ws);
            std::cout << "[WS] connected. total: " << ws_clients_.size() << std::endl;
        }
        std::string msg;
        while (ws.is_open())
            if (ws.read(msg) == httplib::ws::ReadResult::Fail) break;
        {
            std::lock_guard<std::mutex> lock(ws_mutex_);
            ws_clients_.erase(std::remove(ws_clients_.begin(), ws_clients_.end(), &ws), ws_clients_.end());
            std::cout << "[WS] disconnected. total: " << ws_clients_.size() << std::endl;
        }
    });

    // CORS preflight
    svr->Options("/*", [](const httplib::Request &, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin",  "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    });

    std::cout << "[INFO] listening on " << host << ":" << port << std::endl;
    if (!svr->listen(host.c_str(), port))
        std::cerr << "[ERROR] failed to start server" << std::endl;
}

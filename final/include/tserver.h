#pragma once

#include <string>

class TServer {
private:
    std::string host;
    std::string port;
    std::string folder;

public:
    TServer(const std::string &h, const std::string &p, const std::string &f): host(h), port(p), folder(f) { };

    ~TServer() { };

    void start();
};

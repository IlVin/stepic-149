#include "cgi.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>

std::string tbad = "400 Bad Request";
std::string t200 = "HTTP/1.0 200 OK\r\nContent-length: %d\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n%s";
std::string t404 = "HTTP/1.0 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n";

void TCGI::bad_request() {
    wb->append(tbad);
}

void TCGI::start(TBuffer * rb_, TBuffer * wb_) {
    rb = rb_;
    wb = wb_;
    r = new TRequest(rb);
    if (r->status < 0) {
        bad_request();
    } else {
        std::string path = r->path;
        std::string fpath = folder + path;

        std::ifstream * f = new std::ifstream(fpath, std::ios::in );
        if (!f->is_open()) {
            wb->append(t404);
        } else {
            std::string content;
            getline(*f, content, '\0');
            f->close();
            std::stringstream ss;
            ss  << "HTTP/1.0 200 OK\r\nContent-length: " << content.size()
                << "\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n" << content;
            wb->append(ss.str());
        }
        delete f;

    }
    return;
}

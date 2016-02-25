#pragma once

#include <ev.h>
#include "buffer.h"
#include "cgi.h"
#include "request.h"

class TCGI {
    private:
        std::string folder;

        TBuffer * rb;
        TBuffer * wb;
        TRequest * r;

    public:
        TCGI(std::string f): folder(f) {
        }
        ~TCGI() {
        }

        void start(TBuffer * rb_, TBuffer * wb_);
        void bad_request();
};

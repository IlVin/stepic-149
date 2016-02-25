#pragma once

#include <iostream>
#include <string>
#include "buffer.h"


class TRequest {
    private:
        TBuffer * rb;
    public:
        int status;
        char * path;

        TRequest(TBuffer * rb_): rb(rb_) {
            path = nullptr;
            if( parse() ) {
                status = 0;
            } else {
                status = -1;
            }
        }
        ~TRequest() {
            delete path;
        }

    bool parse();
};

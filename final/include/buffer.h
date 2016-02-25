#pragma once

#include <string>
#include <assert.h>

#include <ev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#define INIT_RAW_SIZE 2048

class TBuffer {
    private:
        char * raw;
        char * hd;
        char * tl;
        size_t size;

    public:
        TBuffer() {
            size = INIT_RAW_SIZE;
            raw = (char *)malloc(size);
            tl = hd = raw;
        }

        virtual ~TBuffer() {
            free(raw);
        }

        char ** head();
        char ** tail();
        size_t length();
        bool space(size_t need_space);
        void append(const std::string &str);
        int rcv(int fd, size_t sz);
        int snd(int fd);
        std::string asString();
};

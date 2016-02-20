#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <ev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUF_SIZE 1024

class HTTPHandler;

struct io_context {
    struct ev_io io;
    HTTPHandler * h;
};

static struct ev_io * w(struct io_context * ctx) {
    return (struct ev_io *) ctx;
}

static struct io_context * ctx(struct ev_io * w) {
    return (struct io_context *) w;
}

class HTTPHandler {
    private:
        int      sock;

        char     r_buffer[BUF_SIZE];
        int      r_size;

        char     w_buffer[BUF_SIZE];
        int      w_size;

    public:
        bool     is_finished;

        HTTPHandler(int s): sock(s) {
            r_size = 0;
            w_size = 0;
        };

        virtual ~HTTPHandler() {
            shutdown(sock, SHUT_RDWR);
            close(sock);
        }
};

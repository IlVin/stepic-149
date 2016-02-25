#pragma once

#include <iostream>
#include <string>
#include <list>

#include <ev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "http_handler.h"

class TServer;

struct acc_context {
    struct ev_io io;
    TServer * srv;
};

struct sig_context {
    struct ev_signal sig;
    TServer * srv;
};

class TServer {
private:
    std::string host;
    std::string port;
    std::string folder;

    int                    sock;

    std::list<HTTPHandler *>   handlers;

    struct ev_loop        *loop;
    struct acc_context    *ctx_accept; // accept
    struct sig_context    *ctx_signal; // signal

public:
    TServer(const std::string &h, const std::string &p, const std::string &f): host(h), port(p), folder(f), loop(EV_DEFAULT) {
        ctx_accept = new acc_context;
        ctx_signal = new sig_context;
        ctx_accept->srv = this;
        ctx_signal->srv = this;
    };

    struct ev_io * w_accept() {
        return (struct ev_io *) ctx_accept;
    }

    struct ev_signal * w_signal() {
        return (struct ev_signal *) ctx_signal;
    }

    static struct acc_context * ctx(struct ev_io * w) {
        return (struct acc_context *) w;
    }

    static struct sig_context * ctx(struct ev_signal * w) {
        return (struct sig_context *) w;
    }

    void close_handlers();

    virtual ~TServer() {
        close_handlers();
        shutdown(sock, SHUT_RDWR);
        close(sock);
    }

    void start();

    void gc_handlers();

    static void sig_cb(struct ev_loop *loop, struct ev_signal *w, int revent) {
        ev_signal_stop(ctx(w)->srv->loop, w);
        ev_unloop(ctx(w)->srv->loop, EVUNLOOP_ALL);
    }

    static void io_accept_cb(struct ev_loop *loop, struct ev_io *w, int revent) {
        int client_sd = accept(w->fd, NULL, NULL);
        ctx(w)->srv->gc_handlers();
        ctx(w)->srv->handlers.push_back(new HTTPHandler(loop, client_sd, ctx(w)->srv->folder));
    }


};



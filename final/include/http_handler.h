#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <ev.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "buffer.h"
#include "cgi.h"

class HTTPHandler;

struct io_context {
    struct ev_io io;
    HTTPHandler * h;
};

static struct ev_io * w(struct io_context * ctx) {
    return (struct ev_io *) ctx;
}

static struct io_context * ioctx(struct ev_io * w) {
    return (struct io_context *) w;
}

class HTTPHandler {
    private:
        struct ev_loop     * loop;
        struct io_context  * rw_ctx; // accept

        std::string folder;

        int      sock;

        TBuffer  *rb;
        TBuffer  *wb;

    public:
        bool     is_finished;

        HTTPHandler(struct ev_loop * l, int s, std::string f): loop(l), folder(f), sock(s) {
            rw_ctx = new io_context;
            rw_ctx->h = this;

            rb = new TBuffer();
            wb = new TBuffer();

            ev_init(w(rw_ctx), r_cb);
            ev_io_set(w(rw_ctx), sock, EV_READ);
            ev_io_start(loop, w(rw_ctx));
        };

        virtual ~HTTPHandler() {
            shutdown(sock, SHUT_RDWR);
            close(sock);
            delete rw_ctx;
            delete rb;
            delete wb;
        }


        static void w_cb(struct ev_loop *loop, struct ev_io *w, int revent) {
            TBuffer *wb = ioctx(w)->h->wb;
            int r = wb->snd(w->fd);
            if (r == 0 || r == -1) {
                ev_io_stop(loop, w);
                ioctx(w)->h->is_finished = true;
                shutdown(w->fd, SHUT_RDWR);
                close(w->fd);
                free(w);
            }
            return;
        }

        static void r_cb(struct ev_loop *loop, struct ev_io *w, int revent) {
            ev_io_stop(loop, w);
            TBuffer *rb = ioctx(w)->h->rb;
            TBuffer *wb = ioctx(w)->h->wb;
            int r = rb->rcv(w->fd, 1024);

            if (r > 0){
                char ** p = rb->head();

                // No multithreading
                if (strstr(*p, "\n\n") != nullptr || strstr(*p, "\n\r\n") != nullptr) {
                    ev_io_stop(loop, w);
                    TCGI * cgi = new TCGI(ioctx(w)->h->folder);
                    cgi->start(rb, wb);
                    delete cgi;
                    //response
                    ev_init(w, w_cb);
                    ev_io_set(w, w->fd, EV_WRITE);
                } 
                ev_io_start(loop, w);
            } else {
                free(w);
            }
            return;
        }

};

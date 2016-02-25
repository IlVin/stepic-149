#include "tserver.h"

#include <string>
#include <list>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ev.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <resolv.h>
#include <errno.h>

void TServer::start() {

    sock = socket(PF_INET, SOCK_STREAM, 0);

    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    struct sockaddr_in addr;
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(std::stoi(port));
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0)
        perror("bind");

    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);

    listen(sock, SOMAXCONN);

    ev_init(w_accept(), io_accept_cb);
    ev_io_set(w_accept(), sock, EV_READ);
    ev_io_start(loop, w_accept());

    ev_signal_init(w_signal(), sig_cb, SIGINT);
    ev_signal_start(loop, w_signal());

    ev_loop(loop, 0);

    return;
};

void TServer::close_handlers() {
    for (std::list<HTTPHandler *>::iterator it = handlers.begin(); it != handlers.end(); it++) {
        delete *it;
    }
}

void TServer::gc_handlers() {
    for (std::list<HTTPHandler *>::iterator it = handlers.begin(); it != handlers.end(); it++) {
        if ((*it)->is_finished) {
            std::cout << "HTTP is finished !!!" << std::endl;
        }
    }
}



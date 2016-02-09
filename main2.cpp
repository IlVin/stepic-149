#include <iostream>
#include <string>

#include <limits.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main() {
    std::string in_path = "/home/box/in.fifo";
    std::string out_path = "/home/box/out.fifo";

    unlink(in_path.c_str());
    unlink(out_path.c_str());

    mkfifo(in_path.c_str(), 0666);
    mkfifo(out_path.c_str(), 0666);
    int fd_in = open(in_path.c_str(), O_RDONLY);
    int flags_in = fcntl(fd_in, F_GETFL, 0);
    fcntl(fd_in, F_SETFL, flags_in | O_NONBLOCK);

    int fd_out = open(out_path.c_str(), O_WRONLY);
    int flags_out = fcntl(fd_out, F_GETFL, 0);
    fcntl(fd_out, F_SETFL, flags_out | O_NONBLOCK);

    char buf[1024];
    int roffs = 0, woffs = 0, cnt;
    while (1) {

        cnt = read(fd_in, &buf[roffs], sizeof(buf) - roffs);
        std::cout << "R[" << roffs << ", " << cnt << "] {" << errno << "}" << std::endl;
        if (cnt < 0 && errno != EAGAIN) {
            std::cout << "R error[" << cnt << "] {" << errno << "}" << std::endl;
            return 1;
        }
        roffs += cnt;

        cnt = write(fd_out, &buf[woffs], roffs - woffs);
        std::cout << "W[" << woffs << ", " << cnt << "] {" << errno << "}" << std::endl;
        if (cnt < 0 && errno != EAGAIN) {
            std::cout << "W[" << cnt << "] {" << errno << "}" << std::endl;
            return 1;
        }
        woffs += cnt;


        if (woffs != roffs) {
            roffs = roffs - woffs;
            for (int i = 0; i < roffs; i++) {
                buf[i] = buf[woffs + i];
            }
        } else {
            woffs = roffs = 0;
        }

        sleep(1);
    }

    return 0;
}

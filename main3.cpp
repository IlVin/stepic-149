#include <iostream>
#include <string>

#include <limits.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

#define PARENT 0
#define CHILD 1

int main() {
    pid_t pid;

    int fd[2];
    socketpair(PF_LOCAL, SOCK_STREAM, 0, fd);

    pid = fork();
    if (pid == 0) {
        close(fd[PARENT]);
        const char hello[] = "hello parent, I am child";
        write(fd[CHILD], hello, sizeof(hello));
    } else {
        close(fd[CHILD]);
        char buf[1024];
        int n = read(fd[PARENT], buf, sizeof(buf));
        printf("parent received '%.*s'\n", n, buf);
    }

    return 0;
}

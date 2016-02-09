#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
// #include <unistd>

void handler(int signal) {
    std::cout << "SIG[" << signal << "]" << std::endl;
}

int main() {

    pid_t pid = fork();

    if (pid > 0) {
        std::ofstream f;
        f.open("/home/box/pid", std::ios::out);
        f << pid;
        f.close();

        int statusp;
        waitpid(pid, &statusp, WUNTRACED);
        std::cout << "Child " << pid << " is dead" << std::endl;

        remove("/home/box/pid");
    } else {
        sigset_t signals;
        sigfillset(&signals);
        sigdelset(&signals, SIGTERM);
        sigdelset(&signals, SIGINT);
        sigprocmask(SIG_SETMASK, &signals, &signals);

        struct sigaction act;
        act.sa_handler = handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGINT, &act, 0);
        sigaction(SIGTERM, &act, 0);

        while(1) {
            pause();
        }
    }


    return 0;
}

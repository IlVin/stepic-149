#include <string>
#include <vector>
#include <iostream>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "tserver.h"


#define OPTSTRING "h:p:d:"

// PROTOTYPE: http://www.skitoy.com/p/writing-an-echo-server-in-libev-and-c/375

int main(int argc, char *argv[]) {

    std::string host = "localhost", port = "33334", folder = ".";

    // Read options
    int opt = getopt( argc, argv, OPTSTRING );
    while (opt != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = optarg;
                break;
            case 'd':
                folder = optarg;
                break;
        }
        opt = getopt( argc, argv, OPTSTRING );
    }

    // Create child process
    pid_t process_id = fork();

    // ERROR fork
    if (process_id < 0) {
        std::cout << "fork failed!" << std::endl;
        exit(1);
    }

    // PARENT PROCESS. Need to kill it.
    if (process_id > 0) {
        std::cout << "process_id of child process " << process_id << std::endl;
        exit(0);
    }

    //unmask the file mode
    umask(0);

    //set new session
    pid_t sid = setsid();

    // ERROR new session
    if(sid < 0) {
        exit(1);
    }

    // Change the current working directory to folder.
    chdir(folder.c_str());

    // Close stdin. stdout and stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    TServer * srv = new TServer(host, port, folder);
    srv->start();
    delete srv;

    return 0;
}

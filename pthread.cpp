#include <iostream>
#include <string>
#include <fstream>

#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include <sys/mman.h>
#include <pthread.h>

void * second_thread(void *arg) {
    pause();
    return NULL;
}

int main() {

    std::ofstream ofs;
    ofs.open("/home/box/main.pid");
    ofs << getpid();
    ofs.close();

    pthread_t s_thread;

    if(pthread_create(&s_thread, NULL, second_thread, NULL)) {
        perror("Error creating s_thread");
        return 1;
    }

    if(pthread_join(s_thread, NULL)) {
        perror("Error joining s_thread\n");
        return 2;
    }

    return 0;
}

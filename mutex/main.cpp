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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void * wait_mutex_pthread(void *arg) {
    std::cout << "wait_mutex_pthread: mutex_lock" << std::endl;
    pthread_mutex_lock (&mutex);
    std::cout << "wait_mutex_pthread: mutex_unlock" << std::endl;
    pthread_mutex_unlock (&mutex);
    return NULL;
}


pthread_spinlock_t spin;

void * wait_spin_pthread(void *arg) {
    std::cout << "wait_spin_pthread: spin_lock" << std::endl;
    pthread_spin_lock (&spin);
    std::cout << "wait_spin_pthread: spin_unlock" << std::endl;
    pthread_spin_unlock (&spin);
    return NULL;
}

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
void * wait_rdlock_pthread(void *arg) {
    std::cout << "wait_rdlock_pthread: lock" << std::endl;
    pthread_rwlock_rdlock(&rwlock);
    std::cout << "wait_rdlock_pthread: unlock" << std::endl;
    pthread_rwlock_unlock (&rwlock);
    return NULL;
}

void * wait_rwlock_pthread(void *arg) {
    std::cout << "wait_wrlock_pthread: lock" << std::endl;
    pthread_rwlock_wrlock(&rwlock);
    std::cout << "wait_wrlock_pthread: unlock" << std::endl;
    pthread_rwlock_unlock (&rwlock);
    return NULL;
}

pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void * wait_cond_pthread(void *arg) {
    std::cout << "wait_cond_pthread: lock" << std::endl;
    pthread_cond_wait(&cond, &cond_mutex);
    std::cout << "wait_cond_pthread: unlock" << std::endl;
    return NULL;
}

pthread_barrier_t bp;
void * wait_barrier_pthread(void *arg) {
    std::cout << "wait_barrier_pthread: lock" << std::endl;
    pthread_barrier_wait(&bp);
    std::cout << "wait_barrier_pthread: unlock" << std::endl;
    return NULL;
}

typedef void * (*wait_fn_thread)(void *);
#define THCNT 6

int main() {

    // Save pid file
    std::ofstream ofs;
    ofs.open("/home/box/main.pid");
    ofs << getpid();
    ofs.close();

    // Init mutexes
    pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);
    pthread_barrier_init(&bp, NULL, 2);

    // Lock mutexes
    std::cout << "main: mutex_lock" << std::endl;
    pthread_mutex_lock (&mutex);
    std::cout << "main: spin_lock" << std::endl;
    pthread_spin_lock(&spin);
    std::cout << "main: wr_lock" << std::endl;
    pthread_rwlock_wrlock(&rwlock);


    // Start pthreads
    wait_fn_thread fn[THCNT]  = {
        wait_mutex_pthread, wait_spin_pthread, wait_rdlock_pthread,
        wait_rwlock_pthread, wait_cond_pthread, wait_barrier_pthread
    };
    pthread_t thread_ids[THCNT];

    for (int i = 0; i < THCNT; i++) {
        if(pthread_create(&thread_ids[i], NULL, fn[i], NULL)) {
            perror("Error creating pthread");
            return 1;
        }
    }

    std::cout << std::endl << "main: sleep(10)" << std::endl << std::endl;
    sleep(10);

    // UnLock mutexes
    std::cout << "main: mutex_unlock" << std::endl;
    pthread_mutex_unlock (&mutex);
    std::cout << "main: spin_unlock" << std::endl;
    pthread_spin_unlock(&spin);
    std::cout << "main: wr_unlock" << std::endl;
    pthread_rwlock_unlock(&rwlock);
    pthread_cond_signal(&cond);
    pthread_barrier_wait(&bp);

    // Join pthreads
    for (int i = 0; i < THCNT; i++) {
        if(pthread_join(thread_ids[i], NULL)) {
            perror("Error joining pthread");
            return 2;
        }
    }

    return 0;
}

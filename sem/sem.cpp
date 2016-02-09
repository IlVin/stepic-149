#include <iostream>
#include <string>
#include <fstream>

#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <sys/sem.h>    /* for shmget etc */
#include <errno.h>  /* errno and perror */
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>

#define DEFAULT_TOK_ID 1
key_t get_tok(std::string &path, int tok_id = DEFAULT_TOK_ID) {
    if (std::ifstream(path.c_str()) == NULL) {
        std::cout << "Create file " << path << std::endl;
        std::ofstream ofs(path.c_str());
        ofs << std::endl;
        ofs.close();
    }
    return ftok(path.c_str(), tok_id);
}

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int sem_reset(int semid) {
    struct semid_ds ds;
    semun arg4;

    arg4.buf = &ds;
    if (semctl(semid,0,IPC_STAT,arg4) == 0) {
        int nsems = ds.sem_nsems;
        ushort *semvals = new ushort[nsems];
        for (int i = 0; i < nsems; i++) semvals[i] = 0;
        arg4.array = semvals;
        semctl(semid, 0, SETALL, arg4);
        delete[] semvals;
    }

    return semid;
}

void sem_dump(int semid) {
    struct semid_ds ds;
    semun arg4;

    arg4.buf = &ds;
    if (semctl(semid,0,IPC_STAT,arg4) == 0) {
        int nsems = ds.sem_nsems;
        std::cout << "NSEMS: " << nsems <<std::endl;
        ushort *semvals = new ushort[nsems];
        arg4.array = semvals;
        semctl(semid, 0, GETALL, arg4);
        for (int i = 0; i < nsems; i++) std::cout << "sem[" << i << "] == " << semvals[i] << std::endl;
        delete[] semvals;
    }

    return;
}

int sem_create(std::string &path, int nsems, int tok_id) {
    key_t tok = get_tok(path, tok_id);
    int semid = semget(tok, nsems, 0666 | IPC_EXCL | IPC_CREAT);
    if (semid == -1) semid = semget(tok, nsems, 0666);
    return sem_reset(semid);
}

void posix_sem(std::string &path, uint val) {
    sem_t * sem = sem_open(path.c_str(), O_CREAT, 0666, val);
    pause();
    sem_close(sem);
}

#define NSEMS 16
int main () {
    std::string path = "/test.sem";
    posix_sem(path, 66);
    return 0;
}

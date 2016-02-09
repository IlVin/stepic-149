#include <iostream>
#include <fstream>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include <sys/mman.h>


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

void sysv_task() {
    std::string path = "/tmp/mem.temp";
    key_t key = get_tok(path, 1);

    size_t size = 1024;
    int shmid = shmget(key, size, 0666 | IPC_CREAT);
    if (shmid == -1) perror("shmget");

    char *data = (char *)shmat(shmid, (void *)NULL, SHM_RND);
    if (data == (char*)(-1)) perror("shmat");

    for (int i = 0; i < 1024; i++) data[i] = 42;

    shmdt(data);
}


void posix_task() {
    std::string path = "/test.shm";
    int size = 1024 * 1024;
    int fd = shm_open(path.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) perror("shm_open");

    if (ftruncate(fd, size) == -1) perror("truncate");

    char * data = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) perror("mmap");

    for (int i = 0; i < size; i++) data[i] = 13;

    std::cout << "Ready" << std::endl;
    pause();

    close(fd);
    shm_unlink(path.c_str());
}

int main() {

    posix_task();

    return 0;
}

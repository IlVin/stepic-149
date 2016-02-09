#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <mqueue.h>
#include <sys/msg.h>

#define MAX_SIZE 100

int msg_create (std::string &path, int flags) {
    // Нужно проверить существование path
    key_t qtok = ftok(path.c_str(), 0);
    return msgget(qtok, flags | IPC_CREAT);
}

int msg_open_rd (std::string &path) {
    return msg_create(path, 0666);
}

int msg_open_wr (std::string &path) {
    return msg_create(path, 0666);
}


#define MSG_PKG_SIZE 100
struct msg_package {
    long mtype;
    char mtext[MSG_PKG_SIZE];
};

int msg_snd(int qid, std::string &msg) {
    msg_package pkg;
    pkg.mtype = 1;
    std::strcpy (&(pkg.mtext[0]), msg.c_str());
    int rc = msgsnd(qid, &pkg, MSG_PKG_SIZE, 0);
    return rc;
}

std::string msg_recv(int qid) {
    msg_package pkg;
    pkg.mtype = 0;
    int rc = msgrcv(qid, &pkg, MSG_PKG_SIZE, 0, 0);
    std::string msg(pkg.mtext);
    return msg;
}

mqd_t mq_create (const char *path, int flags){
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
    return mq_open(path, flags | O_CREAT, 0666, &attr);
}

mqd_t mq_open_rd (const char *path){
    mqd_t mqid = mq_open(path, O_RDONLY | O_EXCL);
    if (mqid != -1) return mqid;
    return mq_create(path, O_RDONLY);
}

mqd_t mq_open_wr (const char *path){
    mqd_t mqid = mq_open(path, O_WRONLY | O_EXCL);
    if (mqid != -1) return mqid;
    return mq_create(path, O_WRONLY);
}

int mq_snd(mqd_t mqd, std::string &str) {
    return mq_send(mqd, str.c_str(), str.size(), 0);
}

std::string mq_rcv(mqd_t mqd) {
    struct mq_attr attr;
    int rc;
    rc = mq_getattr(mqd, &attr);
    std::cout << "RC_GETATTR: " << rc << "; MSGSIZE: " << attr.mq_msgsize << std::endl;

    char *msgptr = (char*) malloc(attr.mq_msgsize);
    rc = mq_receive(mqd, msgptr, attr.mq_msgsize, 0);

    std::string str(msgptr);
    free(msgptr);
    return str;
}

int main() {

    char buffer[MAX_SIZE + 1];

    mqd_t mqd1 = mq_open_rd("/test.mq");
    std::cout << "MQD1: " << mqd1 << std::endl;

    mqd_t mqd2 = mq_open_wr("/test.mq");
    std::cout << "MQD2: " << mqd2 << std::endl;


    std::string snd_str = "Hello!!!";
    mq_snd(mqd2, snd_str);
    std::string rcv_str = mq_rcv(mqd1);

    std::cout << "RCV_STR: " << rcv_str << std::endl;
    return 0;
}

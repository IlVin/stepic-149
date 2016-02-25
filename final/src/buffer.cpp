#include <iostream>
#include <string>
#include <cstring>
#include <assert.h>

#include "buffer.h"


char ** TBuffer::head() {
    return &hd;
};

char ** TBuffer::tail() {
    return &tl;
};

size_t TBuffer::length() {
    return tl - hd;
};

bool TBuffer::space(size_t need_space) {
    while (raw + size - tl < need_space) {
        size_t o1 = hd - raw;
        size_t o2 = tl - raw;
        size *= 2;
        raw = (char *)realloc((void *)raw, size);
        hd = raw + o1;
        tl = raw + o2;
    }
    return true;
};

void TBuffer::append(const std::string &str) {
    if (space(str.size()+1)) {
        memcpy(tl, str.c_str(), str.size());
        tl += str.size();
    }
};

std::string TBuffer::asString() {
    *tl = '\0';
    return std::string(hd);
}

int TBuffer::rcv(int fd, size_t sz) {
    if (!space(sz+1)) return -1;

    int r = recv(fd, tl, sz, MSG_NOSIGNAL);
    if (r  > 0) {
        tl += r;
        *tl = '\0';
    }
    return r;
};

int TBuffer::snd(int fd) {
    if (length() <= 0) return 0;
    int r = send(fd, hd, length(), 0);
    if (r > 0) {
        hd += r;
    }
    return r;
};

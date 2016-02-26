#include <string>
#include <cstring>
#include "request.h"
#include "buffer.h"


bool is(char ** p, char ch) {
    if (**p == ch) {
        (*p)++;
        return true;
    }
    return false;
}

bool is(char ** p, std::string ch_str) {
    const char * ch = ch_str.c_str();
    for (;*ch != '\0'; ch++) {
        if (is(p, *ch)) return true;
    }
    return false;
}

bool is_token(char ** p, std::string tok_str) {
    const char * tok = tok_str.c_str();
    char * ptr = *p;
    for (;*tok != '\0'; tok++) {
        if (!is(&ptr, *tok)) return false;
    }
    *p = ptr;
    return true;
}

bool skip(char ** p, std::string ch_str) {
    const char * ch = ch_str.c_str();
    bool result = false;
    while (is(p, ch)) result = true;
    return result;
}

bool TRequest::parse(){
    char * p = *(rb->head());
    char * t = *(rb->tail());

    if (is_token(&p, "GET") && skip(&p, " ")) {
        char * pth;
        int len;

        if (is_token(&p, "http://")) skip(&p, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_~.:@");
        if (!is(&p, '/')) return false;
        pth = p - 1;
        skip(&p, "/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_~.%");
        if (is(&p, '?')) {
            len = p - pth - 1;
            skip(&p, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_~.%");
            if (!is(&p, ' ')) return false;
        } else if (is(&p, ' ')) {
            len = p - pth - 1;
        } else {
            return false;
        }
//        if(!is_token(&p, "HTTP")) return false;
        path = new char[len+1];
        for (size_t i = 0; i < len; i++) {
            path[i] = pth[i];
        }
        path[len] = '\0';
        return true;
    }
    return false;
};

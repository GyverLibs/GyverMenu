#include "utils.h"

namespace mutil {

uint8_t countSub(const char* str, char div) {
    uint8_t res = 1;
    while (*str) {
        if (*str++ == div) ++res;
    }
    return res;
}

uint8_t getSub(const char* str, char div, uint8_t n, const char** start) {
    while (*str && n) {
        if (*str++ == div) --n;
    }
    *start = str;
    uint8_t len = 0;
    while (*str && *str != div) ++str, ++len;
    return len;
}

uint8_t strlenU(const char* str, uint8_t* letters) {
    uint8_t len = 0;
    *letters = 0;
    while (*str) {
        if ((*str & 0xc0) != 0x80) ++*letters;
        ++len;
        ++str;
    }
    return len;
}
uint8_t strlenU(const char* str, uint8_t* letters, int8_t maxlen) {
    uint8_t len = 0;
    *letters = 0;
    while (*str && maxlen) {
        if ((*str & 0xc0) != 0x80) ++*letters;
        ++len;
        ++str;
        if (maxlen > 0) --maxlen;
    }
    return len;
}

void _dummyRender(const char* str, size_t len) {}

}  // namespace mutil

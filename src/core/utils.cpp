#include "utils.h"

namespace gmutil {

const char* findChar_P(const char* str, char c) {
    for (;;) {
        char ch = GM_PGM_READ(str);
        if (ch == c) return str;
        if (!ch) return nullptr;
        ++str;
    }
}

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

uint8_t strlenU(const char* str, uint8_t* letters, uint8_t maxBytes, uint8_t maxLetters) {
    uint8_t len = 0;
    *letters = 0;
    while (*str && len < maxBytes) {
        if ((*str & 0xc0) != 0x80) {
            if (*letters >= maxLetters) break;
            ++*letters;
        }
        ++str;
        ++len;
    }
    return len;
}

void _dummyRender(const char*, size_t) {}

}  // namespace gmutil

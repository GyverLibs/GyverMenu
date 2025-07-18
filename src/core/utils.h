#pragma once
#include <stdint.h>
#include <string.h>

#define GM_READ_PGM(fstr, str)                 \
    char str[strlen_P((const char*)fstr) + 1]; \
    strcpy_P(str, (const char*)fstr);          \
    str[sizeof(str) - 1] = 0;

namespace mutil {
uint8_t countSub(const char* str, char div);

uint8_t getSub(const char* str, char div, uint8_t n, const char** start);

uint8_t strlenU(const char* str, uint8_t* letters);
uint8_t strlenU(const char* str, uint8_t* letters, int8_t maxlen);

void _dummyRender(const char* str, size_t len);

}  // namespace mutil

#pragma once
#include <stdint.h>
#include <string.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

// ==================== PROGMEM ====================

#ifdef ARDUINO

#define GM_PROGMEM PROGMEM
#define GM_PGM_READ(ptr) ((char)pgm_read_byte(ptr))
#define GM_PGM_FIND(str, c) strchr_P(str, c)
#define GM_PGM_LEN(str) strlen_P(str)

#define GM_READ_PGM(fstr, str)                 \
    char str[strlen_P((const char*)fstr) + 1]; \
    strcpy_P(str, (const char*)fstr)

#else

#define GM_PROGMEM
#define GM_PGM_READ(ptr) (*(ptr))
#define GM_PGM_FIND(str, c) strchr(str, c)
#define GM_PGM_LEN(str) strlen(str)

#endif

namespace gmutil {

uint8_t countSub(const char* str, char div);

uint8_t getSub(const char* str, char div, uint8_t n, const char** start);

uint8_t strlenU(const char* str, uint8_t* letters);

uint8_t strlenU(const char* str, uint8_t* letters, uint8_t maxlen);

void _dummyRender(const char*, size_t);

}  // namespace gmutil
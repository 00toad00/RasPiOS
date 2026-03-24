#ifndef STRING_H
#define STRING_H

#include <stdint.h>

typedef char *string;
typedef char ustring;

typedef enum StrConv {
    STRCONV_UPPER,
    STRCONV_LOWER
} StrConv;

uint8_t strlen8(const string str);
uint16_t strlen16(const string str);
void strcpy(string dest, const string src);
void itoa_int(uint64_t value, string str);
void itoa_hex(uint64_t value, string str);
void itoa_bin(uint64_t value, string str);
void strconv(string str, uint8_t conv);

#endif
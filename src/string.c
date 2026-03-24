#include <string.h>

uint8_t strlen8(const string str) {
    for (uint8_t i = 0; i < UINT8_MAX; i++) {
        if (str[i] == 0) { return i; }
    }
    return 0;
}

uint16_t strlen16(const string str) {
    for (uint16_t i = 0; i < UINT16_MAX; i++) {
        if (str[i] == 0) { return i; }
    }
    return 0;
}

void strcpy(string dest, const string src) {
    uint16_t len = strlen16(src);
    for (uint16_t i = 0; i <= len; i++) { dest[i] = src[i]; }
    return;
}

void itoa_int(uint64_t value, string str) {
    ustring tmp[20];   // max digits in 64-bit decimal = 20
    int i = 0, j = 0;

    // Special case: zero
    if (value == 0) {
        str[j++] = '0';
        str[j] = '\0';
        return;
    }

    // Convert to decimal (in reverse)
    while (value > 0) {
        tmp[i++] = '0' + (value % 10);
        value /= 10;
    }

    // Reverse into the output string
    while (i > 0) {
        str[j++] = tmp[--i];
    }

    str[j] = '\0';
}

void itoa_hex(uint64_t value, string str) {
    char hex[] = "0123456789ABCDEF";
    char tmp[32];
    int i = 0, j = 0;

    if (value == 0) {
        str[j++] = '0';
        str[j++] = 'h';
        str[j] = '\0';
        return;
    }

    while (value > 0) {
        tmp[i++] = hex[value & 0xF];
        value >>= 4;
    }

    while (i > 0) {
        str[j++] = tmp[--i];
    }

    str[j++] = 'h';
    str[j] = '\0';
    return;
}

void itoa_bin(uint64_t value, string str) {
    int i = 63;
    int j = 0;
    int started = 0;

    while (i >= 0) {

        if (value & (1ULL << i)) {
            str[j] = '1';
            j++;
            started = 1;
        } else {
            if (started == 1) {
                str[j] = '0';
                j++;
            }
        }

        i--;
    }

    // If value was 0, print single '0'
    if (j == 0) {
        str[0] = '0';
        j = 1;
    }
    str[j++] = 'b';
    str[j] = '\0';
    return;
}

void strconv(string str, uint8_t conv) {
    if (conv == STRCONV_LOWER) {
        for (uint16_t i = 0; str[i] != 0; i++) {
            if (str[i] >= 'A' && str[i] <= 'Z') { str[i] += 0x20; }
            else if (str[i] >= 'a' && str[i] <= 'z') { str[i] -= 0x20; }
        }
    } else if (conv == STRCONV_UPPER) {
        for (uint16_t i = 0; str[i] != 0; i++) {
            if (str[i] >= 'A' && str[i] <= 'Z') { str[i] += 0x20; }
            else if (str[i] >= 'a' && str[i] <= 'z') { str[i] -= 0x20; }
        }
    }
}
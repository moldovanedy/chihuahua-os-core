#include "string.h"

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    
    while (*src != 0) {
        *d = *src;
        d++;
        src++;
    }

    return dest;
}
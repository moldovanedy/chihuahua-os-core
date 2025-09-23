#include "string.h"

char *strncpy(char *dest, const char *source, size_t num) {
    char *d = dest;
    
    while (*source != 0) {
        *d = *source;
        d++;
        source++;
        num--;
    }

    memset(dest, 0, num);
    return dest;
}
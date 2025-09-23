#include "cstring.h"

char *strncat(char *dest, const char *source, size_t num) {
    char *a = dest;
    dest += strlen(dest);
    
    while (num != 0 && *source != 0) {
        *dest = *source;
        dest++;
        source++;
        num--;
    }
    
    *dest = 0;
    return a;
}
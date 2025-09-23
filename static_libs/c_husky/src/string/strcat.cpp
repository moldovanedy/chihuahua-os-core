#include "cstring.h"

char *strcat(char *dest, const char *source) {
    strcpy(dest + strlen(dest), source);
    return dest;
}
#ifndef C_HUSKY_STRING_H
#define C_HUSKY_STRING_H

#include <cstddef>

extern "C" {
    void *memcpy(void *dest, const void *src, size_t n);
    void *memmove(void *dest, const void *src, size_t n);
    char *strcpy(char *dest, const char *src);
    char *strncpy(char *dest, const char *src, size_t n);

    char *strcat(char *dest, const char *src);
    char *strncat(char *dest, const char *src, size_t n);
    
    int memcmp(const void *left, const void *right, size_t n);
    
    void *memset(void* dest, int c, size_t n);
    size_t strlen(const char *str);
}

#endif //C_HUSKY_STRING_H

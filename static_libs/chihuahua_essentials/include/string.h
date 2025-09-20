#ifndef CHIHUAHUA_ESSENTIALS_STRING_H
#define CHIHUAHUA_ESSENTIALS_STRING_H

#include <cstddef>

extern "C" {
    void *memset(void* dest, int c, size_t n);
    void *memcpy(void *dest, const void *src, size_t n);
    int memcmp(const void *vl, const void *vr, size_t n);
    void *memmove(void *dest, const void *src, size_t n);
}

#endif //CHIHUAHUA_ESSENTIALS_STRING_H

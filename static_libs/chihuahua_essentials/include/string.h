#ifndef STRING_H
#define STRING_H

#include <stddef.h>

void *memset(void* dest, int c, size_t n);
void *memcpy(void *restrict dest, const void *restrict src, size_t n);
int memcmp(const void *vl, const void *vr, size_t n);
void *memmove(void *dest, const void *src, size_t n);

#endif //STRING_H

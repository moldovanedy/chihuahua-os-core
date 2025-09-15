#ifndef BOOTLOADER_SRC_TINY_LIBC_STRING_H
#define BOOTLOADER_SRC_TINY_LIBC_STRING_H

#include <cstddef>

void *memset(void* dest, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *vl, const void *vr, size_t n);
void *memmove(void *dest, const void *src, size_t n);

#endif // BOOTLOADER_SRC_TINY_LIBC_STRING_H

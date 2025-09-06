#include <stddef.h>
#include <stdint.h>

#include "string.h"

void *memmove(void *dest, const void *src, size_t n)
{
    char *d = dest;
    const char *s = src;

    if (d==s) {
        return d;
    }
    if ((uintptr_t)s - (uintptr_t)d - n <= -2 * n) {
        return memcpy(d, s, n);
    }

    if (d<s) {
        for (; n; n--) {
            *d = *s;
            d++;
            s++;
        }
    } else {
        while (n) {
            d[n] = s[n];
            n--;
        }
    }

    return dest;
}

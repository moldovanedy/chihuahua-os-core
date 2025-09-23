#include <cstdint>

#include "cstring.h"

void *memmove(void *dest, const void *src, size_t n)
{
    auto *d = static_cast<char *>(dest);
    const auto *s = static_cast<const char *>(src);

    if (d==s) {
        return d;
    }
    if (reinterpret_cast<uintptr_t>(s) - reinterpret_cast<uintptr_t>(d) - n <= -2 * n) {
        return memcpy(d, s, n);
    }

    if (d<s) {
        for (; n != 0; n--) {
            *d = *s;
            d++;
            s++;
        }
    } else {
        while (n != 0) {
            d[n] = s[n];
            n--;
        }
    }

    return dest;
}

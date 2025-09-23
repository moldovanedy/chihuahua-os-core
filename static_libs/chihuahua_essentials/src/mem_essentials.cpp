#include <cstdint>

#include "chihuahua_essentials/mem_essentials.h"

int memcmp(const void *vl, const void *vr, size_t n) {
    const auto *l = static_cast<const unsigned char *>(vl);
    const auto *r = static_cast<const unsigned char *>(vr);
    for (; n != 0 && *l == *r; n--, l++, r++){}
    return n != 0 ? *l-*r : 0;
}

void *memcpy(void *dest, const void *src, size_t n) {
    auto *d = static_cast<unsigned char *>(dest);
    const auto *s = static_cast<const unsigned char *>(src);

    for (; n != 0; n--) {
        *d = *s;
        d++;
        s++;
    }
    
    return dest;
}

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

void *memset(void* dest, int c, size_t n) {
    //some code taken from Musl libc for optimisation
    auto *traverser = static_cast<unsigned char *>(dest);

    if (n == 0) {
        return dest;
    }

    /* Fill head and tail with minimal branching. Each
     * conditional ensures that all the subsequently used
     * offsets are well-defined and in the dest region. */

    traverser[0] = traverser[n-1] = c;
    if (n <= 2) {
        return dest;
    }
    
    traverser[1] = traverser[n-2] = c;
    traverser[2] = traverser[n-3] = c;
    if (n <= 6) {
        return dest;
    }
    
    traverser[3] = traverser[n-4] = c;
    if (n <= 8) {
        return dest;
    }
    
    /* Advance pointer to align it at a 4-byte boundary,
     * and truncate n to a multiple of 4. The previous code
     * already took care of any head/tail that get cut off
     * by the alignment. */

    const size_t k = -reinterpret_cast<uintptr_t>(traverser) & 3;
    traverser += k;
    n -= k;
    n &= -4;
    n /= 4;

    auto *w_traverser = reinterpret_cast<uint32_t *>(traverser);
    uint32_t w_c = c & 0xFF;
    w_c |= w_c << 8 | w_c << 16 | w_c << 24;

    /* Pure C with no aliasing violations. */
    for (; n != 0; n--, w_traverser++) {
        *w_traverser = w_c;
    }

    //simple, unoptimized implementation
    
    // while (n--) {
    //     *s = c;
    //     s++;
    // }

    return dest;
}
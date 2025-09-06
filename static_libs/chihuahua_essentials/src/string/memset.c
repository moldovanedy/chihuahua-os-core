#include <stddef.h>
#include <stdint.h>

#include "string.h"

void *memset(void* dest, int c, size_t n) {
    //some code taken from Musl libc for optimisation
    unsigned char *traverser = dest;

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

    const size_t k = -(uintptr_t)traverser & 3;
    traverser += k;
    n -= k;
    n &= -4;
    n /= 4;

    uint32_t *w_traverser = (uint32_t *)traverser;
    uint32_t w_c = c & 0xFF;
    w_c |= w_c << 8 | w_c << 16 | w_c << 24;

    /* Pure C with no aliasing violations. */
    for (; n; n--, w_traverser++) {
        *w_traverser = w_c;
    }

    //simple, unoptimized implementation
    
    // while (n--) {
    //     *s = c;
    //     s++;
    // }

    return dest;
}

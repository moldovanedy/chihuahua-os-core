#include "string.h"

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

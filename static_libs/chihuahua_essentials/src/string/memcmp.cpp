#include "string.h"

int memcmp(const void *vl, const void *vr, size_t n) {
    const auto *l = static_cast<const unsigned char *>(vl);
    const auto *r = static_cast<const unsigned char *>(vr);
    for (; n != 0 && *l == *r; n--, l++, r++){}
    return n != 0 ? *l-*r : 0;
}

#include "cstring.h"

int memcmp(const void *left, const void *right, size_t n) {
    const auto *l = static_cast<const unsigned char *>(left);
    const auto *r = static_cast<const unsigned char *>(right);
    for (; n != 0 && *l == *r; n--, l++, r++){}
    return n != 0 ? *l-*r : 0;
}

#ifndef BOOTLOADER_STRING_UTILS_H
#define BOOTLOADER_STRING_UTILS_H

#include <efi.h>

namespace Utils {
    bool int32ToWideCharN(int value, CHAR16* str, int bufSize, int base = 10);
    bool int64ToWideCharN(int64_t value, CHAR16* str, int bufSize, int base = 10);
} //namespace Utils

#endif //BOOTLOADER_STRING_UTILS_H
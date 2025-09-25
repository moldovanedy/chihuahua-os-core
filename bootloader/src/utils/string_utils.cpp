#include "string_utils.h"

constexpr int MIN_BASE = 2;
constexpr int MAX_BASE = 36;

constexpr CHAR16 DIGITS[] = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

bool Utils::int32ToWideCharN(int value, CHAR16* str, const int bufSize, const int base) {
    return int64ToWideCharN(value, str, bufSize, base);
}

bool Utils::int64ToWideCharN(int64_t value, CHAR16* str, const int bufSize, const int base) {
    const int limit = bufSize - 1;

    if (base < MIN_BASE || base > MAX_BASE) {
        return false;
    }
    
    bool isNegative = false;
    if (value < 0 && base == 10) {
        isNegative = true;
        value = -value;
    }

    int idx = 0;
    while (idx < limit) {
        const int remainder = value % base;
        if (remainder > MAX_BASE - 1) {
            return false;
        }

        str[idx] = remainder > 9 ? 'A' + (remainder - 10)  : '0' + remainder;
        
        value /= base;
        idx++;

        if (value == 0) {
            break;
        }
    }

    if (isNegative) {
        if (idx == limit) {
            return false;
        }

        str[idx] = L'-';
        idx++;
    }

    const int actualSize = idx;

    int start = 0;
    int end = actualSize - 1;
    while (start < end) {
        const CHAR16 temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }

    str[actualSize + 1] = '\0';
    return true;
}
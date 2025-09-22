#ifndef MAIN_H
#define MAIN_H

#include <efi.h>

extern "C" EFI_STATUS efi_main(EFI_HANDLE handle, EFI_SYSTEM_TABLE *st);

namespace Log {
    EFI_STATUS print(CHAR16 *str);
}

[[noreturn]] void panic();

#endif //MAIN_H
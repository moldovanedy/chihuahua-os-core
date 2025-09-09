#ifndef MAIN_H
#define MAIN_H

#include <efi.h>

extern EFI_STATUS efi_main(EFI_HANDLE h, EFI_SYSTEM_TABLE *st);

EFI_STATUS print(CHAR16 *str);

#endif //MAIN_H

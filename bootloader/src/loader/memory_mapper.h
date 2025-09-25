#ifndef BOOTLOADER_MEMORY_MAPPER_H
#define BOOTLOADER_MEMORY_MAPPER_H

#include <efi.h>

#include "boot_params.h"

namespace MemoryMapper {
    void mapMemory(EFI_BOOT_SERVICES *bs, const MemoryMap_t *memoryMap);
} //namespace MemoryMapper

#endif //BOOTLOADER_MEMORY_MAPPER_H
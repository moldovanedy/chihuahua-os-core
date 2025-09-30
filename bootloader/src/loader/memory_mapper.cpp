#include "memory_mapper.h"

#include "chihuahua_essentials/mem_essentials.h"
#include "paginator/page_table.h"
#include "src/main.h"

namespace MemoryMapper {
    static EFI_BOOT_SERVICES *cachedBs;
    
    void mapMemory(EFI_BOOT_SERVICES *bs, const MemoryMap_t *memoryMap) {
        EFI_PHYSICAL_ADDRESS pageTablesPtr = 0;
        const EFI_STATUS status = bs->AllocatePages(
            AllocateAnyPages, EfiLoaderData, 1, &pageTablesPtr);

        if (EFI_ERROR(status)) {
            return;
        }

        cachedBs = bs;
        memset(reinterpret_cast<void *>(pageTablesPtr), 0, EFI_PAGE_SIZE);

        const auto pageTableController = Paginator::PageTableRootController(
            reinterpret_cast<Paginator::PageTable_t *>(pageTablesPtr),
            [] {
                EFI_PHYSICAL_ADDRESS ptAddress = 0;
                const EFI_STATUS ptAllocStatus = cachedBs->AllocatePages(
                    AllocateAnyPages, EfiLoaderData, 1, &ptAddress);

                // CHAR16 string[15] = {};
                // memset(string, 0, sizeof(string));
                // Utils::int64ToWideCharN(ptAddress, string, sizeof(string) / 2);
                // Log::print(string);
                // Log::print(L"\r\n");

                if (EFI_ERROR(ptAllocStatus) || ptAddress == 0 || ptAddress % EFI_PAGE_SIZE != 0) {
                    Log::print(L"FATAL: Could not allocate memory for paging.\r\n");
                    panic();
                }

                return ptAddress;
            },
            true);

        const size_t mapSize = memoryMap->mem_map_size / memoryMap->entry_size;
        for (size_t i = 0; i < mapSize; i++) {
            //we need to respect the entry size, as it seems that some firmware (QEMU at least) don't respect
            // sizeof(MemoryMapEntry_t) and instead add some padding
            const MemoryMapEntry_t *entry =
                reinterpret_cast<MemoryMapEntry_t *>(
                    reinterpret_cast<char*>(memoryMap->entries) + (i * memoryMap->entry_size));
            
            if (entry->type == EfiConventionalMemory) {
                continue;
            }

            for (size_t j = 0; j < entry->page_count; j++) {
                const Paginator::PageMapError err = pageTableController.identityMapPage(
                    entry->physical_start + (EFI_PAGE_SIZE * j),
                    Paginator::PageFlags::Present
                    | Paginator::PageFlags::ReadBit
                    | Paginator::PageFlags::WriteBit
                    | Paginator::PageFlags::ExecuteBit
                    | Paginator::PageFlags::UserModeAccessible);

                if (err != Paginator::PageMapError::NoError) {
                    Log::print(L"FATAL: Could not set up paging (mapping error).\r\n");
                    panic();
                }
            }
        }

        const bool success = pageTableController.activateRootPageTable();
        if (!success) {
            Log::print(L"FATAL: Could not activate paging.\r\n");
            panic();
        }

        cachedBs = nullptr;
    }
}

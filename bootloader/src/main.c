#include <c-efi.h>
#include <stdbool.h>

#include "boot_params.h"
#include "main.h"

/**
 * Returns the current memory map. Might fail in multiple ways, so always check isSuccessful. If it's not successful,
 * the returned value is invalid and should not be used.
 * @param isSuccessful A reference pointer that is true if the function successfully got and returned a memory map,
 * false otherwise.
 * @return The memory map or an invalid value if something failed.
 */
static MemoryMap get_memory_map(bool* isSuccessful);

static CEfiSystemTable *system_table = NULL;
static CEfiSimpleTextOutputProtocol *cout = NULL;

CEfiStatus print(CEfiChar16 *str) {
    if (cout == NULL) {
        return C_EFI_DEVICE_ERROR;
    }
    
    return cout->output_string(cout, str);
}

// ReSharper disable once CppParameterNeverUsed
CEfiStatus efi_main(CEfiHandle h, CEfiSystemTable *st) {
    system_table = st;
    cout = system_table->con_out;
    
    CEfiStatus r = print(L"Start booting ChihuahuaOS.\r\n");
    if (C_EFI_ERROR(r)) {
        return r;
    }

    bool isSuccessful;
    MemoryMap mem_map = get_memory_map(&isSuccessful);
    if (isSuccessful) {
        print(L"Memory map retrieved.\r\n");
    }
    else {
        print(L"Memory map failed.\r\n");
    }

    CEfiUSize x;
    r = system_table->boot_services->wait_for_event(1, &system_table->con_in->wait_for_key, &x);
    if (C_EFI_ERROR(r)) {
        return r;
    }

    return 0;
}

static MemoryMap get_memory_map(bool* isSuccessful) {
    *isSuccessful = false;
    
    if (system_table == NULL) {
        return INVALID_MEMORY_MAP;
    }

    CEfiPhysicalAddress mem_map_ptr = 0;
    CEfiStatus status = system_table->boot_services->allocate_pages(
        C_EFI_ALLOCATE_ANY_PAGES,
        C_EFI_LOADER_DATA,
        1,
        &mem_map_ptr);
    if (C_EFI_ERROR(status) || mem_map_ptr == 0) {
        return INVALID_MEMORY_MAP;
    }

    CEfiUSize map_size = PAGE_SIZE;
    CEfiUSize map_key = 0;
    CEfiUSize descriptor_size = 0;
    CEfiU32 descriptor_version = 0;
    
    status = system_table->boot_services->get_memory_map(
        &map_size,
        (CEfiMemoryDescriptor*)mem_map_ptr,
        &map_key,
        &descriptor_size,
        &descriptor_version);

    if (!C_EFI_ERROR(status)) {
        MemoryMap mem_map = {
            .mem_map_size = map_size,
            .mem_map_key = map_key,
            .entries = (MemoryMapEntry*)mem_map_ptr,
            .entry_size = descriptor_size,
            .entry_version = descriptor_version,
        };

        *isSuccessful = true;
        return mem_map;
    }

    //if we get an error, and it's not from insufficient memory, then we abort
    if (status != C_EFI_BUFFER_TOO_SMALL) {
        return INVALID_MEMORY_MAP;
    }

    //we free the one page that we allocated, so we can allocate the correct size
    status = system_table->boot_services->free_pages(mem_map_ptr, 1);
    if (C_EFI_ERROR(status)) {
        return INVALID_MEMORY_MAP;
    }

    status = system_table->boot_services->allocate_pages(
        C_EFI_ALLOCATE_ANY_PAGES,
        C_EFI_LOADER_DATA,
        (map_size + PAGE_SIZE - 1) / PAGE_SIZE,
        &mem_map_ptr);

    if (C_EFI_ERROR(status)) {
        return INVALID_MEMORY_MAP;
    }

    //try again
    status = system_table->boot_services->get_memory_map(
        &map_size,
        (CEfiMemoryDescriptor*)mem_map_ptr,
        &map_key,
        &descriptor_size,
        &descriptor_version);
    
    //if it's still an error, abort
    if (C_EFI_ERROR(status)) {
        return INVALID_MEMORY_MAP;
    }
    
    MemoryMap mem_map = {
        .mem_map_size = map_size,
        .mem_map_key = map_key,
        .entries = (MemoryMapEntry*)mem_map_ptr,
        .entry_size = descriptor_size,
        .entry_version = descriptor_version,
    };

    *isSuccessful = true;
    return mem_map;
}

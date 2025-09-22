#include <efi.h>

#include "boot_params.h"
#include "loader/kernel_reader.h"

#include "main.h"

#include "gop.h"

/**
 * Returns the current memory map. Might fail in multiple ways, so always check isSuccessful. If it's not successful,
 * the returned value is invalid and should not be used.
 * @param isSuccessful A reference pointer that is true if the function successfully got and returned a memory map,
 * false otherwise.
 * @return The memory map or an invalid value if something failed.
 */
static MemoryMap_t getMemoryMap(bool *isSuccessful);

static EFI_SYSTEM_TABLE *systemTable = nullptr;
static EFI_SIMPLE_TEXT_OUT_PROTOCOL *cout = nullptr;

EFI_STATUS Log::print(CHAR16 *str) {
    if (cout == nullptr) {
        return EFI_DEVICE_ERROR;
    }

    return cout->OutputString(cout, str);
}

[[noreturn]] void panic() {
    Log::print(L"Boot failed. You can turn off the device now.\r\n");

    while (true) {
        UINTN x;
        systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &x);
    }
}

extern "C" EFI_STATUS efi_main(const EFI_HANDLE handle, EFI_SYSTEM_TABLE *st) {
    systemTable = st;
    cout = systemTable->ConOut;

    Log::print(L"Start booting ChihuahuaOS.\r\n");

    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    EFI_STATUS status = st->BootServices->LocateProtocol(
        &gopGuid, nullptr, reinterpret_cast<void **>(&gop));
    if (EFI_ERROR(status)) {
        Log::print(L"Failed to locate graphics output protocol.\r\n");
        panic();
    }

    FramebufferInfo_t fbInfo = INVALID_FRAMEBUFFER_INFO;
    constexpr int PREFERRED_WIDTH = 1920;
    constexpr int PREFERRED_HEIGHT = 1080;
    if (Gop::setAppropriateFramebuffer(gop, PREFERRED_WIDTH, PREFERRED_HEIGHT, &fbInfo)) {
        //everything printed until now will be lost, but the cursor position won't be reset, so we reset it now 
        cout->ClearScreen(cout);
        cout->SetCursorPosition(cout, 0, 0);

        Log::print(L"Set graphics mode successfully.\r\n");
    } else {
        Log::print(L"WARN: Failed to set a graphics mode.\r\n");
    }


    KernelReader::KernelLoadError error;
    KernelReader::readKernel(handle, st, &error);


    bool isSuccessful;
    getMemoryMap(&isSuccessful);
    if (isSuccessful) {
        Log::print(L"Memory map retrieved.\r\n");
    }
    else {
        Log::print(L"Memory map failed.\r\n");
    }

    UINTN x;
    status = systemTable->BootServices->WaitForEvent(1, &systemTable->ConIn->WaitForKey, &x);
    if (EFI_ERROR(status)) {
        return status;
    }

    return 0;
}

static MemoryMap_t getMemoryMap(bool *isSuccessful) {
    *isSuccessful = false;

    if (systemTable == nullptr) {
        return INVALID_MEMORY_MAP;
    }

    EFI_PHYSICAL_ADDRESS memMapPtr = 0;
    EFI_STATUS status = systemTable->BootServices->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        1,
        &memMapPtr);
    if (EFI_ERROR(status) || memMapPtr == 0) {
        return INVALID_MEMORY_MAP;
    }

    UINTN mapSize = PAGE_SIZE;
    UINTN mapKey = 0;
    UINTN descriptorSize = 0;
    UINT32 descriptorVersion = 0;

    status = systemTable->BootServices->GetMemoryMap(
        &mapSize,
        reinterpret_cast<EFI_MEMORY_DESCRIPTOR *>(memMapPtr),
        &mapKey,
        &descriptorSize,
        &descriptorVersion);

    if (!EFI_ERROR(status)) {
        MemoryMap_t mem_map;
        mem_map.mem_map_size = mapSize;
        mem_map.mem_map_key = mapKey;
        mem_map.entries = reinterpret_cast<MemoryMapEntry_t *>(memMapPtr);
        mem_map.entry_size = descriptorSize;
        mem_map.entry_version = descriptorVersion;

        *isSuccessful = true;
        return mem_map;
    }

    //if we get an error, and it's not from insufficient memory, then we abort
    if (status != EFI_BUFFER_TOO_SMALL) {
        return INVALID_MEMORY_MAP;
    }

    //we free the one page that we allocated, so we can allocate the correct size
    status = systemTable->BootServices->FreePages(memMapPtr, 1);
    if (EFI_ERROR(status)) {
        return INVALID_MEMORY_MAP;
    }

    status = systemTable->BootServices->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        (mapSize + PAGE_SIZE - 1) / PAGE_SIZE,
        &memMapPtr);

    if (EFI_ERROR(status)) {
        return INVALID_MEMORY_MAP;
    }

    //try again
    status = systemTable->BootServices->GetMemoryMap(
        &mapSize,
        reinterpret_cast<EFI_MEMORY_DESCRIPTOR *>(memMapPtr),
        &mapKey,
        &descriptorSize,
        &descriptorVersion);

    //if it's still an error, abort
    if (EFI_ERROR(status)) {
        return INVALID_MEMORY_MAP;
    }

    MemoryMap_t memMap;
    memMap.mem_map_size = mapSize;
    memMap.mem_map_key = mapKey;
    memMap.entries = reinterpret_cast<MemoryMapEntry_t *>(memMapPtr);
    memMap.entry_size = descriptorSize;
    memMap.entry_version = descriptorVersion;

    *isSuccessful = true;
    return memMap;
}

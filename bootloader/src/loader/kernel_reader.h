#ifndef KERNEL_READER_H
#define KERNEL_READER_H

#include <efi.h>

namespace KernelReader {
    typedef struct KernelElfInfo {
        /**
         * The physical address where the kernel executable is loaded.
         */
        uint64_t PhysicalAddress;
        /**
         * The virtual address of the entry point (kernel_main).
         */
        uint64_t EntryPointVirtualAddress;
    } KernelElfInfo;

    static constexpr KernelElfInfo INVALID_KERNEL_ELF_INFO = {
        0,
        0,
    };

    typedef enum KernelLoadError {
        FileReadSuccess = 0,
        FileReadVolumeNotFound = 1,
        FileReadKernelNotFound = 2,
        FileReadAccessDenied = 3,
        FileReadVolumeCorrupted = 4,
        FileReadUnknownError = 255,
    } KernelLoadError;

    /**
     * Reads and loads the kernel binary into memory.
     * @param handle The UEFI image handle.
     * @param systemTable The EFI_SYSTEM_TABLE.
     * @param error The eventual error encountered while reading the file OR KernelLoadError::Success if no error occurred.
     */
    KernelElfInfo readKernel(EFI_HANDLE handle, const EFI_SYSTEM_TABLE *systemTable, KernelLoadError *error);
}

#endif //KERNEL_READER_H

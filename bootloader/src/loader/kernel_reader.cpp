#include <cstdint>
#include "elf/elf_loader.h"
#include "src/main.h"

#include "kernel_reader.h"

namespace KernelReader {
    typedef struct ReadFileInfo {
        void *Buffer;
        uint64_t BufferSize;
    } ReadFileInfo;

    constexpr ReadFileInfo INVALID_READ_FILE_INFO = {nullptr, 0};
    static EFI_BOOT_SERVICES *bs;

    static EFI_FILE_HANDLE getVolume(EFI_HANDLE handle, bool *isSuccessful);

    static ReadFileInfo readFile(EFI_FILE_HANDLE fileHandle, KernelLoadError *error);

    KernelElfInfo readKernel(EFI_HANDLE handle, const EFI_SYSTEM_TABLE *systemTable, KernelLoadError *error) {
        *error = FileReadUnknownError;
        bs = systemTable->BootServices;

        bool isSuccessful;
        EFI_FILE_HANDLE volumeHandle = getVolume(handle, &isSuccessful);
        if (!isSuccessful) {
            return INVALID_KERNEL_ELF_INFO;
        }

        EFI_FILE_HANDLE fileHandle;
        const EFI_STATUS status = volumeHandle->Open(
            volumeHandle,
            &fileHandle,
            L"\\boot\\kernel.elf",
            EFI_FILE_MODE_READ,
            EFI_FILE_READ_ONLY);

        if (EFI_ERROR(status)) {
            switch (status) {
                case EFI_NO_MEDIA:
                case EFI_MEDIA_CHANGED:
                case EFI_NOT_FOUND:
                    *error = FileReadKernelNotFound;
                    break;
                case EFI_VOLUME_CORRUPTED:
                    *error = FileReadVolumeCorrupted;
                    break;
                case EFI_WRITE_PROTECTED:
                case EFI_ACCESS_DENIED:
                    *error = FileReadAccessDenied;
                    break;
                default:
                    *error = FileReadUnknownError;
                    break;
            }
            return INVALID_KERNEL_ELF_INFO;
        }

        const ReadFileInfo kernelFileInfo = readFile(fileHandle, error);
        if (kernelFileInfo.BufferSize == 0) {
            return INVALID_KERNEL_ELF_INFO;
        }

        const auto elfLoader = Elf::ElfLoader(kernelFileInfo.Buffer, kernelFileInfo.BufferSize);
        Elf::ElfLoader::ElfError err = elfLoader.checkElf();

        if (err != Elf::ElfLoader::ElfError::NoError) {
            Log::print(L"Failed to load kernel: ELF header is corrupt.\r\n");
        }

        int numProgHeaders = 0;
        const Elf::Elf64_ProgHeader *progHeaders = elfLoader.getProgramHeaders(&numProgHeaders, &err);

        for (int i = 0; i < numProgHeaders; i++) {
            const Elf::ElfLoader::LoaderFunction fn = [](
                const int bufferSize,
                Elf::Elf64_Addr,
                Elf::Elf_SegmentFlags)
                -> void * {
                EFI_PHYSICAL_ADDRESS buffer = 0;
                const EFI_STATUS allocStatus = bs->AllocatePages(
                    AllocateAnyPages,
                    EfiLoaderData,
                    (bufferSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE,
                    &buffer);

                if (EFI_ERROR(allocStatus)) {
                    return nullptr;
                }

                return reinterpret_cast<void *>(buffer);
            };

            const Elf::ElfLoader::ElfError elfLoadError = elfLoader.loadExecutableProgram(&progHeaders[i], fn);
            if (
                elfLoadError != Elf::ElfLoader::ElfError::NoError
                && elfLoadError != Elf::ElfLoader::ElfError::ElfSectionNotLoadable
            ) {
                Log::print(L"Failed to load program header with index {?}! Boot failed\r\n");
                return INVALID_KERNEL_ELF_INFO;
            }
        }

        int numSectionHeaders = 0;
        const Elf::Elf64_SectionHeader *sectionHeaders = elfLoader.getSectionHeaders(&numSectionHeaders, &err);

        for (int i = 0; i < numSectionHeaders; i++) {
            const Elf::ElfLoader::LoaderFunction fn = [](
                const int bufferSize,
                Elf::Elf64_Addr,
                Elf::Elf_SegmentFlags)
                -> void * {
                EFI_PHYSICAL_ADDRESS buffer = 0;
                const EFI_STATUS allocStatus = bs->AllocatePages(
                    AllocateAnyPages,
                    EfiLoaderData,
                    (bufferSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE,
                    &buffer);

                if (EFI_ERROR(allocStatus)) {
                    return nullptr;
                }

                return reinterpret_cast<void *>(buffer);
            };

            const Elf::ElfLoader::ElfError elfLoadError = elfLoader.loadNoBitsSection(&sectionHeaders[i], fn);
            if (
                elfLoadError != Elf::ElfLoader::ElfError::NoError
                && elfLoadError != Elf::ElfLoader::ElfError::ElfSectionNotLoadable
            ) {
                Log::print(L"Failed to load program header with index {?}! Boot failed\r\n");
                return INVALID_KERNEL_ELF_INFO;
            }
        }

        volumeHandle->Close(fileHandle);
        return INVALID_KERNEL_ELF_INFO;
    }

    static EFI_FILE_HANDLE getVolume(EFI_HANDLE handle, bool *isSuccessful) {
        *isSuccessful = false;
        EFI_GUID loadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
        EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

        EFI_FILE_HANDLE volumeHandle = nullptr;
        EFI_LOADED_IMAGE *loadedImage = nullptr;
        EFI_FILE_IO_INTERFACE *storageVolume;

        EFI_STATUS status = bs->HandleProtocol(
            handle,
            &loadedImageGuid,
            reinterpret_cast<void **>(&loadedImage));
        if (EFI_ERROR(status)) {
            return volumeHandle;
        }

        status = bs->HandleProtocol(
            loadedImage->DeviceHandle,
            &fsGuid,
            reinterpret_cast<void **>(&storageVolume));
        if (EFI_ERROR(status)) {
            return volumeHandle;
        }

        status = storageVolume->OpenVolume(storageVolume, &volumeHandle);
        if (EFI_ERROR(status)) {
            return volumeHandle;
        }

        *isSuccessful = true;
        return volumeHandle;
    }

    static ReadFileInfo readFile(EFI_FILE_HANDLE fileHandle, KernelLoadError *error) {
        *error = FileReadUnknownError;
        EFI_GUID fileInfoGuid = EFI_FILE_INFO_ID;

        constexpr size_t DEFAULT_BUFFER_SIZE = 256U;
        UINTN bufferSize = DEFAULT_BUFFER_SIZE;
        void *infoBuffer;
        EFI_STATUS status = bs->AllocatePool(EfiLoaderData, bufferSize, &infoBuffer);
        if (EFI_ERROR(status)) {
            bs->FreePool(infoBuffer);
            return INVALID_READ_FILE_INFO;
        }

        // normally we will loop max. 2 times, as the second call will contain the correct buffer size (given by UEFI),
        //  but we want to make sure we don't enter an infinite loop
        int retries = 0;
        constexpr int NUM_RETRIES = 5;
        while (retries < NUM_RETRIES) {
            status = fileHandle->GetInfo(fileHandle, &fileInfoGuid, &bufferSize, infoBuffer);
            // if success, break the loop
            if (!EFI_ERROR(status)) {
                break;
            }

            // if it's a different error, we return early, as the function failed
            if (status != EFI_BUFFER_TOO_SMALL) {
                switch (status) {
                    case EFI_VOLUME_CORRUPTED:
                        *error = FileReadVolumeCorrupted;
                        break;
                    case EFI_NO_MEDIA:
                        *error = FileReadKernelNotFound;
                        break;
                    default:
                        *error = FileReadUnknownError;
                        break;
                }

                bs->FreePool(infoBuffer);
                return INVALID_READ_FILE_INFO;
            }

            // if the buffer was too small, we try again with the EFI-provided size
            bs->FreePool(infoBuffer);
            bs->AllocatePool(EfiLoaderData, bufferSize, &infoBuffer);
            retries++;
        }

        if (retries >= NUM_RETRIES) {
            bs->FreePool(infoBuffer);
            return INVALID_READ_FILE_INFO;
        }

        // the max file size is 256 MiB, can't possibly have such a large file
        constexpr uint64_t MAX_FILE_SIZE = 256LU * 1024LU * 1024LU;

        const auto *fileInfo = static_cast<EFI_FILE_INFO *>(infoBuffer);
        if (fileInfo->FileSize == 0 || fileInfo->FileSize > MAX_FILE_SIZE) {
            bs->FreePool(infoBuffer);
            return INVALID_READ_FILE_INFO;
        }

        uint64_t fileSize = fileInfo->FileSize;
        const uint64_t neededPages = (fileSize + (EFI_PAGE_SIZE - 1)) / EFI_PAGE_SIZE;
        if (neededPages > SIZE_MAX) {
            bs->FreePool(infoBuffer);
            return INVALID_READ_FILE_INFO;
        }

        // free the file info buffer, we don't need it anymore
        bs->FreePool(infoBuffer);

        EFI_PHYSICAL_ADDRESS fileBufferPhysAddress;
        bs->AllocatePages(
            AllocateAnyPages,
            EfiLoaderData,
            neededPages,
            &fileBufferPhysAddress);

        status = fileHandle->Read(fileHandle, &fileSize, reinterpret_cast<void *>(fileBufferPhysAddress));
        if (!EFI_ERROR(status)) {
            // success! return the necessary info
            const ReadFileInfo readFileInfo = {
                reinterpret_cast<void *>(fileBufferPhysAddress),
                fileSize,
            };

            return readFileInfo;
        }

        switch (status) {
            case EFI_VOLUME_CORRUPTED:
                *error = FileReadVolumeCorrupted;
                break;
            case EFI_NO_MEDIA:
                *error = FileReadKernelNotFound;
                break;
            default:
                *error = FileReadUnknownError;
                break;
        }

        bs->FreePool(infoBuffer);
        return INVALID_READ_FILE_INFO;
    }
}

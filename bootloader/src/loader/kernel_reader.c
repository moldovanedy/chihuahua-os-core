#include <stdbool.h>

#include "kernel_reader.h"

typedef struct ReadFileInfo {
    void* Buffer;
    uint64_t BufferSize;
} ReadFileInfo;

const ReadFileInfo INVALID_READ_FILE_INFO = { 0 };

static EFI_FILE_HANDLE getVolume(EFI_HANDLE handle, const EFI_SYSTEM_TABLE *systemTable, bool *isSuccessful);
static ReadFileInfo readFile(EFI_FILE_HANDLE fileHandle, const EFI_SYSTEM_TABLE *systemTable, KernelLoadError *error);

KernelElfInfo readKernel(EFI_HANDLE handle, const EFI_SYSTEM_TABLE *systemTable, KernelLoadError *error) {
    *error = FileReadUnknownError;
    bool isSuccessful;
    EFI_FILE_HANDLE volumeHandle = getVolume(handle, systemTable, &isSuccessful);
    if (!isSuccessful) {
        return INVALID_KERNEL_ELF_INFO;        
    }

    EFI_FILE_HANDLE fileHandle;
    EFI_STATUS status = volumeHandle->Open(
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

    const ReadFileInfo kernelFileInfo = readFile(fileHandle, systemTable, error);
    if (kernelFileInfo.BufferSize == 0) {
        return INVALID_KERNEL_ELF_INFO;
    }

    volumeHandle->Close(fileHandle);
    return INVALID_KERNEL_ELF_INFO;
}

static EFI_FILE_HANDLE getVolume(EFI_HANDLE handle, const EFI_SYSTEM_TABLE *systemTable, bool *isSuccessful) {
    *isSuccessful = false;
    EFI_GUID loadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFI_FILE_HANDLE volumeHandle = 0;
    EFI_LOADED_IMAGE *loadedImage = NULL;
    EFI_FILE_IO_INTERFACE *storageVolume;

    EFI_STATUS status = systemTable->BootServices->HandleProtocol(handle, &loadedImageGuid, (void**)&loadedImage);
    if (EFI_ERROR(status)) {
        return volumeHandle;
    }
    
    status = systemTable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &fsGuid, (void**)&storageVolume);
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

static ReadFileInfo readFile(EFI_FILE_HANDLE fileHandle, const EFI_SYSTEM_TABLE *systemTable, KernelLoadError *error) {
    *error = FileReadUnknownError;
    EFI_GUID fileInfoGuid = EFI_FILE_INFO_ID;

    const size_t DEFAULT_BUFFER_SIZE = 256U;
    UINTN bufferSize = DEFAULT_BUFFER_SIZE;
    void* infoBuffer;
    EFI_STATUS status = systemTable->BootServices->AllocatePool(EfiLoaderData, bufferSize, &infoBuffer);
    if (EFI_ERROR(status)) {
        goto CleanupAndExitError;
    }

    //normally we will loop max. 2 times, as the second call will contain the correct buffer size (given by UEFI),
    // but we want to make sure we don't enter an infinite loop
    int retries = 0;
    const int NUM_RETRIES = 5;
    while (retries < NUM_RETRIES) {
        status = fileHandle->GetInfo(fileHandle, &fileInfoGuid, &bufferSize, infoBuffer);
        //if success, break the loop
        if (!EFI_ERROR(status)) {
            break;
        }

        //if it's a different error, we return early, as the function failed
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

            goto CleanupAndExitError;
        }

        //if the buffer was too small, we try again with the EFI-provided size
        systemTable->BootServices->FreePool(infoBuffer);
        systemTable->BootServices->AllocatePool(EfiLoaderData, bufferSize, &infoBuffer);
        retries++;
    }

    if (retries >= NUM_RETRIES) {
        goto CleanupAndExitError;
    }

    //the max file size is 256 MiB, can't possibly have such a large file
    const uint64_t MAX_FILE_SIZE = 256LU * 1024LU * 1024LU;
    
    const EFI_FILE_INFO *fileInfo = infoBuffer;
    if (fileInfo->FileSize == 0 || fileInfo->FileSize > MAX_FILE_SIZE) {
        goto CleanupAndExitError;
    }

    uint64_t fileSize = fileInfo->FileSize;
    const uint64_t neededPages = (fileSize + (EFI_PAGE_SIZE - 1)) / EFI_PAGE_SIZE;
    if (neededPages > SIZE_MAX) {
        goto CleanupAndExitError;
    }

    //free the file info buffer, we don't need it anymore
    systemTable->BootServices->FreePool(infoBuffer);
    
    EFI_PHYSICAL_ADDRESS fileBufferPhysAddress;
    systemTable->BootServices->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        neededPages,
        &fileBufferPhysAddress);

    status = fileHandle->Read(fileHandle, &fileSize, (void*)fileBufferPhysAddress);
    if (!EFI_ERROR(status)) {
        //success! return the necessary info
        const ReadFileInfo readFileInfo = {
            .Buffer = (void*)fileBufferPhysAddress,
            .BufferSize = fileSize,
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

CleanupAndExitError:
    systemTable->BootServices->FreePool(infoBuffer);
    return INVALID_READ_FILE_INFO;
}

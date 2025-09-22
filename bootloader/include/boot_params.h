#ifndef BOOT_PARAMS_H
#define BOOT_PARAMS_H

#include <cstddef>

static constexpr int PAGE_SIZE = 4096;

/**
 * Describes a continuous region of memory in the MemoryMap. Fully compatible with EFI_MEMORY_DESCRIPTOR.
 */
struct MemoryMapEntry_t {
    /**
     * The type of memory that's used here.
     */
    uint32_t type;
    /**
     * The physical address start of the described region.
     */
    uint64_t physical_start;
    /**
     * The virtual address start of the described region.
     */
    uint64_t virtual_start;
    /**
     * The number of 4 KiB pages that this region describes.
     */
    uint64_t page_count;
    /**
     * Some flags that further describe the region.
     */
    uint64_t flags;
};

/**
 * Describes the memory map for the RAM. Fully compatible with the parameters of UEFI's EFI_BOOT_SERVICES.GetMemoryMap().
 */
struct MemoryMap_t {
    /**
     * The size (in bytes) of the entries array.
     */
    size_t mem_map_size;
    /**
     * An array of memory map entries. This is the important data. The array size is mem_map_size / entry_size.
     */
    MemoryMapEntry_t *entries;
    /**
     * The UEFI key for this memory map (determines if the map is valid or not).
     */
    size_t mem_map_key;
    /**
     * The size (in bytes) of a single entry.
     */
    size_t entry_size;
    /**
     * The UEFI spec version for the entry.
     */
    uint32_t entry_version;
};

/**
 * Represents an invalid memory map. It is returned when something failed while getting the memory map. All values are
 * 0 and the entries array is a null pointer.
 */
static constexpr MemoryMap_t INVALID_MEMORY_MAP = {
    0,
    nullptr,
    0,
    0,
    0
};

struct FramebufferInfo_t {
    uint32_t Width;
    uint32_t Height;
    uint32_t BytesPerRow;
    bool isRgb;
};

static constexpr FramebufferInfo_t INVALID_FRAMEBUFFER_INFO = {
    0,
    0,
    0,
    false
};

#endif //BOOT_PARAMS_H

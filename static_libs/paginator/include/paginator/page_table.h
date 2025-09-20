#ifndef PAGINATOR_PAGE_TABLE_H
#define PAGINATOR_PAGE_TABLE_H

#include <cstdint>

namespace Paginator {
    struct PageTable_t {
        uint64_t entries[512];
    };

    enum class PageFlags {
        /**
         * If set, it means the page is present in RAM (opposed to swapped to disk).
         */
        Present = 1,
        /**
         * If set, both user-space and kernel-space can access it.
         */
        UserModeAccessible = 1 << 1,
        /**
         * If set, the data in this page can be executed.
         */
        ExecuteBit = 1 << 2,
        /**
         * If set, data can be written to this page.
         */
        WriteBit = 1 << 3,
        /**
         * If set, the data in this page can be read.
         */
        ReadBit = 1 << 4,
        /**
         * If set, this page is a "huge" page, generally 1 or 2 MiB, opposed to the usual 4 KiB.
         */
        IsHugePage = 1 << 15,
    };

    inline PageFlags operator|(PageFlags a, PageFlags b)
    {
        return static_cast<PageFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline PageFlags operator&(PageFlags a, PageFlags b)
    {
        return static_cast<PageFlags>(static_cast<int>(a) & static_cast<int>(b));
    }

    enum class PageMapError {
        NoError = 0,
        /**
         * An entry already exists and a forceful write was not allowed.
         */
        EntryExists = 1,
        InvalidVirtAddress = 2,
        /**
         * The allocator callback returned 0 as the physical address. 
         */
        AllocFailed = 3,
        UnknownError = INT32_MAX,
    };
    

    class PageTableRootController {
        PageTable_t *rootPageTableAddress;

    public:
        typedef std::size_t (*PageFrameAllocator)();
        bool pagingDisabledNow;

        /**
         * Constructor.
         * @param rootPageTable The root page table of the paging structure. 
         * @param allocator The physical frame allocator. Must return usable physical addresses for the page tables
         * themselves.
         * @param pagingDisabledNow True only when the paging structure is not yet running (or at least it's not the one
         * ChihuahuaOS created). This should be false all the time, except in the bootloader.
         */
        PageTableRootController(
            PageTable_t *rootPageTable,
            PageFrameAllocator allocator,
            bool pagingDisabledNow = false);

        /**
         * Maps a virtual address to the given physical address if possible.
         * @param virtAddress The virtual address to map.
         * @param physAddress The destination physical address.
         * @param flags The page flags for the current map. Will affect the way this mapping works. See PageFlags for
         * more info.
         * @param forceWrite If true, will write the new mapping even if another one already exists (i.e. if the
         * virtual address is already mapped to another physical address).
         * @return True if the mapping succeeded, false otherwise.
         */
        static PageMapError mapPage(
            std::size_t virtAddress,
            std::size_t physAddress,
            PageFlags flags,
            bool forceWrite = true);

        /**
         * Unmaps the given virtual address from the page tables.
         * @param virtAddress The virtual address to unmap.
         */
        static void unmapPage(std::size_t virtAddress);

        /**
         * A simple wrapper around mapPage that maps the virtual address to the same physical address.
         * @param address The identity-mapped address.
         * @param flags The page flags for the current map. Will affect the way this mapping works. See PageFlags for
         * more info.
         * @param forceWrite If true, will write the new mapping even if another one already exists (i.e. if the
         * virtual address is already mapped to another physical address).
         * @return True if the mapping succeeded, false otherwise.
         */
        static PageMapError identityMapPage(std::size_t address, PageFlags flags, bool forceWrite = true);

        /**
         * Translates a virtual address to a physical one if a mapping for the virtual address exists.
         * @param virtAddress The virtual address for which you want the physical address.
         * @return The physical address corresponding to the given virtual address or 0 if a mapping does not exist.
         */
        static std::size_t translateVirtToPhys(std::size_t virtAddress);

        /**
         * Activates (or applies) the given root page table, so all the paging rules set are immediately effective.
         * @return True if the operation succeeded, false otherwise.
         */
        static bool activateRootPageTable();

    private:
        PageFrameAllocator allocator;
    };

} //namespace Paginator

#endif //PAGINATOR_PAGE_TABLE_H
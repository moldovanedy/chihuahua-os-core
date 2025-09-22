#ifndef PAGINATOR_ARCH_X86_64_PAGING_CONTROLLER_H
#define PAGINATOR_ARCH_X86_64_PAGING_CONTROLLER_H

#include <cstdint>

#include "paginator/page_table.h"

namespace Paginator::X86_64 {
    enum class X86_64PageFlags : uint64_t {
        Present = 1,
        /**
         * If set, the page is read/write, otherwise it's read-only.
         */
        WriteEnable = 1 << 1,
        /**
         * If set, this can be accessed by both user-mode and kernel-mode, otherwise just kernel-mode accessible.
         */
        UserModeAccessible = 1 << 2,
        WriteThrough = 1 << 3,
        CacheDisable = 1 << 4,
        Accessed = 1 << 5,
        /**
         * CPU writes this bit on write to the mapped frame.
         */
        Dirty = 1 << 6,
        /**
         * If set, it means the page is a "huge" page of 2 MiB (on level 2) or 1 GiB (on level 3), otherwise it's
         * a normal, 4 KiB page.
         */
        HugePage = 1 << 7,
        /**
         * If set, it means the entry is available in all address spaces, so it isn't flushed by the TLB on an
         * address space switch.
         */
        Global = 1 << 8,
        /**
         * If set, this page is not executable, otherwise it can host executable code.
         */
        ExecuteDisable = 1ULL << 63ULL,
    };

    inline X86_64PageFlags operator|(X86_64PageFlags a, X86_64PageFlags b)
    {
        return static_cast<X86_64PageFlags>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
    }

    inline X86_64PageFlags operator&(X86_64PageFlags a, X86_64PageFlags b)
    {
        return static_cast<X86_64PageFlags>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
    }

    inline X86_64PageFlags operator~(X86_64PageFlags a) {
        return static_cast<X86_64PageFlags>(~static_cast<uint64_t>(a));
    }

    PageMapError mapPage(
        PageTable_t *rootPageTable,
        PageTableRootController::PageFrameAllocator allocator,
        std::size_t virtAddress,
        std::size_t physAddress,
        PageFlags flags,
        bool forceWrite,
        bool pagingDisabledNow = false);

    void unmapPage(PageTable_t *rootPageTable, std::size_t virtAddress, bool pagingDisabledNow = false);

    uint64_t translateVirtToPhys(const PageTable_t *rootPageTable, std::size_t virtAddress,
                                 bool pagingDisabledNow = false);

    bool activateRootPageTable(PageTable_t *rootPageTable, bool pagingDisabledNow = false);
} //namespace Paginator::X86_64
#endif //PAGINATOR_ARCH_X86_64_PAGING_CONTROLLER_H
#include "paginator/page_table.h"

#if __x86_64__
#include "arch/x86_64/x86_64_paging_controller.h"
#endif

namespace Paginator {
    PageTableRootController::PageTableRootController(
        PageTable_t *rootPageTable,
        const PageFrameAllocator allocator,
        const bool pagingDisabledNow)
        :   rootPageTableAddress(rootPageTable),
            pagingDisabledNow(pagingDisabledNow),
            allocator(allocator)
    {
    }

    PageMapError PageTableRootController::mapPage(
        const std::size_t virtAddress,
        const std::size_t physAddress,
        const PageFlags flags,
        const bool forceWrite) const {
#if __x86_64__
        return X86_64::mapPage(
            this->rootPageTableAddress,
            this->allocator,
            virtAddress,
            physAddress,
            flags,
            forceWrite,
            this->pagingDisabledNow);
#endif
    }

    void PageTableRootController::unmapPage(const std::size_t virtAddress) const {
#if __x86_64__
        X86_64::unmapPage(this->rootPageTableAddress, virtAddress, this->pagingDisabledNow);
#endif
    }

    PageMapError PageTableRootController::identityMapPage(
        const std::size_t address,
        const PageFlags flags,
        const bool forceWrite) const {
#if __x86_64__
        return X86_64::mapPage(
            this->rootPageTableAddress,
            this->allocator,
            address,
            address,
            flags,
            forceWrite,
            this->pagingDisabledNow);
#endif
    }

    std::size_t PageTableRootController::translateVirtToPhys(const std::size_t virtAddress) const {
#if __x86_64__
        return X86_64::translateVirtToPhys(this->rootPageTableAddress, virtAddress, this->pagingDisabledNow);
#endif
    }

    bool PageTableRootController::activateRootPageTable() const {
#if __x86_64__
        return X86_64::activateRootPageTable(this->rootPageTableAddress, this->pagingDisabledNow);
#endif
    }
} //namespace Paginator

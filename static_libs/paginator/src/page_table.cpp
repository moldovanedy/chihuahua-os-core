#if __x86_64__
#include "arch/x86_64/x86_64_paging_controller.h"
#endif

#include "paginator/page_table.h"

namespace Paginator {
#if __x86_64__
    std::optional<X86_64PagingController> _pagingController = std::nullopt;
#endif
    
    PageTableRootController::PageTableRootController(
        PageTable_t *rootPageTable,
        const PageFrameAllocator allocator,
        const bool pagingDisabledNow)
        :   rootPageTableAddress(rootPageTable),
            pagingDisabledNow(pagingDisabledNow),
            allocator(allocator)
    {
#if __x86_64__
        _pagingController = X86_64PagingController(rootPageTable, allocator);
#endif
    }

    PageMapError PageTableRootController::mapPage(
        const std::size_t virtAddress,
        const std::size_t physAddress,
        const PageFlags flags,
        const bool forceWrite)
    {
        if (!_pagingController.has_value()) {
            return PageMapError::UnknownError;
        }
        
        return _pagingController.value().mapPage(virtAddress, physAddress, flags, forceWrite);
    }

    void PageTableRootController::unmapPage(const std::size_t virtAddress) {
        if (!_pagingController.has_value()) {
            return;
        }
        
        return _pagingController.value().unmapPage(virtAddress);
    }

    
    PageMapError PageTableRootController::identityMapPage(
        const std::size_t address,
        const PageFlags flags,
        const bool forceWrite)
    {
        if (!_pagingController.has_value()) {
            return PageMapError::UnknownError;
        }
        
        return _pagingController.value().mapPage(address, address, flags, forceWrite);
    }

    std::size_t PageTableRootController::translateVirtToPhys(const std::size_t virtAddress) {
        if (!_pagingController.has_value()) {
            return false;
        }
        
        return _pagingController.value().translateVirtToPhys(virtAddress);
    }

    bool PageTableRootController::activateRootPageTable() {
        if (!_pagingController.has_value()) {
            return false;
        }
        
        return _pagingController.value().activateRootPageTable();
    }

} //namespace Paginator

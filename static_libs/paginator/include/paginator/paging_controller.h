#ifndef PAGINATOR_PAGING_CONTROLLER_H
#define PAGINATOR_PAGING_CONTROLLER_H

#include <cstddef>
#include <optional>

#include "page_table.h"

namespace Paginator {
    class IPagingController {
    public:
        std::size_t pageSize;
        std::optional<std::size_t> hugePageSize;
        PageTable_t *rootPageTable;
        PageTableRootController::PageFrameAllocator allocator;
        bool pagingDisabledNow;

        IPagingController(
            PageTable_t *rootPageTable,
            std::size_t pageSize,
            std::optional<std::size_t> hugePageSize,
            PageTableRootController::PageFrameAllocator allocator,
            bool pagingDisabledNow = false);
        virtual ~IPagingController();

        [[nodiscard]]
        virtual PageMapError mapPage(
            std::size_t virtAddress,
            std::size_t physAddress,
            PageFlags flags,
            bool forceWrite) const = 0;

        virtual void unmapPage(std::size_t virtAddress) const = 0;
        
        [[nodiscard]]
        virtual std::size_t translateVirtToPhys(std::size_t virtAddress) const = 0;

        [[nodiscard]]
        virtual bool activateRootPageTable() const = 0;
    };
} //namespace Paginator

#endif //PAGINATOR_PAGING_CONTROLLER_H
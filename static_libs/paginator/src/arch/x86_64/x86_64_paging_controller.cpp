#include <chihuahua_essentials/mem_essentials.h>

#include "x86_64_paging_controller.h"

#include <bits/move.h>

namespace Paginator::X86_64 {
    constexpr uint64_t INDEX_MASK = 0x1FF;
    constexpr uint64_t PAGE_OFFSET_MASK = 0x3FF;
    constexpr uint64_t PHYSICAL_ADDRESS_MASK = 0x000FFFFFFFFFF000;

    constexpr uint64_t PAGE_TABLE_SIZE = 4096;
    constexpr uint64_t PAGE_SIZE = 4096;
    constexpr uint64_t HUGE_PAGE_SIZE = 1024ULL * 1024ULL * 2ULL;

    constexpr uint64_t SIGN_EXTENSION = 0xFFFFULL << 48ULL;
    constexpr uint64_t RECURSIVE_INDEX = 0x01;
    
    constexpr uint64_t P4_SHIFT = 39;
    constexpr uint64_t P3_SHIFT = 30;
    constexpr uint64_t P2_SHIFT = 21;
    constexpr uint64_t P1_SHIFT = 12;

    uint64_t constructTableEntry(uint64_t physicalAddress, PageFlags flags);

    /**
     * Calls "invlpg" on the given virtual address to remove any TLB caches associated with the given address.
     * @param virtAddress The virtual address you want to remove from the TLB cache
     */
    void invalidatePage(uint64_t virtAddress);


#pragma region Public implementation

    PageMapError mapPage(
        PageTable_t *rootPageTable,
        const PageTableRootController::PageFrameAllocator allocator,
        const std::size_t virtAddress,
        const std::size_t physAddress,
        const PageFlags flags,
        const bool forceWrite,
        const bool pagingDisabledNow)
    {
        const uint64_t l4Idx = (virtAddress >> P4_SHIFT) & INDEX_MASK;
        const uint64_t l3Idx = (virtAddress >> P3_SHIFT) & INDEX_MASK;
        const uint64_t l2Idx = (virtAddress >> P2_SHIFT) & INDEX_MASK;
        const uint64_t l1Idx = (virtAddress >> P1_SHIFT) & INDEX_MASK;

        const PageFlags pageTableFlags =
            PageFlags::Present
        | PageFlags::ReadBit
        | PageFlags::WriteBit
        | PageFlags::UserModeAccessible;

        if (l4Idx > INDEX_MASK || l3Idx > INDEX_MASK || l2Idx > INDEX_MASK || l1Idx > INDEX_MASK) {
            return PageMapError::InvalidVirtAddress;
        }

        //the address must be canonical: all upper bits need to be all 0 or all 1
        const uint64_t signExtension = virtAddress >> 48;
        if (signExtension != 0 && signExtension != 0xFFFF) {
            return PageMapError::InvalidVirtAddress;
        }

        //if a mapping already exists, and we can't overwrite, return early 
        if (!forceWrite && translateVirtToPhys(rootPageTable, virtAddress, pagingDisabledNow) != 0) {
            return PageMapError::EntryExists;
        }


        uint64_t entryForL3 = rootPageTable->entries[l4Idx];
        if (entryForL3 == 0) {
            const uint64_t physAddr = allocator();
            if (physAddr == 0) {
                return PageMapError::AllocFailed;
            }

            if (pagingDisabledNow) {
                memset(reinterpret_cast<void *>(physAddr), 0, PAGE_TABLE_SIZE);
            } else {
                //TODO
            }

            entryForL3 = constructTableEntry(physAddr, pageTableFlags);
            rootPageTable->entries[l4Idx] = entryForL3;

            //recursive mapping
            PageTable_t *l3Table;
            if (pagingDisabledNow) {
                l3Table = reinterpret_cast<PageTable_t *>(entryForL3 & PHYSICAL_ADDRESS_MASK);
            } else {
                const uint64_t recursiveAddress =
                        SIGN_EXTENSION
                        | RECURSIVE_INDEX << P4_SHIFT
                        | RECURSIVE_INDEX << P3_SHIFT
                        | RECURSIVE_INDEX << P2_SHIFT
                        | l4Idx << P1_SHIFT;
                l3Table = reinterpret_cast<PageTable_t *>(recursiveAddress);
            }

            l3Table->entries[RECURSIVE_INDEX] = entryForL3;
        }


        PageTable_t *l3Table;
        if (pagingDisabledNow) {
            l3Table = reinterpret_cast<PageTable_t *>(entryForL3 & PHYSICAL_ADDRESS_MASK);
        } else {
            const uint64_t recursiveAddress =
                    SIGN_EXTENSION
                    | RECURSIVE_INDEX << P4_SHIFT
                    | RECURSIVE_INDEX << P3_SHIFT
                    | RECURSIVE_INDEX << P2_SHIFT
                    | l4Idx << P1_SHIFT;
            l3Table = reinterpret_cast<PageTable_t *>(recursiveAddress);
        }

        uint64_t entryForL2 = l3Table->entries[l3Idx];
        if (entryForL2 == 0) {
            const uint64_t physAddr = allocator();
            if (physAddr == 0) {
                return PageMapError::AllocFailed;
            }

            if (pagingDisabledNow) {
                memset(reinterpret_cast<void *>(physAddr), 0, PAGE_TABLE_SIZE);
            } else {
                //TODO
            }

            entryForL2 = constructTableEntry(physAddr, pageTableFlags);
            rootPageTable->entries[l3Idx] = entryForL2;

            PageTable_t *l2Table;
            if (pagingDisabledNow) {
                l2Table = reinterpret_cast<PageTable_t *>(entryForL2 & PHYSICAL_ADDRESS_MASK);
            } else {
                const uint64_t recursiveAddress =
                        SIGN_EXTENSION
                        | RECURSIVE_INDEX << P4_SHIFT
                        | RECURSIVE_INDEX << P3_SHIFT
                        | l4Idx << P2_SHIFT
                        | l3Idx << P1_SHIFT;
                l2Table = reinterpret_cast<PageTable_t *>(recursiveAddress);
            }

            l2Table->entries[RECURSIVE_INDEX] = entryForL2;
        }


        PageTable_t *l2Table;
        if (pagingDisabledNow) {
            l2Table = reinterpret_cast<PageTable_t *>(entryForL2 & PHYSICAL_ADDRESS_MASK);
        } else {
            const uint64_t recursiveAddress =
                    SIGN_EXTENSION
                    | RECURSIVE_INDEX << P4_SHIFT
                    | RECURSIVE_INDEX << P3_SHIFT
                    | l4Idx << P2_SHIFT
                    | l3Idx << P1_SHIFT;
            l2Table = reinterpret_cast<PageTable_t *>(recursiveAddress);
        }

        uint64_t entryForL1 = l2Table->entries[l2Idx];
        if (entryForL1 == 0) {
            const uint64_t physAddr = allocator();
            if (physAddr == 0) {
                return PageMapError::AllocFailed;
            }

            if (pagingDisabledNow) {
                memset(reinterpret_cast<void *>(physAddr), 0, PAGE_TABLE_SIZE);
            } else {
                //TODO
            }

            entryForL1 = constructTableEntry(physAddr, pageTableFlags);
            rootPageTable->entries[l2Idx] = entryForL1;

            PageTable_t *l1Table;
            if (pagingDisabledNow) {
                l1Table = reinterpret_cast<PageTable_t *>(entryForL1 & PHYSICAL_ADDRESS_MASK);
            } else {
                const uint64_t recursiveAddress =
                        SIGN_EXTENSION
                        | RECURSIVE_INDEX << P4_SHIFT
                        | l4Idx << P3_SHIFT
                        | l3Idx << P2_SHIFT
                        | l2Idx << P1_SHIFT;
                l1Table = reinterpret_cast<PageTable_t *>(recursiveAddress);
            }

            l1Table->entries[RECURSIVE_INDEX] = entryForL1;
        }


        //the actual L1 entry
        PageTable_t *l1Table;
        if (pagingDisabledNow) {
            l1Table = reinterpret_cast<PageTable_t *>(entryForL1 & PHYSICAL_ADDRESS_MASK);
        } else {
            const uint64_t recursiveAddress =
                    SIGN_EXTENSION
                    | RECURSIVE_INDEX << P4_SHIFT
                    | l4Idx << P3_SHIFT
                    | l3Idx << P2_SHIFT
                    | l2Idx << P1_SHIFT;
            l1Table = reinterpret_cast<PageTable_t *>(recursiveAddress);
        }

        const uint64_t existingPhysAddress = l1Table->entries[l1Idx];
        if (existingPhysAddress != 0) {
            if (forceWrite) {
                invalidatePage(virtAddress);          
            }
            else {
                return PageMapError::EntryExists;
            }
        }
        
        l1Table->entries[l1Idx] = constructTableEntry(physAddress, flags);
        return PageMapError::NoError;
    }

    void unmapPage(PageTable_t *rootPageTable, const std::size_t virtAddress, const bool pagingDisabledNow) {
        //TODO
    }

    uint64_t translateVirtToPhys(
        const PageTable_t *rootPageTable,
        const std::size_t virtAddress,
        bool pagingDisabledNow) {
        const uint64_t l4Idx = (virtAddress >> P4_SHIFT) & INDEX_MASK;
        const uint64_t l3Idx = (virtAddress >> P3_SHIFT) & INDEX_MASK;
        const uint64_t l2Idx = (virtAddress >> P2_SHIFT) & INDEX_MASK;
        const uint64_t l1Idx = (virtAddress >> P1_SHIFT) & INDEX_MASK;

        if (l4Idx > INDEX_MASK || l3Idx > INDEX_MASK || l2Idx > INDEX_MASK || l1Idx > INDEX_MASK) {
            return 0;
        }

        const uint64_t l3PhysAddr = rootPageTable->entries[l4Idx] & PHYSICAL_ADDRESS_MASK;
        if (l3PhysAddr == 0) {
            return 0;
        }

        const uint64_t l2PhysAddr = reinterpret_cast<PageTable_t *>(l3PhysAddr)->entries[l3Idx] & PHYSICAL_ADDRESS_MASK;
        if (l2PhysAddr == 0) {
            return 0;
        }

        const uint64_t l1PhysAddr = reinterpret_cast<PageTable_t *>(l2PhysAddr)->entries[l2Idx] & PHYSICAL_ADDRESS_MASK;
        if (l1PhysAddr == 0) {
            return 0;
        }

        return reinterpret_cast<PageTable_t *>(l1PhysAddr)->entries[l1Idx] & PHYSICAL_ADDRESS_MASK;
    }

    [[nodiscard]]
    bool activateRootPageTable(PageTable_t *rootPageTable, bool pagingDisabledNow) {
        uint64_t physAddr;
        if (pagingDisabledNow) {
            physAddr = reinterpret_cast<uintptr_t>(rootPageTable);
        } else {
            uint64_t recursiveAddr =
                    SIGN_EXTENSION
                    | (RECURSIVE_INDEX << P4_SHIFT)
                    | (RECURSIVE_INDEX << P3_SHIFT)
                    | (RECURSIVE_INDEX << P2_SHIFT)
                    | (RECURSIVE_INDEX << P1_SHIFT);

            physAddr = reinterpret_cast<PageTable_t *>(recursiveAddr)->entries[RECURSIVE_INDEX] & PHYSICAL_ADDRESS_MASK;
        }

        asm ("mov %0, %%cr3"::"r"(physAddr) : "memory");
        return true;
    }

#pragma endregion // Public implementation


    uint64_t constructTableEntry(const uint64_t physicalAddress, const PageFlags flags) {
        auto x86_64PageFlags = X86_64PageFlags::None;
        
        if ((flags & PageFlags::Present) != PageFlags::None) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::Present;
        }

        if ((flags & PageFlags::UserModeAccessible) != PageFlags::None) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::UserModeAccessible;
        }

        if ((flags & PageFlags::WriteBit) != PageFlags::None) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::WriteEnable;
        }

        if ((flags & PageFlags::IsHugePage) != PageFlags::None) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::HugePage;
        }

        if ((flags & PageFlags::ExecuteBit) == PageFlags::None) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::ExecuteDisable;
        }

        const uint64_t data = static_cast<uint64_t>(x86_64PageFlags) | physicalAddress;
        return data;
    }

    void invalidatePage(const uint64_t virtAddress) {
        // uintptr_t va = virtAddress;
        // int i = 1;
        //asm volatile("invlpg [%0]" :: "r"(va+i));
    }

    // uint64_t level_4_table_addr =
    //     SIGN_EXTENSION
    // | (RECURSIVE_INDEX << P4_SHIFT)
    // | (RECURSIVE_INDEX << P3_SHIFT)
    // | (RECURSIVE_INDEX << P2_SHIFT)
    // | (RECURSIVE_INDEX << P1_SHIFT);
} //namespace Paginator::X86_64

#include <chihuahua_essentials/mem_essentials.h>
#include <chihuahua_essentials/binary_utils.h>

#include "x86_64_paging_controller.h"

namespace Paginator::X86_64 {
    constexpr uint64_t INDEX_MASK = 0x1FF;
    constexpr uint64_t PAGE_OFFSET_MASK = 0x3FF;

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

    /**
     * Gets only the physical address of a page entry (uint64_t).
     * @param entry The instance of a page entry (uint64_t).
     */
#define GET_ADDR_FROM_ENTRY(entry) (((entry) >> 12ULL) & 40ULL)

    /**
     * The actual physical address.
     */
#define SET_PHYSICAL_ADDRESS(entry, val) (entry |= ((val) << 12ULL))


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

            rootPageTable->entries[l4Idx] =
                entryForL3 =
                    constructTableEntry(physAddr, PageFlags::Present | PageFlags::WriteBit);

            //recursive mapping
            PageTable_t *l3Table;
            if (pagingDisabledNow) {
                l3Table = reinterpret_cast<PageTable_t *>(GET_ADDR_FROM_ENTRY(entryForL3));
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
            l3Table = reinterpret_cast<PageTable_t *>(GET_ADDR_FROM_ENTRY(entryForL3));
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

            rootPageTable->entries[l3Idx] =
                entryForL2 =
                    constructTableEntry(physAddr, PageFlags::Present | PageFlags::WriteBit);

            PageTable_t *l2Table;
            if (pagingDisabledNow) {
                l2Table = reinterpret_cast<PageTable_t *>(GET_ADDR_FROM_ENTRY(entryForL2));
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
            l2Table = reinterpret_cast<PageTable_t *>(GET_ADDR_FROM_ENTRY(entryForL2));
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

            rootPageTable->entries[l2Idx] =
                entryForL1 =
                    constructTableEntry(physAddr, PageFlags::Present | PageFlags::WriteBit);

            PageTable_t *l1Table;
            if (pagingDisabledNow) {
                l1Table = reinterpret_cast<PageTable_t *>(GET_ADDR_FROM_ENTRY(entryForL1));
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
            l1Table = reinterpret_cast<PageTable_t *>(GET_ADDR_FROM_ENTRY(entryForL1));
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
        bool) {
        const uint64_t l4Idx = (virtAddress >> P4_SHIFT) & INDEX_MASK;
        const uint64_t l3Idx = (virtAddress >> P3_SHIFT) & INDEX_MASK;
        const uint64_t l2Idx = (virtAddress >> P2_SHIFT) & INDEX_MASK;
        const uint64_t l1Idx = (virtAddress >> P1_SHIFT) & INDEX_MASK;

        if (l4Idx > INDEX_MASK || l3Idx > INDEX_MASK || l2Idx > INDEX_MASK || l1Idx > INDEX_MASK) {
            return 0;
        }

        const uint64_t l3PhysAddr = GET_ADDR_FROM_ENTRY(rootPageTable->entries[l4Idx]);
        if (l3PhysAddr == 0) {
            return 0;
        }

        const uint64_t l2PhysAddr = GET_ADDR_FROM_ENTRY(reinterpret_cast<PageTable_t *>(l3PhysAddr)->entries[l3Idx]);
        if (l2PhysAddr == 0) {
            return 0;
        }

        const uint64_t l1PhysAddr = GET_ADDR_FROM_ENTRY(reinterpret_cast<PageTable_t *>(l2PhysAddr)->entries[l2Idx]);
        if (l1PhysAddr == 0) {
            return 0;
        }

        return GET_ADDR_FROM_ENTRY(reinterpret_cast<PageTable_t *>(l1PhysAddr)->entries[l1Idx]);
    }

    [[nodiscard]]
    bool activateRootPageTable(PageTable_t *rootPageTable, bool pagingDisabledNow) {
        //TODO

        uint64_t physAddr;
        if (pagingDisabledNow) {
            physAddr = reinterpret_cast<uint64_t>(rootPageTable);
        } else {
            uint64_t recursiveAddr =
                    SIGN_EXTENSION
                    | (RECURSIVE_INDEX << P4_SHIFT)
                    | (RECURSIVE_INDEX << P3_SHIFT)
                    | (RECURSIVE_INDEX << P2_SHIFT)
                    | (RECURSIVE_INDEX << P1_SHIFT);

            physAddr = reinterpret_cast<PageTable_t *>(recursiveAddr)->entries[RECURSIVE_INDEX];
        }

        uint64_t cr3Flags;
        asm ("mov %0, cr3;" : "=r"(cr3Flags));

        physAddr |= cr3Flags & (PAGE_SIZE - 1);
        asm ("mov cr3, %0;"::"r"(physAddr));
        return false;
    }

#pragma endregion // Public implementation


    uint64_t constructTableEntry(const uint64_t physicalAddress, const PageFlags flags) {
        uint64_t data = 0;
        auto x86_64PageFlags = static_cast<X86_64PageFlags>(0);
        
        if ((flags & PageFlags::Present) == PageFlags::Present) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::Present;
        }

        if ((flags & PageFlags::UserModeAccessible) == PageFlags::UserModeAccessible) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::UserModeAccessible;
        }

        if ((flags & PageFlags::WriteBit) == PageFlags::WriteBit) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::WriteEnable;
        }

        if ((flags & PageFlags::IsHugePage) == PageFlags::IsHugePage) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::HugePage;
        }

        if ((flags & PageFlags::ExecuteBit) != PageFlags::ExecuteBit) {
            x86_64PageFlags = x86_64PageFlags | X86_64PageFlags::ExecuteDisable;
        }
        
        ESSENTIALS_SET_BITS_ADDITIVE(data, static_cast<uint64_t>(x86_64PageFlags), 0);
        SET_PHYSICAL_ADDRESS(data, physicalAddress);
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

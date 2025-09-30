#ifndef INTERRUPT_MANAGER_ARCH_X86_64_IDT_H
#define INTERRUPT_MANAGER_ARCH_X86_64_IDT_H

#include <cstdint>

namespace InterruptManager::Arch::X86_64 {
    struct InterruptStackFrame
    {
        uint64_t InstructionPointer = 0;
        uint64_t CodeSegment = 0;
        uint64_t RFlags = 0;
        uint64_t StackPointer = 0;
        uint64_t StackSegment = 0;
    };
    
    class InterruptEntry {
    public:
        /**
         * Address bits 0-15.
         */
        uint16_t Address1 = 0;
        /**
         * Segment selector (GDT).
         */
        uint16_t SegmentSelector = 0;
        /**
         * Offset of Interrupt Stack Table (IST) determined by the least significant 3 bits. The rest are reserved
         * and should be 0.
         */
        uint8_t InterruptStackTable = 0;
        /**
         * Specified gate type (bits 0-3), a reserved bit (4) that must be 0, privilege table (DPL, bits 5-6),
         * and the present bit (bit 7).
         */
        uint8_t Attributes = 0;
        /**
         * Address bits 48-63.
         */
        uint16_t Address2 = 0;
        /**
         * Address bits 64-95.
         */
        uint32_t Address3 = 0;
        /**
         * Reserved; must be 0.
         */
        uint32_t Zero = 0;

        static InterruptEntry MISSING_INTERRUPT_HANDLER;
    };
    
    class alignas(16) IDT {
        InterruptEntry gates[32] {InterruptEntry::MISSING_INTERRUPT_HANDLER};
        
    public:
        IDT();
        ~IDT();

        IDT& operator=(const IDT& value) = default;
    };
}

#endif //INTERRUPT_MANAGER_ARCH_X86_64_IDT_H
#ifndef INTERRUPT_MANAGER_ARCH_X86_64_GDT_H
#define INTERRUPT_MANAGER_ARCH_X86_64_GDT_H
#include <cstdint>

namespace InterruptManager::Arch::X86_64 {
    class GdtSegment {
    public:
        uint16_t Limit1 = 0;
        uint16_t Base1 = 0;
        uint8_t Base2 = 0;
        uint8_t AccessByte = 0;
        uint8_t Limit2AndFlags = 0;
        uint8_t Base3 = 0;

        explicit GdtSegment(const uint8_t accessByte)
        {
            this->AccessByte = accessByte;
        }

        void setBase(uint32_t base);
        void setLimit(uint32_t limit);
        void setFlags(uint8_t flags);
    };
    
    class GdtSysSegment {
    public:
        uint16_t Limit1 = 0;
        uint16_t Base1 = 0;
        uint8_t Base2 = 0;
        uint8_t AccessByte = 0;
        uint8_t Limit2AndFlags = 0;
        uint8_t Base3 = 0;
        uint32_t Base4 = 0;
        uint32_t Zero = 0;

        void setBase(uint64_t base);
        void setLimit(uint64_t limit);
        void setFlags(uint8_t flags);

        explicit GdtSysSegment(const uint8_t accessByte)
        {
            this->AccessByte = accessByte;
        }
    };

    class Gdt {
        GdtSegment userSegments[5];
        GdtSysSegment tssSegment;

    public:
        Gdt();
        ~Gdt() = default;

        void loadGdt();
    };
} //namespace InterruptManager::Arch::X86_64

#endif //INTERRUPT_MANAGER_ARCH_X86_64_GDT_H
#ifndef INTERRUPT_MANAGER_ARCH_X86_64_TSS_H
#define INTERRUPT_MANAGER_ARCH_X86_64_TSS_H
#include <cstdint>

struct Tss {
    uint32_t Reserved1 = 0;
    uint64_t PrivilegeStackPointers[3] = {};
    uint64_t Reserved2 = 0;
    uint64_t InterruptStackPointers[7] = {};
    uint64_t Reserved3 = 0;
    uint16_t Reserved4 = 0;
    uint16_t IoMapBaseAddress = 0;
};

#endif //INTERRUPT_MANAGER_ARCH_X86_64_TSS_H
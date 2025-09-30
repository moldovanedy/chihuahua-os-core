#include "gdt.h"

#include "tss.h"

using namespace InterruptManager::Arch::X86_64;

extern void setGdt(uint64_t limit, uint64_t base);

Gdt::Gdt()
    : userSegments{
        GdtSegment(0),
        GdtSegment(0),
        GdtSegment(0),
        GdtSegment(0),
        GdtSegment(0)
    },
      tssSegment(0)
{
    this->userSegments[0] = GdtSegment(0);

    //kernel code segment
    this->userSegments[1] = GdtSegment(0x9A);
    this->userSegments[1].setLimit(0xFFFFF);
    this->userSegments[1].setFlags(0x0A);

    //kernel data segment
    this->userSegments[2] = GdtSegment(0x92);
    this->userSegments[2].setLimit(0xFFFFF);
    this->userSegments[2].setFlags(0x0C);

    //user code segment
    this->userSegments[3] = GdtSegment(0xFA);
    this->userSegments[3].setLimit(0xFFFFF);
    this->userSegments[3].setFlags(0x0A);

    //user data segment
    this->userSegments[4] = GdtSegment(0xF2);
    this->userSegments[4].setLimit(0xFFFFF);
    this->userSegments[4].setFlags(0x0C);

    //TSS
    auto tss = Tss();
    this->tssSegment = GdtSysSegment(0x89);
    this->tssSegment.setBase(reinterpret_cast<uint64_t>(&tss));
    this->tssSegment.setLimit(sizeof(Tss) - 1);
}


void Gdt::loadGdt() {
    
}


#pragma region GdtSegment implementation

void GdtSegment::setLimit(const uint32_t limit) {
    this->Limit1 = limit & 0xFFFF;
    this->Limit2AndFlags |= (limit >> 16) & 0x0F;
}

void GdtSegment::setBase(const uint32_t base) {
    this->Base1 = base & 0xFFFF;
    this->Base2 = base >> 16;
    this->Base3 = base >> 24;
}

void GdtSegment::setFlags(const uint8_t flags) {
    this->Limit2AndFlags |= flags << 4;
}

#pragma endregion //GdtSegment implementation


#pragma region GdtSysSegment implementation

void GdtSysSegment::setLimit(const uint64_t limit) {
    this->Limit1 = limit & 0xFFFF;
    this->Limit2AndFlags |= (limit >> 16) & 0x0F;
}

void GdtSysSegment::setBase(const uint64_t base) {
    this->Base1 = base & 0xFFFF;
    this->Base2 = base >> 16;
    this->Base3 = base >> 24;
    this->Base4 = base >> 32;
}

void GdtSysSegment::setFlags(const uint8_t flags) {
    this->Limit2AndFlags |= flags << 4;
}

#pragma endregion //GdtSysSegment implementation
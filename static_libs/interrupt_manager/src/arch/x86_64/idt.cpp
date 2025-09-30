#include "idt.h"

using namespace InterruptManager::Arch::X86_64;

InterruptEntry InterruptEntry::MISSING_INTERRUPT_HANDLER = InterruptEntry {
    .Address1 = 0,
    .SegmentSelector = 0,
    .InterruptStackTable = 0,
    .Attributes = 0b1110,
    .Address2 = 0,
    .Address3 = 0,
    .Zero = 0,
};
#ifndef BOOTLOADER_GOP_H
#define BOOTLOADER_GOP_H

#include <efi.h>
#include "boot_params.h"

namespace Gop {
    bool setAppropriateFramebuffer(
        EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
        int preferredWidth,
        int preferredHeight,
        FramebufferInfo_t *fbInfo);
} //namespace Gop

#endif //BOOTLOADER_GOP_H
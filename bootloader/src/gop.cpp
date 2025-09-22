#include "gop.h"

#include <tuple>

namespace Gop {
    bool setAppropriateFramebuffer(
        EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
        const int preferredWidth,
        const int preferredHeight,
        FramebufferInfo_t *fbInfo)
    {
        *fbInfo = INVALID_FRAMEBUFFER_INFO;
        
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *bestModeInfo = gop->Mode->Info;
        std::tuple<int, int> bestDeviation =
            std::make_tuple(
                preferredWidth - bestModeInfo->HorizontalResolution,
                preferredHeight - bestModeInfo->VerticalResolution);
        uint32_t bestModeIndex = gop->Mode->Mode;

        const uint32_t modesCount = gop->Mode->MaxMode;
        for (uint32_t i = 0; i < modesCount; i++) {
            EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *thisModeInfo;
            uint64_t sizeOfInfo;

            const EFI_STATUS status = gop->QueryMode(gop, i, &sizeOfInfo, &thisModeInfo);
            if (EFI_ERROR(status)) {
                continue;
            }

            //we don't support anything other than RGB32 and BGR32
            if (
                thisModeInfo->PixelFormat != PixelBlueGreenRedReserved8BitPerColor
                && thisModeInfo->PixelFormat != PixelRedGreenBlueReserved8BitPerColor)
            {
                continue;
            }
            
            std::tuple<int, int> thisDeviation =
                std::make_tuple(
                    preferredWidth - thisModeInfo->HorizontalResolution,
                    preferredHeight - thisModeInfo->VerticalResolution);

            const uint32_t thisDeviationTotal =
                __builtin_abs(std::get<0>(thisDeviation) + std::get<1>(thisDeviation));
            const uint32_t currentDeviationTotal =
                __builtin_abs(std::get<0>(bestDeviation) + std::get<1>(bestDeviation));
            
            if (thisDeviationTotal < currentDeviationTotal) {
                bestModeInfo = thisModeInfo;
                bestDeviation = thisDeviation;
                bestModeIndex = i;
            }
        }

        *fbInfo = FramebufferInfo_t {
            bestModeInfo->HorizontalResolution,
            bestModeInfo->VerticalResolution,
            bestModeInfo->PixelsPerScanLine,
            bestModeInfo->PixelFormat == PixelRedGreenBlueReserved8BitPerColor,
        };
        
        if (bestModeInfo == gop->Mode->Info) {
            return true;
        }

        const EFI_STATUS status = gop->SetMode(gop, bestModeIndex);
        return !EFI_ERROR(status);
    }

}//namespace Gop
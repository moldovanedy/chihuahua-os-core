#include "elf/elf_definitions.h"
#include "cstring.h"

#include "elf/elf_loader.h"

extern "C" {
    int a() {
        return 5;
    }

    void b() {
        
    }
}

namespace Elf
{
    ElfLoader::ElfError ElfLoader::checkElf() const
    {
        const Elf64_ElfHeader header = *static_cast<Elf64_ElfHeader *>(this->elfFile);
        if (
            header.Identifiers[static_cast<int>(Elf_IdentIndex::EI_MAG0)] != ELF_MAG0 || header.Identifiers[static_cast<int>(Elf_IdentIndex::EI_MAG1)] != ELF_MAG1 || header.Identifiers[static_cast<int>(Elf_IdentIndex::EI_MAG2)] != ELF_MAG2 || header.Identifiers[static_cast<int>(Elf_IdentIndex::EI_MAG3)] != ELF_MAG3)
        {
            return ElfError::ElfHeaderCorrupted;
        }

        if (
            header.Identifiers[static_cast<int>(Elf_IdentIndex::EI_CLASS)] != 2 || header.Identifiers[static_cast<int>(Elf_IdentIndex::EI_VERSION)] != 1 || header.Version != 1 || header.Machine != Elf_Machine::x86_64 || header.Type != Elf_Type::ET_EXEC)
        {
            return ElfError::ElfTypeNotSupported;
        }

        return ElfError::NoError;
    }

    Elf64_ProgHeader *ElfLoader::getProgramHeaders(int *numProgHeaders, ElfError *error) const
    {
        *error = checkElf();
        if (*error != ElfError::NoError)
        {
            return nullptr;
        }

        const Elf64_ElfHeader elfHeader = *static_cast<Elf64_ElfHeader *>(elfFile);
        // it shouldn't be different, but you never know...
        if (elfHeader.ProgHeaderEntrySize != sizeof(Elf64_ProgHeader))
        {
            *error = ElfError::ElfGenericError;
            return nullptr;
        }

        *numProgHeaders = elfHeader.ProgHeaderTableEntriesNum;
        if (elfHeader.ProgHeaderOffset + (*numProgHeaders * sizeof(Elf64_ProgHeader)) >= this->elfFileSize)
        {
            *error = ElfError::ElfSizeExceeded;
            return nullptr;
        }

        return reinterpret_cast<Elf64_ProgHeader *>(static_cast<char *>(elfFile) + elfHeader.ProgHeaderOffset);
    }

    Elf64_SectionHeader *ElfLoader::getSectionHeaders(int *numSectionHeaders, ElfError *error) const
    {
        *error = checkElf();
        if (*error != ElfError::NoError)
        {
            return nullptr;
        }

        const Elf64_ElfHeader elfHeader = *static_cast<Elf64_ElfHeader *>(elfFile);
        // it shouldn't be different, but you never know...
        if (elfHeader.SectionHeaderEntrySize != sizeof(Elf64_SectionHeader))
        {
            *error = ElfError::ElfGenericError;
            return nullptr;
        }

        *numSectionHeaders = elfHeader.SectionHeaderTableEntriesNum;
        if (elfHeader.SectionHeaderOffset + (*numSectionHeaders * sizeof(Elf64_SectionHeader)) >= this->elfFileSize)
        {
            *error = ElfError::ElfSizeExceeded;
            return nullptr;
        }

        return reinterpret_cast<Elf64_SectionHeader *>(static_cast<char *>(elfFile) + elfHeader.SectionHeaderOffset);
    }

    ElfLoader::ElfError ElfLoader::loadExecutableProgram(
        const Elf64_ProgHeader *progHeader,
        const LoaderFunction loaderCallback)
        const
    {
        if (progHeader->SegmentType != Elf_SegmentType::PT_LOAD)
        {
            return ElfError::ElfSectionNotLoadable;
        }

        void *dest = loaderCallback(
            static_cast<int>(progHeader->SizeInMemory),
            progHeader->VirtAddress,
            progHeader->Flags);

        if (dest == nullptr)
        {
            return ElfError::ElfGenericError;
        }

        if (
            progHeader->Offset >= this->elfFileSize || progHeader->Offset + progHeader->SizeInFile >= this->elfFileSize || progHeader->Offset + progHeader->SizeInMemory >= this->elfFileSize)
        {
            return ElfError::ElfSizeExceeded;
        }

        memcpy(dest, static_cast<char *>(elfFile) + progHeader->Offset, progHeader->SizeInFile);

        // zero-out eventual mismatch between the size in file vs the size in memory
        if (progHeader->SizeInMemory > progHeader->SizeInFile)
        {
            memset(
                static_cast<char *>(dest) + progHeader->SizeInFile,
                0,
                progHeader->SizeInMemory - progHeader->SizeInFile);
        }

        return ElfError::NoError;
    }

    ElfLoader::ElfError ElfLoader::loadNoBitsSection(
        const Elf64_SectionHeader *sectionHeader,
        const LoaderFunction loaderCallback)
        const
    {
        if (
            sectionHeader->SectionHeaderType != Elf_SectionType::SHT_NOBITS || sectionHeader->SectionSize == 0 || (static_cast<uint32_t>(sectionHeader->Flags) & static_cast<uint32_t>(Elf_SectionFlags::SHF_ALLOC)) == 0)
        {
            return ElfError::ElfSectionNotLoadable;
        }

        if (
            sectionHeader->OffsetInFile >= this->elfFileSize || sectionHeader->OffsetInFile + sectionHeader->SectionSize >= this->elfFileSize)
        {
            return ElfError::ElfSizeExceeded;
        }

        void *dest = loaderCallback(
            static_cast<int>(sectionHeader->SectionSize),
            sectionHeader->VirtualAddress,
            Elf_SegmentFlags::PF_R | Elf_SegmentFlags::PF_W);

        if (dest == nullptr)
        {
            return ElfError::ElfGenericError;
        }

        memset(dest, 0, sectionHeader->SectionSize);
        return ElfError::NoError;
    }
} // namespace Elf
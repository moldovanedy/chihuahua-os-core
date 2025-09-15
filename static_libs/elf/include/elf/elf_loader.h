#ifndef ELF_ELF_LOADER_H
#define ELF_ELF_LOADER_H

#include <cstddef>

#include "elf_definitions.h"

extern "C" {
    int a();
    void b();
}

namespace Elf
{
    class ElfLoader
    {
        void *elfFile;
        size_t elfFileSize;

    public:
        ElfLoader(void *elfFilePtr, const size_t elfFileSize)
        {
            this->elfFile = elfFilePtr;
            this->elfFileSize = elfFileSize;
        }

        enum class ElfError
        {
            /**
             * Everything went OK.
             */
            NoError = 0,
            /**
             * The header (the ELF, section, or program header) was corrupted.
             */
            ElfHeaderCorrupted = 1,
            /**
             * The ELF file type is not supported, for example, the ELF is not for x86_64, or it is not a version 1 ELF.
             */
            ElfTypeNotSupported = 2,
            /**
             * The section is not loadable, so it was skipped. It's not an error per se, it's just a different state.
             * Applicable to program headers.
             */
            ElfSectionNotLoadable = 3,
            /**
             * A parameter indicated to a region outside the ELF file.
             */
            ElfSizeExceeded = 4,
            /**
             * A generic error.
             */
            ElfGenericError = INT32_MAX
        };

        /**
         * Specifies a function that loads data from the file at a given address. The address must be page-aligned.
         * @param memorySize The minimum required size.
         * @param virtAddress The virtual address where the data will "stay". Useful for paging.
         * @param segmentFlags The flags that should be used for the memory segment of the data: read, write, execute.
         * @return The address where the data can be written.
         */
        typedef void *(*LoaderFunction)(int memorySize, Elf64_Addr virtAddress, Elf_SegmentFlags segmentFlags);

        /**
         * Checks the ELF header.
         * @return ElfError::NoError if the ELF header is OK and the ELF type is supported, otherwise an error of type
         * ElfError.
         */
        [[nodiscard]] ElfError checkElf() const;

        /**
         * Returns the array of program headers with the size being set in numProgHeaders.
         * @param numProgHeaders [OUT] Returns the array size, including 0.
         * @param error [OUT] An eventual error, or ElfError::NoError if nothing went wrong.
         * @return A pointer to the first element of the array or nullptr if some error occurred (stored in error).
         */
        Elf64_ProgHeader *getProgramHeaders(int *numProgHeaders, ElfError *error) const;

        /**
         * Returns the array of section headers with the size being set in numSectionHeaders.
         * @param numSectionHeaders [OUT] Returns the array size, including 0.
         * @param error [OUT] An eventual error, or ElfError::NoError if nothing went wrong.
         * @return A pointer to the first element of the array or nullptr if some error occurred (stored in error).
         */
        Elf64_SectionHeader *getSectionHeaders(int *numSectionHeaders, ElfError *error) const;

        /**
         * Loads the PT_LOAD section from the given program header into memory at the virtual address specified
         * in the file data. The entire file must be loaded into memory.
         * @param progHeader The program header from the ELF file. You need to have the entire file in memory.
         * @param loaderCallback The function that needs to give the address at which to write the data.
         */
        ElfError loadExecutableProgram(const Elf64_ProgHeader *progHeader, LoaderFunction loaderCallback) const;

        /**
         * Loads a .bss/NO_BITS section from the given section header into memory at the virtual address specified
         * in the file data. The entire file must be loaded into memory.
         * @param sectionHeader The section header from the ELF file. You need to have the entire file in memory.
         * @param loaderCallback The function that needs to give the address at which to write the data.
         */
        ElfError loadNoBitsSection(const Elf64_SectionHeader *sectionHeader, LoaderFunction loaderCallback) const;
    };
} // namespace Elf

#endif // ELF_ELF_LOADER_H

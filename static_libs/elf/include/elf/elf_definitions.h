#ifndef ELF_ELF_DEFINITIONS_H
#define ELF_ELF_DEFINITIONS_H

#include <cstdint>

namespace Elf {
    /**
     * pointer (long uint)
     */
    typedef uint64_t Elf64_Addr;


#pragma region ELF file header
    
    constexpr int ELF_IDENT_SIZE = 16;

    constexpr char ELF_MAG0 = 0x7F;
    constexpr char ELF_MAG1 = 'E';
    constexpr char ELF_MAG2 = 'L';
    constexpr char ELF_MAG3 = 'F';
    
    /**
     * The ELF header identifier indices.
     */
    enum class Elf_IdentIndex: uint32_t {
        /**
         * 0x7F
         */
        EI_MAG0 = 0,
        /**
         * 'E' (0x45)
         */
        EI_MAG1 = 1,
        /**
         * 'L' (0x4C)
         */
        EI_MAG2 = 2,
        /**
         * 'F' (0x46)
         */
        EI_MAG3 = 3,
        /**
         * Architecture: always 2 (64-bit).
         */
        EI_CLASS = 4,
        /**
         * Endianness: 1 for little endian, 2 for big endian.
         */
        EI_DATA = 5,
        /**
         * ELF version: Always 1.
         */
        EI_VERSION = 6,
        /**
         * OS ABI: normally 0 (SysV) for bare metal.
         */
        EI_OSABI = 7,
        /**
         * OS specific; generally ignored.
         */
        EI_ABIVERSION = 8,
        /**
         * Padding bytes: 7 bytes that should be zeroed when written, ignored when read.
         */
        EI_PAD = 9
    };

    enum class Elf_Type : uint16_t {
        /**
         * Unknown.
         */
        ET_NONE = 0,
        /**
         * Relocatable file.
         */
        ET_REL = 1,
        /**
         * Executable file.
         */
        ET_EXEC = 2,
        /**
         * Dynamic library.
         */
        ET_DYN = 3,
        /**
         * Core file.
         */
        ET_CORE = 4,
        /**
         * Start of OS-specific range; generally ignored.
         */
        ET_LOOS = 0xFE00,
        /**
         * End of OS-specific range; generally ignored.
         */
        ET_HIOS = 0xFEFF,
    };

    enum class Elf_Machine : uint16_t {
        x86_64 = 0x3E,
        AArch64 = 0xB7,
    };


    /**
     * ELF header: the file's basic information holder.
     */
    struct Elf64_ElfHeader {
        /**
         * Identifiers like architecture, endianness, version etc. See Elf_IdentIndex for the array indexers to use.
         */
        uint8_t Identifiers[ELF_IDENT_SIZE];
        /**
         * File type; see Elf_Type for more info.
         */
        Elf_Type Type;
        /**
         * Target ISA; see Elf_Machine for more info.
         */
        Elf_Machine Machine;
        /**
         * Should always be 1.
         */
        uint32_t Version;
        /**
         * Program entry point. 0 if not an executable.
         */
        Elf64_Addr EntryPoint;
        /**
         * The offset (from the file start) of the program header table. Generally 0x40.
         */
        uint64_t ProgHeaderOffset;
        /**
         * The offset (from the file start) of the section header table.
         */
        uint64_t SectionHeaderOffset;
        /**
         * Architecture-specific flags.
         */
        uint32_t Flags;
        /**
         * The size of this header. Should always be 64.
         */
        uint16_t ThisSize;
        /**
         * The size of an entry in the program header table.
         */
        uint16_t ProgHeaderEntrySize;
        /**
         * The number of program header table entries.
         */
        uint16_t ProgHeaderTableEntriesNum;
        /**
         * The size of an entry in the section header table.
         */
        uint16_t SectionHeaderEntrySize;
        /**
        * The number of section header table entries.
        */
        uint16_t SectionHeaderTableEntriesNum;
        /**
         * The index of the entry in the section header table that contains the actual section names.
         */
        uint16_t SectionNamesEntryIndex;
    };
#pragma endregion //ELF file header


#pragma region ELF program header

    enum class Elf_SegmentType: uint32_t {
        /**
         * Unused program header table entry.
         */
        PT_NULL = 0,
        /**
         * Loadable segment.
         */
        PT_LOAD = 1,
        /**
         * Dynamic link info.
         */
        PT_DYNAMIC = 2,
        /**
         * Interpreter info.
         */
        PT_INTERP = 3,
        /**
         * Auxiliary info.
         */
        PT_NOTE = 4,
        /**
         * Reserved; not used.
         */
        PT_SHLIB = 5,
        /**
         * The segment containing the program header itself.
         */
        PT_PHDR = 6,
        /**
         * Thread-Local Storage template.
         */
        PT_TLS = 7,
        /**
         * Begin of OS-specific range.
         */
        PT_LOOS = 0x60000000,
        /**
         * End of OS-specific range.
         */
        PT_HIOS = 0x6FFFFFFF,
    };

    enum class Elf_SegmentFlags: uint32_t {
        PF_X = 1,
        PF_W = 2,
        PF_R = 4,
    };

    inline Elf_SegmentFlags operator|(Elf_SegmentFlags a, Elf_SegmentFlags b)
    {
        return static_cast<Elf_SegmentFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    struct Elf64_ProgHeader {
        /**
         * Segment type; see Elf_ProgramType for more info.
         */
        Elf_SegmentType SegmentType;
        /**
         * Segment-dependent flags; see Elf_SegmentFlags for more info.
         */
        Elf_SegmentFlags Flags;
        /**
         * Offset (from the beginning of the file) of the segment in the file.
         */
        uint64_t Offset;
        /**
         * The segment's virtual address.
         */
        Elf64_Addr VirtAddress;
        /**
         * The segment's physical address; should either be 0 or equal to VirtAddress.
         */
        Elf64_Addr PhysAddress;
        /**
         * The segment size in the file; might be 0.
         */
        uint64_t SizeInFile;
        /**
         * The segment size in memory; might be 0.
         */
        uint64_t SizeInMemory;
        /**
         * 0 and 1 specify no alignment. Otherwise, a power of two, where VirtAddress = Offset % Alignment.
         */
        uint64_t Alignment;
    };

#pragma endregion  //ELF program header


#pragma region ELF section header

    enum class Elf_SectionType : uint32_t {
        /**
         * Unused entry.
         */
        SHT_NULL = 0,
        /**
         * Program data.
         */
        SHT_PROGBITS = 1,
        /**
         * Symbol table.
         */
        SHT_SYMTAB = 2,
        /**
         * String table.
         */
        SHT_STRTAB = 3,
        /**
         * Relocation entries with addends.
         */
        SHT_RELA = 4,
        /**
         * Symbol hash table.
         */
        SHT_HASH = 5,
        /**
         * Dynamic link info
         */
        SHT_DYNAMIC = 6,
        /**
         * Notes.
         */
        SHT_NOTE = 7,
        /**
         * Program space with no data (e.g. .bss).
         */
        SHT_NOBITS = 8,
        /**
         * Relocation entries.
         */
        SHT_REL = 9,
        /**
         * Reserved.
         */
        SHT_SHLIB = 0x0A,
        /**
         * Dynamic linking symbol table.
         */
        SHT_DYNSYM = 0x0B,
        /**
         * Global constructors array.
         */
        SHT_INIT_ARRAY = 0x0E,
        /**
         * Global destructors array.
         */
        SHT_FINI_ARRAY = 0x0F,
        /**
         * Pre-constructors array.
         */
        SHT_PREINIT_ARRAY = 0x10,
        /**
         * Section group.
         */
        SHT_GROUP = 0x11,
        /**
         * Extended section indices.
         */
        SHT_SYMTAB_SHNDX= 0x12,
        /**
         * Number of defined types (i.e. the size of this enumeration).
         */
        SHT_NUM = 0x13,
    };

    enum class Elf_SectionFlags : uint64_t {
        /**
         * Writable section.
         */
        SHF_WRITE = 0x01,
        /**
         * Needs to occupy memory during execution.
         */
        SHF_ALLOC = 0x02,
        /**
         * Executable section.
         */
        SHF_EXECINSTR = 0x04,
        /**
         * Might be merged.
         */
        SHF_MERGE = 0x10,
        /**
         * Contains null-terminated strings.
         */
        SHF_STRINGS = 0x20,
        /**
         * Elf64_SectionHeader::Info contains SHT index (?).
         */
        SHF_INFO_LINK = 0x40,
        /**
         * Preserve order after combining.
         */
        SHF_LINK_ORDER = 0x80,
        /**
         * OS-specific handling is required.
         */
        SHF_OS_NONCONFORMING = 0x100,
        /**
         * The section is member of a group.
         */
        SHF_GROUP = 0x200,
        /**
         * Section holds thread-local data.
         */
        SHF_TLS = 0x400,
    };

    inline Elf_SectionFlags operator|(Elf_SectionFlags a, Elf_SectionFlags b)
    {
        return static_cast<Elf_SectionFlags>(static_cast<int>(a) | static_cast<int>(b));
    }

    struct Elf64_SectionHeader {
        /**
         * The index of the string (in the ".shstrtab" section) that represents this section.
         */
        uint32_t SectionNameIndex;
        /**
         * Section header type.
         */
        Elf_SectionType SectionHeaderType;
        /**
         * Section attribute flags.
         */
        Elf_SectionFlags Flags;
        /**
         * The virtual address of this section.
         */
        Elf64_Addr VirtualAddress;
        /**
         * Offset (from the beginning of the file) of this section.
         */
        uint64_t OffsetInFile;
        /**
         * The size in bytes of this section; might be 0.
         */
        uint64_t SectionSize;
        /**
         * ?
         */
        uint32_t LinkInfo;
        /**
         * ?
         */
        uint32_t Info;
        /**
         * Must be a power of two.
         */
        uint64_t AddressAlignment;
        /**
         * The size in bytes of each entry for sections that contain fixed-size entries; otherwise it's 0.
         */
        uint64_t EntryFixedSize;
    };
    
#pragma endregion //ELF section header
    
} //namespace Elf

#endif //ELF_ELF_DEFINITIONS_H

#ifndef ELF_H
#define ELF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define EI_MAGIC   "\x7f" "ELF"

#define ELF_REL_SYM64(i)    (i) >> 32
#define ELF_REL_TYPE64(i)   (i) & 0xffffffff

enum elf_class {
    ECLASS_NONE = 0x0,
    ECLASS_32BIT = 0x1,
    ECLASS_64BIT = 0x2
};

enum elf_data {
    EDATA_LITTLE = 0x1,
    EDATA_BIG = 0x2
};

enum elf_abi {
    ABI_SYSV = 0x0,
    ABI_HPUX = 0x1,
    ABI_NBSD = 0x2,
    ABI_LINUX = 0x3,
    ABI_HURD = 0x4,
    ABI_SLRS = 0x6,     // Solaris
    ABI_AIX = 0x7,
    ABI_IRIX = 0x8,
    ABI_FBSD = 0x9,
    ABI_TRU64 = 0xA,
    ABI_NMOD = 0xB,     // Novell Modesto
    ABI_OBSD = 0xC,
    ABI_OVMS = 0xD,
    ABI_NSKR = 0xE,
    ABI_AROS = 0xF,
    ABI_FENX = 0x10,    // FenixOS
    ABI_NUXI = 0x11,
    ABI_OVOS = 0x12     // Stratus Technologies OpenVOS
};

enum elf_etype {
    ET_NONE = 0x0,
    ET_REL = 0x1,
    ET_EXEC = 0x2,
    ET_DYN = 0x3,
    ET_CORE = 0x4,
    ET_LOOS = 0xFE00,
    ET_HIOS = 0xFEFF,
    ET_LOPROC = 0xFF00,
    ET_HIPROC = 0xFFFF
};

enum elf_shtype : uint32_t {
    SH_NULL           = 0,
    SH_PROGBITS       = 1,
    SH_SYMTAB         = 2,
    SH_STRTAB         = 3,
    SH_RELA           = 4,
    SH_HASH           = 5,
    SH_DYNAMIC        = 6,
    SH_NOTE           = 7,
    SH_NOBITS         = 8,
    SH_REL            = 9,
    SH_SHLIB          = 10,
    SH_DYNSYM         = 11,
    SH_INIT_ARRAY     = 14,
    SH_FINI_ARRAY     = 15,
    SH_PREINIT_ARRAY  = 16,
    SH_GROUP          = 17,
    SH_SYMTAB_SHNDX   = 18,
    SH_RELR           = 19,
    SH_LOOS           = 0x60000000,
    SH_HIOS           = 0x6fffffff,
    SH_LOPROC         = 0x70000000,
    SH_HIPROC         = 0x7fffffff,
    SH_LOUSER         = 0x80000000,
    SH_HIUSER         = 0xffffffff,
};

enum elf_phtype {
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PT_SHLIB = 5,
    PT_PHDR = 6,
    PT_TLS = 7,
    PT_LOOS = 0x60000000,
    PT_HIOS = 0x6fffffff,
    PT_LOPROC = 0x70000000,
    PT_HIPROC = 0x7fffffff
};

enum elf_dyntype {
    DT_NULL = 0,
    DT_HASH = 4,
    DT_GNU_HASH = 4,
    DT_STRTAB = 5,
    DT_SYMTAB = 6,
    DT_RELA = 7,
    DT_RELASZ = 8,
    DT_RELAENT = 9,
    DT_STRSZ = 10,
    DT_SYMENT = 11
};

struct eh_ident {
    uint32_t ei_mag;
    uint8_t ei_class;
    uint8_t ei_data;
    uint8_t ei_version;
    uint8_t ei_osabi;
    uint8_t ei_abiversion;
    uint8_t ei_pad[7];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
};

struct eh_offs64 {
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
};

struct eh_trailer {
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shtrndx;
};

struct elf_shdr64 {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_align;
    uint64_t sh_esize;  // table entry size
};

struct elf_phdr64 {
    uint32_t p_type;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
};

struct elf_dyn64 {
    int64_t d_tag;
    uint64_t d_val;
};

struct elf_sym64 {
    uint32_t st_name;
    uint8_t st_info;
    uint8_t st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
};

struct elf_rela64 {
    uint64_t r_offset;
    uint64_t r_info;
    int64_t r_addend;
};

int elf_read64(FILE* elf);

/* psABI-specific functions*/
void elf_do_rela(FILE* elf, struct elf_rela64* relatab, size_t rela_entries,
    size_t symtab_offs, size_t symsz,
    char* strtab, size_t strtabsz);

#endif

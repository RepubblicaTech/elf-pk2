#include <elf.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int elf_read64(FILE* elf) {
    struct eh_offs64 eh_offsets;
    struct eh_trailer eh;
    fseek(elf, sizeof(struct eh_ident), SEEK_SET);
    if (fread(&eh_offsets, sizeof(struct eh_offs64), 1, elf) != 1) {
        fprintf(stderr, "Couldn't read ELF sections offsets!\n");
        return -1;
    }
    if (fread(&eh, sizeof(struct eh_trailer), 1, elf) != 1) {
        fprintf(stderr, "Couldn't read ELF header trailer!\n");
        return -1;
    }

    // all should be populated after looping through headers
    size_t relaoffs, relasz, relatabsz;
    size_t stroffs, strtabsz;
    size_t symoffs, symsz;

    printf("Program headers @ %#lx; %hd*%#hx\n", eh_offsets.e_phoff, eh.e_phnum, eh.e_phentsize);
    for (uint16_t p = 0; p < eh.e_phnum; p++) {
        size_t offs = eh_offsets.e_phoff + (eh.e_phentsize * p);
        struct elf_phdr64 phdr;
        fseek(elf, offs, SEEK_SET);
        if (fread(&phdr, sizeof(struct elf_phdr64), 1, elf) != 1) {
            fprintf(stderr, "Couldn't read ELF program header n.%hu!\n", p);
            return -1;
        }

        printf("phdr[%hu] @ %#zx\n", p, offs);
        printf("\ttype (%u): ", phdr.p_type);
        switch (phdr.p_type) {
            case PT_NULL:
                printf("UNUSED\n");
                break;
            case PT_LOAD:
                printf("loadable\n");
                printf("\tData @ %#lx in file; len=%u\n", phdr.p_offset, phdr.p_filesz);
                printf("\tVAddr %#lx\n", phdr.p_vaddr);
                break;
            case PT_DYNAMIC:
                printf("dynamic linking\n");
                fseek(elf, phdr.p_offset, SEEK_SET);
                struct elf_dyn64* dyns = malloc(phdr.p_filesz);
                size_t dyn_entries = phdr.p_filesz / sizeof(struct elf_dyn64);
                if (fread(dyns, phdr.p_filesz, 1, elf) != 1) {
                    fprintf(stderr, "Couldn't read ELF dynamic linking data!\n");
                    return -2;
                }
                for (size_t d = 0; d < dyn_entries; d++) {
                    struct elf_dyn64* dyn = &dyns[d];
                    if (dyn->d_tag == DT_NULL) break;
                    printf("\t");
                    switch (dyn->d_tag) {
                        case DT_SYMTAB:
                            printf("symtab @ %#lx\n", dyn->d_val);
                            symoffs = dyn->d_val;
                            break;
                        case DT_STRTAB:
                            printf("strtab @ %#lx\n", dyn->d_val);
                            stroffs = dyn->d_val;
                            break;
                        case DT_RELA:
                            printf("relatab @ %#lx\n", dyn->d_val);
                            relaoffs = dyn->d_val;
                            break;
                        case DT_RELASZ:
                            printf("relatab size: %#lx\n", dyn->d_val);
                            relatabsz = dyn->d_val;
                            break;
                        case DT_RELAENT:
                            printf("rela entry size: %#lx\n", dyn->d_val);
                            relasz = dyn->d_val;
                            break;
                        case DT_STRSZ:
                            printf("strtab size: %#lx\n", dyn->d_val);
                            strtabsz = dyn->d_val;
                            break;
                        case DT_SYMENT:
                            printf("sym entry size: %#lx\n", dyn->d_val);
                            symsz = dyn->d_val;
                            break;
                        default:
                            printf("tag:%#lx; val=%#lx\n", dyn->d_tag, dyn->d_val);
                            break;
                    }
                }
                break;
            default:
                printf("unknown/unsupported\n");
                break;
        }
    }

    fseek(elf, stroffs, SEEK_SET);
    char* strtab = malloc(strtabsz);
    if (fread(strtab, strtabsz, 1, elf) != 1) {
        fprintf(stderr, "Couldn't read string table!\n");
        free(strtab);
        return -3;
    }
    fseek(elf, relaoffs, SEEK_SET);
    struct elf_rela64* relatab = malloc(relatabsz);
    size_t rela_entries = relatabsz / relasz;
    if (fread(relatab, relatabsz, 1, elf) != 1) {
        fprintf(stderr, "Couldn't read symbol table!\n");
        free(strtab);
        free(relatab);
        return -3;
    }

    elf_do_rela(elf, relatab, rela_entries, symoffs, symsz, strtab, strtabsz);

    printf("Section headers @ %#lx; %hd*%#hx\n", eh_offsets.e_shoff, eh.e_shnum, eh.e_shentsize);

    free(strtab);
    free(relatab);

    return 0;
}

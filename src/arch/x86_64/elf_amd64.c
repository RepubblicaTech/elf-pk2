#include <elf.h>
#include <elf_arch.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void elf_do_rela(FILE* elf, struct elf_rela64* relatab, size_t rela_entries, size_t symtab_offs, size_t symsz, char* strtab, size_t strtabsz) {
    for (size_t r = 0; r < rela_entries; r++) {
        struct elf_rela64* rela = &relatab[r];
        printf("RELA[%zu]\n", r);

        size_t s = ELF_REL_SYM64(rela->r_info);
        printf("\t@ symtab[%lu]\n", s);
        fseek(elf, symtab_offs + (s * symsz), SEEK_SET);
        struct elf_sym64* sym = malloc(symsz);
        if (fread(sym, symsz, 1, elf) != 1) {
            fprintf(stderr, "Couldn't read symbol table!\n");
            free(sym);
            return;
        }

        if (sym->st_name > strtabsz) {
            fprintf(stderr, "String index out of bounds!\n");
            free(sym);
            return;
        }

        char* symname = &strtab[sym->st_name];
        printf("\tSymbol (idx %u): %s\n", sym->st_name, symname);

        printf("\toffset %#lx\n", rela->r_offset);
        printf("\taddend %ld\n", rela->r_addend);

        printf("\ttype (%lu): ", ELF_REL_TYPE64(rela->r_info));
        switch (ELF_REL_TYPE64(rela->r_info)) {
            case R_X8664_64:
                printf("Symbol address + addend\n");
                break;
            case R_X8664_GLOB_DAT:
            case R_X8664_JUMP_SLOT:
                printf("Symbol address\n");
                break;
            case R_X8664_RELATIVE:
                printf("Program base + addend\n");
                break;
            default:
                printf("Unknown\n");
                break;
        }

        free(sym);
    }
}

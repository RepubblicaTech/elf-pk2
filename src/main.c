#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <elf.h>

int main(int argc, char **argv) {
    struct eh_ident ei;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s path/to/elf\n", argv[0]);
        return -1;
    }

    FILE* elf = fopen(argv[1], "r");
    if (!elf) {
        fprintf(stderr, "Could not open %s!\n", argv[1]);
        return -2;
    }
    if (fread(&ei, sizeof(struct eh_ident), 1, elf) != 1) {
        fprintf(stderr, "Error reading file %s!\n", argv[1]);
        fclose(elf);
        return -3;
    }
    if (strncmp((char*)&ei.ei_mag, EI_MAGIC, strlen(EI_MAGIC)) != 0) {
        fprintf(stderr, "Invalid ELF magic %s!\n", (char*)&ei.ei_mag);
        fclose(elf);
        return -4;
    }

    switch (ei.ei_class) {
        case ECLASS_64BIT:
            if (elf_read64(elf) != 0) {
                fprintf(stderr, "Something went wrong when parsing the ELF!\n");
                return -5;
            }
            break;
        default:
            fprintf(stderr, "Unknown/unsupported ELF architecture!\n");
            break;
    }

    fclose(elf);
    return 0;
}

/**
 * @file
 * @brief Readelf shows information about ELF binaries.
 *
 * @date 2.05.10
 * @author Anatoly Trosinenko
 *         - Initial implementation.
 *           Huge parts of code are written by Aleksandr Kirov
 * FIXME: aggrh, spaghetti code turns your brain into Camembert cheese. (sikmir)
 */

#include <ctype.h>
#include <shell_command.h>
#include <lib/libelf.h>
#include <string.h>

#define COMMAND_NAME     "readelf"
#define COMMAND_DESC_MSG "shows information about ELF binaries"
#define HELP_MSG         "Usage: readelf [-hspramt] <filename>"

static const char *man_page =
#include "readelf.inc"
        ;

DECLARE_SHELL_COMMAND(COMMAND_NAME, exec, COMMAND_DESC_MSG, HELP_MSG, man_page);

extern int errno;

static uint32_t rev_long(uint32_t n, uint8_t reversed) {
        if (reversed == 2) {
                return ((n & 0x000000ff) << 24)
                       + ((n & 0x0000ff00) << 8)
                       + ((n & 0x00ff0000) >> 8)
                       + ((n & 0xff000000) >> 24);
        } else {
                return n;
        }
}

static uint16_t rev_short(uint16_t n, uint8_t reversed) {
        if (reversed == 2) {
                return ((n & 0x00ff) << 8) + ((n & 0xff00) >> 8);
        } else {
                return n;
        }
}

static void print_header(Elf32_Ehdr *head) {
        int i, x;
        printf("e_ident : ");

        for (i = 0; i < 16; i++) {
                if (isgraph(head->e_ident[i])) {
                        printf("%c ", head->e_ident[i]);
                } else {
                        printf("0x%x ", (int) head->e_ident[i]);
                }
        }
        printf("\n");

        printf("EI_CLASS - File class : ");
        switch (head->e_ident[4]) {
        case 0:
                printf("EFLCLASSNONE - Invalid class");
                break;
        case 1:
                printf("ELFCLASS32 - 32-bit objects");
                break;
        case 2:
                printf("ELFCLASS64 - 64-bit objects");
                break;
        default:
                printf("Unknown class (%d)", head->e_ident[4]);
                break;
        }
        printf("\n");

        printf("EI_DATA - Data encoding : ");
        switch (head->e_ident[5]) {
        case 0:
                printf("ELFDATANONE - Invalid data encoding");
                break;
        case 1:
                printf("ELFDATA2LSB");
                break;
        case 2:
                printf("ELFDATA2MSB");
                break;
        default:
                printf("Unknown data encoding (%d)", head->e_ident[5]);
                break;
        }
        printf("\n");

        printf("EI_VERSION - ELF header version number : ");
        switch (head->e_ident[6]) {
        case 0:
                printf("EV_NONE - Invalid version");
                break;
        case 1:
                printf("EV_CURRENT - Current version");
                break;
        default:
                printf("Unknown version (%d)", head->e_ident[6]);
                break;
        }
        printf("\n");

        printf("e_type - object file type : ");
        x = rev_short(head->e_type, head->e_ident[5]);
        switch (x) {
        case 0:
                printf("ET_NONE - No file type");
                break;
        case 1:
                printf("ET_REL - Relocatable file");
                break;
        case 2:
                printf("ET_EXEC - Executable file");
                break;
        case 3:
                printf("ET_DYN - Shared object file");
                break;
        case 4:
                printf("ET_CORE - Core file");
                break;
        default:
                if (0xff00 <= x && x <= 0xffff) {
                        printf("Processor specific (0x%x)", x);
                } else {
                        printf("Unknown type (0x%x)", x);
                }
                break;
        }
        printf("\n");

        printf("e_machine - required architecture for an individual file : ");
        x = rev_short(head->e_machine, head->e_ident[5]);
        switch (x) {
        case 0:
                printf("EM_NONE - No machine");
                break;
        case 1:
                printf("EM_M32 - AT&T WE 32100");
                break;
        case 2:
                printf("EM_SPARC - SPARC");
                break;
        case 3:
                printf("EM_386 - Intel 80386");
                break;
        case 4:
                printf("EM_68K - Motorola 68000");
                break;
        case 5:
                printf("EM_88K - Motorola 88000");
                break;
        case 7:
                printf("EM_860 - Intel 80860");
                break;
        case 8:
                printf("EM_MIPS - MIPS RS3000");
                break;
        default:
                printf("%d", x);
        }
        printf("\n");

        printf("e_version - Object file version : ");
        x = rev_long(head->e_version, head->e_ident[5]);
        switch (x) {
        case 0:
                printf("EV_NONE - Invalid version");
                break;
        case 1:
                printf("EV_CURRENT - Current version");
                break;
        default:
                printf("Unknown version (%u)", x);
        }
        printf("\n");

        printf("e_entry - Virtual adress first to transfer control and start \
		   process (if no entry point then 0) : %u\n",
               rev_long(head->e_entry, head->e_ident[5]));
        printf("e_phoff - Program header table's file offset \
		   (0 if no such one) : %u\n",
               rev_long(head->e_phoff, head->e_ident[5]));
        printf("e_shoff - Section header table's file offset \
		   (0 if no such one) : %u\n",
               rev_long(head->e_shoff, head->e_ident[5]));
        printf("e_flags - Processor specific flags associated \
 		   with file : %u\n",
               rev_long(head->e_flags, head->e_ident[5]));
        printf("e_ehsize - ELF header's size : 0x%x \n",
               rev_short(head->e_ehsize, head->e_ident[5]));
        printf("e_phentsize - Size of entry in file's program \
		   header table : 0x%x \n",
               rev_short(head->e_phentsize, head->e_ident[5]));
        printf("e_phnum - Number of entries in program header table : 0x%x \n",
               rev_short(head->e_phnum, head->e_ident[5]));
        printf("e_shentsize - Section header's size : 0x%x \n",
               rev_short(head->e_shentsize, head->e_ident[5]));
        printf("e_shnum - Number of entries in the section header table :0x%x \n",
               rev_short(head->e_shnum, head->e_ident[5]));
        printf("e_shstrndx - Section table header table index of the entry : 0x%x \n",
               rev_short(head->e_shstrndx, head->e_ident[5]));
}

static void print_sections(Elf32_Shdr *sections, int8_t *names, int count, uint8_t rev) {
        size_t i;
        if (names == NULL) {
                return;
        }
        printf("Names of sections:\n");
        for (i = 0; i < count; i++) {
                printf("\tsection #%d:\t%s", i, &(names[rev_long(sections[i].sh_name, rev)]));
        }
}

static void print_section_head(Elf32_Shdr *section_header, uint8_t rev,
                        int8_t *string_table) {
        int x;
        Elf32_Word flags;

        printf("\n -- section info --\n\n");
        printf("sh_name - name of section : %u - ",
               rev_long(section_header->sh_name, rev));

        if (string_table == NULL || rev_long(section_header->sh_name, rev) == 0) {
                printf("no name");
        } else {
                printf("%s", &(string_table[rev_long(section_header->sh_name, rev)]));
        }
        printf("\n");

        printf("sh_type - type of section header : ");
        x = rev_long(section_header->sh_type, rev);
        switch (x) {
        case 0:
                printf("SHT_NULL");
                break;
        case 1:
                printf("SHT_PROGBITS");
                break;
        case 2:
                printf("SHT_SYMTAB");
                break;
        case 3:
                printf("SHT_STRTAB");
                break;
        case 4:
                printf("SHT_RELA");
                break;
        case 5:
                printf("SHT_HASH");
                break;
        case 6:
                printf("SHT_DYNAMIC");
                break;
        case 7:
                printf("SHT_NOTE");
                break;
        case 8:
                printf("SHT_NOBITS");
                break;
        case 9:
                printf("SHT_REL");
                break;
        case 10:
                printf("SHT_SHLIB");
                break;
        case 11:
                printf("SHT_DYNSYM");
                break;
        default:
                if (0x70000000 <= x && x <= 0x7fffffff) {
                        printf("Processor specific (0x%x)", x);
                } else {
                        if (0x80000000 <= x && x <= 0xffffffff) {
                                printf("Type reserved for applications (0x%x)", x);
                        } else {
                                printf("Unknown type (0x%x)", x);
                        }
                }
        }
        printf("\n");

        flags = rev_long(section_header->sh_flags, rev);

        printf("sh_flags - 1-bit flags : %lx : ", flags);
        if (flags & 0x1) printf("SHF_WRITE ");
        if (flags & 0x2) printf("SHF_ALLOC ");
        if (flags & 0x4) printf("SHF_EXECINSTR ");
        if (flags & 0x10) printf("SHF_MEGRE ");
        if (flags & 0x20) printf("SHF_STRINGS ");
        if (flags & 0x40) printf("SHF_INFO_LINK ");
        if (flags & 0x80) printf("SHF_LINK_ORDER ");
        if (flags & 0x100) printf("SHF_OS_NONCONFORMING ");
        if (flags & 0x200) printf("SHF_GROUP ");
        if (flags & 0x400) printf("SHF_TLS ");
        if (flags & 0x0ff00000) {
                printf("(SHF_MASKOS flags = 0x%lx) ", (flags >> 20) & 0xff);
        }
        if (flags & 0xf0000000) {
                printf("(SHF_MASKPROC flags = 0x%lxu)", (flags >> 28) & 0xf);
        }
        printf("\n");

        printf("sh_addr - first byte in memory image of process : %u\n",
               rev_long(section_header->sh_addr, rev));

        printf("sh_offset - first byte of section in file : %u\n",
               rev_long(section_header->sh_offset, rev));

        printf("sh_size - section size : %u \n",
               rev_long(section_header->sh_size, rev));

        printf("sh_link - section header table index link: %u\n",
               rev_long(section_header->sh_link, rev));

        printf("sh_info - extra information: %u\n",
               rev_long(section_header->sh_info, rev));

        printf("sh_addralign - : %u\n",
               rev_long(section_header->sh_addralign, rev));

        printf("sh_entsize - : %u\n",
               rev_long(section_header->sh_entsize, rev));
}

static void print_segment_head(Elf32_Phdr * segment_header, uint8_t reversed) {
        int x;

        printf("\n -- segment info --\n\n");

        printf("p_type - type of segment : ");
        x = rev_long(segment_header->p_type, reversed);
        switch (x) {
        case 0:
                printf("PT_NULL");
                break;
        case 1:
                printf("PT_LOAD");
                break;
        case 2:
                printf("PT_DYNAMIC");
                break;
        case 3:
                printf("PT_INTERP");
                break;
        case 4:
                printf("PT_NOTE");
                break;
        case 5:
                printf("PT_SHLIB");
                break;
        case 6:
                printf("PT_PHDR");
                break;
        case 7:
                printf("PT_TLS");
                break;
        default:
                if (0x70000000 <= x && x <= 0x7fffffff) {
                        printf("Processor specific (0x%x)", x);
                } else {
                        printf("Unknown type (0x%x)", x);
                }
                break;
        }
        printf("\n");

        printf("p_offset - first byte of segment in file : %u\n",
               rev_long(segment_header->p_offset, reversed));

        printf("p_vaddr - virtual address for first byte : %u\n",
               rev_long(segment_header->p_vaddr, reversed));

        printf("p_paddr - segments physical address : %u\n",
               rev_long(segment_header->p_paddr, reversed));

        printf("p_filesz - size of file image of the  segment : %u\n",
               rev_long(segment_header->p_filesz, reversed));

        printf("p_memsz - size of memory image of the segment : %u\n",
               rev_long(segment_header->p_memsz, reversed));

        printf("p_flags - flags relevant for the segment : %u\n",
               rev_long(segment_header->p_flags, reversed));

        printf("p_align - segments are aligned in memory and in the file : %u\n",
               rev_long(segment_header->p_align, reversed));
}

static void print_rel(Elf32_Rel *rel_array, int count, int reversed) {
        size_t i;

        if (count != 0) {
                printf("\n");

                for (i = 0; i < count; i++) {
                        printf("\nr_offset : %u", rev_long(rel_array[i].r_offset, reversed));
                        printf("\nr_info : %u\n", rev_long(rel_array[i].r_info, reversed));
                }
        } else {
                printf("\n\nList of Elf32_Rel structures if empty\n");
        }
}

static void print_rela(Elf32_Rela *rela_array, int count, int reversed) {
        size_t i;

        if (count != 0) {
                printf("\n");

                for (i = 0; i < count; i++) {
                        printf("\nr_offset : %u", rev_long(rela_array[i].r_offset, reversed));
                        printf("\nr_info : %u", rev_long(rela_array[i].r_info, reversed));
                        printf("\nr_addend : %u\n", rev_long(rela_array[i].r_addend, reversed));
                }
        } else {
                printf("\nList of Elf32_Rela structures if empty\n");
        }
}

/* TODO names*/
static void print_symb(Elf32_Sym *symb, int8_t *names,
                int counter, char reversed) {
        size_t i;

        printf("\n");
        if (counter != 0) {
                for (i = 0; i < counter; i++) {
                        printf("\n Symbol #%d \n", i);
                        printf("st_name - name of symbol, index : %u and name : ",
                               rev_long(symb[i].st_name, reversed));
                        if ((names != NULL) && (rev_long(symb[i].st_name, reversed) != 0)) {
                                printf("%s", &(names[rev_long(symb[i].st_name, reversed)]));
                        }
                        printf("\n");

                        printf("st_value - value of symbol : %u\n",
                               rev_long(symb[i].st_value, reversed));

                        printf("st_size - size of object : %u\n",
                               rev_long(symb[i].st_size, reversed));

                        printf("st_info - type and binding attributes : %c \n",
                               symb[i].st_info);

                        printf("st_other - not defined : %c \n",
                               symb[i].st_other);

                        printf("st_shndx - index of according section %hd \n",
                               rev_short(symb[i].st_shndx, reversed));
                }
        } else {
                printf("\nList of symbols is empty \n");
        }
}

static int exec(int argsc, char **argsv) {
        int op, e, i, cnt;
        char show_head = 0, show_sections = 0, show_segments = 0, show_rel = 0,
                                    show_rela = 0, show_symb = 0, show_names = 0;
        FILE *f;
        Elf32_Ehdr head;
        Elf32_Shdr sections[MAX_NUMBER_OF_SECTIONS];
        Elf32_Phdr segments[MAX_SEGMENTS];
        Elf32_Rel rel[MAX_REL_ARRAY_LENGTH];
        Elf32_Rela rela[MAX_RELA_ARRAY_LENGTH];
        Elf32_Sym symb[MAX_SYMB];
        int8_t names[MAX_NAME_LENGTH];
        int8_t symb_names[MAX_SYMB_NAMES];
        int names_l, sections_count, rel_count, rela_count, symb_count, symb_names_l;

        if (argsc <= 1) {
                show_help();
                return 1;
        }
        getopt_init();
        cnt = 0;
        do {
                cnt++;
                op = getopt(argsc - 1, argsv, "hspramt");
                switch (op) {
                case 'h':
                        show_head = 1;
                        break;
                case 's':
                        show_sections = 1;
                        break;
                case 'p':
                        show_segments = 1;
                        break;
                case 'r':
                        show_rel = 1;
                        break;
                case 'a':
                        show_rela = 1;
                        break;
                case 'm':
                        show_symb = 1;
                        break;
                case 't':
                        show_names = 1;
                        break;
                case -1:
                        cnt -= 1;
                        break;
                default:
                        show_help();
                        return 1;
                }
        } while (op != -1);

        if (cnt == 0) {
                show_head = 1;
                show_sections = 1;
                show_segments = 1;
                show_rel = 1;
                show_rela = 1;
                show_symb = 1;
                show_names = 1;
        }

        f = fopen(argsv[argsc - 1], "r");
        if (f == NULL) {
                printf("Cannot open file %s: %s\n", argsv[argsc - 1], strerror(errno));
                return 1;
        }
        // TODO strerror -> elf_error
        if ((e = elf_read_header(f, &head)) < 0) {
                printf("Cannot read header: %d\n", e);
                return 1;
        }
        // TODO use correct sections[] length
        sections_count = rev_short(head.e_shnum, head.e_ident[5]);
        if (show_sections || show_rel || show_rela || show_symb || show_names) {
                if ((e = elf_read_sections_table(f, &head, sections)) < 0) {
                        printf("Cannot read sections table: %d\n", e);
                        show_sections = 0;
                        show_rel = 0;
                        show_rela = 0;
                        show_symb = 0;
                        show_names = 0;
                }
        }
        if (show_sections) {
                if ((e = elf_read_string_table(f, &head, sections, names, &names_l)) < 0) {
                        printf("Cannot read string table: %d\n", e);
                        names_l = 0;
                        show_sections = 0;
                        show_names = 0;
                }
        }
        if (show_segments) {
                if ((e = elf_read_segments_table(f, &head, segments)) < 0) {
                        printf("Cannot read segments table (program header): %d\n", e);
                        show_segments = 0;
                }
        }
        if (show_rel) {
                if ((e = elf_read_rel_table(f, &head, sections, rel, &rel_count)) < 0) {
                        printf("Cannot read rel table: %d\n", e);
                        show_rel = 0;
                }
        }
        if (show_rela) {
                if ((e = elf_read_rela_table(f, &head, sections, rela, &rela_count)) < 0) {
                        printf("Cannot read rela table: %d\n", e);
                        show_rela = 0;
                }
        }
        if (show_symb) {
                if ((e = elf_read_symbol_table(f, &head, sections, symb, &symb_count)) < 0) {
                        printf("Cannot read symbol table: %d\n", e);
                        show_symb = 0;
                }
                if ((e = elf_read_symbol_string_table(f, &head, sections, names,
                                                      symb_names, &symb_names_l)) < 0) {
                        printf("Cannot read symbol names: %d\n", e);
                        symb_names_l = 0;
                }
        }

        if (show_head) {
                print_header(&head);
        }

        if (show_names && names_l != 0) {
                print_sections(sections, names, sections_count, head.e_ident[5]);
        }
        if (show_sections) {
                printf("\n %d sections: \n", sections_count);
                for (i = 0; i < sections_count; i++) {
                        print_section_head(&(sections[i]), head.e_ident[5],
                                           (names_l != 0) ? names : NULL);
                }
        }

        if (show_segments) {
                printf("\n%d segments: \n", rev_short(head.e_phnum, head.e_ident[5]));
                for (i = 0; i < rev_short(head.e_phnum, head.e_ident[5]); i++) {
                        print_segment_head(&(segments[i]), head.e_ident[5]);
                }
        }
        if (show_rel) {
                print_rel(rel, rel_count, head.e_ident[5]);
        }
        if (show_rela) {
                print_rela(rela, rela_count, head.e_ident[5]);
        }
        if (show_symb) {
                print_symb(symb, (symb_names_l != 0) ? symb_names : NULL, symb_count, head.e_ident[5]);
        }
        return 0;
}


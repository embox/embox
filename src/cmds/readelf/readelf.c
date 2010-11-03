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

typedef struct header_item {
	int  i;
	char desc[100];
} header_item_t;

static const header_item_t header_class_desc[] = {
	{ ELFCLASSNONE, "none"  },
	{ ELFCLASS32,   "ELF32" },
	{ ELFCLASS64,   "ELF64" },
};

static const header_item_t header_data_desc[] = {
	{ ELFDATANONE, "none"                          },
	{ ELFDATA2LSB, "2's complement, little endian" },
	{ ELFDATA2MSB, "2's complement, big endian"    },
};

static const header_item_t header_osabi_desc[] = {
	{ ELFOSABI_NONE,      "UNIX - System V"      },
#ifdef ALL_ELF
	{ ELFOSABI_HPUX,      "UNIX - HP-UX"         },
	{ ELFOSABI_NETBSD,    "UNIX - NetBSD"        },
	{ ELFOSABI_LINUX,     "UNIX - Linux"         },
	{ ELFOSABI_HURD,      "GNU/Hurd"             },
	{ ELFOSABI_SOLARIS,   "UNIX - Solaris"       },
	{ ELFOSABI_AIX,       "UNIX - AIX"           },
	{ ELFOSABI_IRIX,      "UNIX - IRIX"          },
	{ ELFOSABI_FREEBSD,   "UNIX - FreeBSD"       },
	{ ELFOSABI_TRU64,     "UNIX - TRU64"         },
	{ ELFOSABI_MODESTO,   "Novell - Modesto"     },
	{ ELFOSABI_OPENBSD,   "UNIX - OpenBSD"       },
	{ ELFOSABI_OPENVMS,   "VMS - OpenVMS"        },
	{ ELFOSABI_NSK,       "HP - Non-Stop Kernel" },
	{ ELFOSABI_AROS,      "AROS"                 },
	{ ELFOSABI_STANDALONE,"Standalone App"       },
	{ ELFOSABI_ARM,       "ARM"                  },
#endif
};

static const header_item_t header_type_desc[] = {
	{ ET_NONE,  "NONE (None)"             },
#ifdef ALL_ELF
	{ ET_REL,   "REL (Relocatable file)"  },
#endif
	{ ET_EXEC,  "EXEC (Executable file)"  },
#ifdef ALL_ELF
	{ ET_DYN,   "DYN (Shared object file)"},
	{ ET_CORE,  "CORE (Core file)"        },
#endif
};

static const header_item_t header_mach_desc[] = {
	{ EM_NONE,           "None"                                   },
#ifdef ALL_ELF
	{ EM_M32,            "WE32100"                                },
#endif
	{ EM_SPARC,          "Sparc"                                  },
#ifdef ALL_ELF
	{ EM_SPU,            "SPU"                                    },
	{ EM_386,            "Intel 80386"                            },
	{ EM_68K,            "MC68000"                                },
	{ EM_88K,            "MC88000"                                },
	{ EM_486,            "Intel 80486"                            },
	{ EM_860,            "Intel 80860"                            },
	{ EM_MIPS,           "MIPS R3000"                             },
	{ EM_S370,           "IBM System/370"                         },
	{ EM_MIPS_RS3_LE,    "MIPS R4000 big-endian"                  },
	{ EM_OLD_SPARCV9,    "Sparc v9 (old)"                         },
	{ EM_PARISC,         "HPPA"                                   },
	{ EM_PPC_OLD,        "Power PC (old)"                         },
	{ EM_SPARC32PLUS,    "Sparc v8+"                              },
	{ EM_960,            "Intel 90860"                            },
	{ EM_PPC,            "PowerPC"                                },
	{ EM_PPC64,          "PowerPC64"                              },
	{ EM_V800,           "NEC V800"                               },
	{ EM_FR20,           "Fujitsu FR20"                           },
	{ EM_RH32,           "TRW RH32"                               },
	{ EM_MCORE,          "MCORE"                                  },
	{ EM_ARM,            "ARM"                                    },
	{ EM_OLD_ALPHA,      "Digital Alpha (old)"                    },
	{ EM_SH,             "Renesas / SuperH SH"                    },
	{ EM_SPARCV9,        "Sparc v9"                               },
	{ EM_TRICORE,        "Siemens Tricore"                        },
	{ EM_ARC,            "ARC"                                    },
	{ EM_H8_300,         "Renesas H8/300"                         },
	{ EM_H8_300H,        "Renesas H8/300H"                        },
	{ EM_H8S,            "Renesas H8S"                            },
	{ EM_H8_500,         "Renesas H8/500"                         },
	{ EM_IA_64,          "Intel IA-64"                            },
	{ EM_MIPS_X,         "Stanford MIPS-X"                        },
	{ EM_COLDFIRE,       "Motorola Coldfire"                      },
	{ EM_68HC12,         "Motorola M68HC12"                       },
	{ EM_ALPHA,          "Alpha"                                  },
	{ EM_CYGNUS_D10V,    "d10v"                                   },
	{ EM_D10V,           "d10v"                                   },
	{ EM_CYGNUS_D30V,    "d30v"                                   },
	{ EM_D30V,           "d30v"                                   },
	{ EM_CYGNUS_M32R,    "Renesas M32R (formerly Mitsubishi M32r)"},
	{ EM_M32R,           "Renesas M32R (formerly Mitsubishi M32r)"},
	{ EM_CYGNUS_V850,    "MEC v850"                               },
	{ EM_V850,           "NEC v850"                               },
	{ EM_CYGNUS_MN10300, "mn10300"                                },
	{ EM_MN10300,        "mn10300"                                },
	{ EM_CYGNUS_MN10200, "mn10200"                                },
	{ EM_MN10200,        "mn10200"                                },
	{ EM_CYGNUS_FR30,    "Fujitsu FR30"                           },
	{ EM_FR30,           "Fujitsu FR30"                           },
	{ EM_CYGNUS_FRV,     "Fujitsu FR-V"                           },
	{ EM_PJ_OLD,         "picoJava"                               },
	{ EM_PJ,             "picoJava"                               },
	{ EM_MMA,            "Fujitsu Multimedia Accelerator"         },
	{ EM_PCP,            "Siemens PCP"                            },
	{ EM_NCPU,           "Sony nCPU embedded RISC processor"      },
	{ EM_NDR1,           "Denso NDR1 microprocesspr"              },
	{ EM_STARCORE,       "Motorola Star*Core processor"           },
	{ EM_ME16,           "Toyota ME16 processor"                  },
	{ EM_ST100,          "STMicroelectronics ST100 processor"              },
	{ EM_TINYJ,          "Advanced Logic Corp. TinyJ embedded processor"   },
	{ EM_FX66,           "Siemens FX66 microcontroller"                    },
	{ EM_ST9PLUS,        "STMicroelectronics ST9+ 8/16 bit microcontroller"},
	{ EM_ST7,            "STMicroelectronics ST7 8-bit microcontroller"    },
	{ EM_68HC16,         "Motorola MC68HC16 Microcontroller"               },
	{ EM_68HC11,         "Motorola MC68HC11 Microcontroller"               },
	{ EM_68HC08,         "Motorola MC68HC08 Microcontroller"               },
	{ EM_68HC05,         "Motorola MC68HC05 Microcontroller"               },
	{ EM_SVX,            "Silicon Graphics SVx"                            },
	{ EM_ST19,           "STMicroelectronics ST19 8-bit microcontroller"   },
	{ EM_VAX,            "Digital VAX"                                     },
	{ EM_AVR_OLD,        "Atmel AVR 8-bit microcontroller"                 },
	{ EM_AVR,            "Atmel AVR 8-bit microcontroller"                 },
	{ EM_CRIS,           "Axis Communications 32-bit embedded processor"   },
	{ EM_JAVELIN,        "Infineon Technologies 32-bit embedded cpu"       },
	{ EM_FIREPATH,       "Element 14 64-bit DSP processor"                 },
	{ EM_ZSP,            "LSI Logic's 16-bit DSP processor"                },
	{ EM_MMIX,           "Donald Knuth's educational 64-bit processor"     },
	{ EM_HUANY,          "Harvard Universitys's machine-independent object format"},
	{ EM_PRISM,          "Vitesse Prism"                 },
	{ EM_X86_64,         "Advanced Micro Devices X86-64" },
	{ EM_L1OM,           "Intel L1OM"                    },
	{ EM_S390_OLD,       "IBM S/390"                     },
	{ EM_S390,           "IBM S/390"                     },
	{ EM_SCORE,          "SUNPLUS S+Core"                },
	{ EM_XSTORMY16,      "Sanyo Xstormy16 CPU core"      },
	{ EM_OPENRISC,       "OpenRISC"                      },
	{ EM_OR32,           "OpenRISC"                      },
	{ EM_CRX,            "National Semiconductor CRX microprocessor"},
	{ EM_DLX,            "OpenDLX"                                  },
	{ EM_IP2K_OLD,       "Ubicom IP2xxx 8-bit microcontrollers"     },
	{ EM_IP2K,           "Ubicom IP2xxx 8-bit microcontrollers"     },
	{ EM_IQ2000,         "Vitesse IQ2000"                 },
	{ EM_XTENSA_OLD,     "Tensilica Xtensa Processor"     },
	{ EM_XTENSA,         "Tensilica Xtensa Processor"     },
	{ EM_LATTICEMICO32,  "Lattice Mico32"                 },
	{ EM_M32C_OLD,       "Renesas M32c"                   },
	{ EM_M32C,           "Renesas M32c"                   },
	{ EM_MT,             "Morpho Techologies MT processor"},
	{ EM_BLACKFIN,       "Analog Devices Blackfin"        },
	{ EM_NIOS32,         "Altera Nios"                    },
	{ EM_ALTERA_NIOS2,   "Altera Nios II"                 },
	{ EM_XC16X,          "Infineon Technologies xc16x"    },
	{ EM_CYGNUS_MEP,     "Toshiba MeP Media Engine"       },
	{ EM_CR16,           "National Semiconductor's CR16"  },
	{ EM_CR16_OLD,       "National Semiconductor's CR16"  },
#endif
	{ EM_MICROBLAZE,     "Xilinx MicroBlaze"              },
#ifdef ALL_ELF
	{ EM_MICROBLAZE_OLD, "Xilinx MicroBlaze"              },
#endif
};

static void print_header(Elf32_Ehdr *head) {
	int i, x;
	printf("ELF Header:\n");
	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++) {
		printf("%02x ", (int) head->e_ident[i]);
	}
	printf("\n");

	printf("  Class:                             %s\n",
		header_class_desc[head->e_ident[EI_CLASS]].desc);
	printf("  Data:                              %s\n",
		header_data_desc[head->e_ident[EI_DATA]].desc);
	printf("  Version:                           %d %s\n",
		head->e_ident[EI_VERSION],
		(head->e_ident[EI_VERSION] == EV_CURRENT
		? "(current)"
		: (head->e_ident[EI_VERSION] != EV_NONE
		? "unknown"
		: "")));
	printf("  OS/ABI:                            %s\n",
		header_osabi_desc[head->e_ident[EI_OSABI]].desc);
	printf("  ABI Version:                       %d\n",
		head->e_ident[EI_ABIVERSION]);
	printf("  Type:                              %s\n",
		header_type_desc[rev_short(head->e_type, head->e_ident[EI_DATA])]);
	printf("  Machine:                           %s\n",
		header_mach_desc[rev_short(head->e_machine, head->e_ident[EI_DATA])]);

        printf("  Version:                           0x%lx\n",
    		rev_long(head->e_version, head->e_ident[EI_DATA]));
        printf("  Entry point address:               0x%08u\n",
               rev_long(head->e_entry, head->e_ident[EI_DATA]));
        printf("  Start of program headers:          %u (bytes into file)\n",
               rev_long(head->e_phoff, head->e_ident[EI_DATA]));
        printf("  Start of section headers:          %u (bytes into file)\n",
               rev_long(head->e_shoff, head->e_ident[EI_DATA]));
        printf("  Flags:                             0x%u\n",
               rev_long(head->e_flags, head->e_ident[EI_DATA]));
        printf("  Size of this header:               %d (bytes)\n",
               rev_short(head->e_ehsize, head->e_ident[EI_DATA]));
        printf("  Size of program headers:           %d (bytes)\n",
               rev_short(head->e_phentsize, head->e_ident[EI_DATA]));
        printf("  Number of program headers:         %d\n",
               rev_short(head->e_phnum, head->e_ident[EI_DATA]));
        printf("  Size of section headers:           %d (bytes)\n",
               rev_short(head->e_shentsize, head->e_ident[EI_DATA]));
        printf("  Number of section headers:         %d\n",
               rev_short(head->e_shnum, head->e_ident[EI_DATA]));
        printf("  Section header string table index: %d\n",
               rev_short(head->e_shstrndx, head->e_ident[EI_DATA]));
}

static void print_sections(Elf32_Shdr *sections, int8_t *names, int count, uint8_t rev) {
        size_t i;
        if (names == NULL) {
                return;
        }
        printf("Symbol table '.symtab' contains %d entries:\n", count);
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


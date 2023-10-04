/**
 * @file
 * @brief Readelf shows information about ELF binaries.
 *
 * @date 2.05.10
 * @author Anatoly Trosinenko
 *         - Initial implementation.
 *         - Huge parts of code are written by Aleksandr Kirov
 * @author Nikolay Korotky
 *         - Refactoring, fix bugs.
 */

#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <lib/libelf.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* Collection of information about elf. */
typedef struct {
	FILE         *fd;
	/* Should we reverse bytes when reading */
	int          need_reverse;
	/* Header of the file */
	Elf32_Ehdr   header;
	char         **sections;
	/* Section table */
	Elf32_Shdr   sh_table[128];
	/* Program table */
	Elf32_Phdr   ph_table[128];
	/* String table. Contains section names */
	char         *string_table;
	/* Symbol table */
	Elf32_Sym    sym_table[128];
	/* Section header index of the associated string table for symbol table */
	int          sym_names_shidx;
	/* Symbol names */
	char         *sym_names;
	unsigned int sym_count;
	/* Elf .dynamic section */
	Elf32_Dyn    dyn_section[128];
	unsigned int dyn_count;
	/* Elf relocation array */
	Elf32_Rel    rel_array[128];
	unsigned int rel_count;
	Elf32_Addr   base_addr; /* For shared libraries */
} Elf32_Obj;

static int elf_read_section_header_table(int elf_file, Elf32_Obj *elf) { return 0; }
static int elf_read_string_table(int elf_file, Elf32_Obj *elf) { return 0; }
static int elf_read_program_header_table(int elf_file, Elf32_Obj *elf) { return 0; }
static int elf_read_dynamic_section(int elf_file, Elf32_Obj *elf) { return 0; }
static int elf_read_rel_table(int elf_file, Elf32_Obj *elf) { return 0; }
static int elf_read_symbol_table(int elf_file, Elf32_Obj *elf) { return 0; }
static int elf_read_symbol_names(int elf_file, Elf32_Obj *elf) { return 0; }

typedef struct elf_objlist_item {
	Elf32_Obj                *obj;
	struct elf_objlist_item  *next;
} Elf32_Objlist_item;

typedef struct {
	Elf32_Objlist_item *first;
} Elf32_Objlist;

static void elf_initialize_object(Elf32_Obj *obj) {
}

static void print_usage(void) {
	printf("Usage: readelf <option(s)> elf-file>\n");
}

typedef struct header_item {
	int i;
	unsigned char desc[70];
} header_item_t;

/* =========== Print ELF header ============= */

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
	{ ELFOSABI_NONE,       "UNIX - System V"      },
	{ ELFOSABI_HPUX,       "UNIX - HP-UX"         },
	{ ELFOSABI_NETBSD,     "UNIX - NetBSD"        },
	{ ELFOSABI_LINUX,      "UNIX - Linux"         },
	{ ELFOSABI_HURD,       "GNU/Hurd"             },
	{ ELFOSABI_SOLARIS,    "UNIX - Solaris"       },
	{ ELFOSABI_AIX,        "UNIX - AIX"           },
	{ ELFOSABI_IRIX,       "UNIX - IRIX"          },
	{ ELFOSABI_FREEBSD,    "UNIX - FreeBSD"       },
	{ ELFOSABI_TRU64,      "UNIX - TRU64"         },
	{ ELFOSABI_MODESTO,    "Novell - Modesto"     },
	{ ELFOSABI_OPENBSD,    "UNIX - OpenBSD"       },
	{ ELFOSABI_OPENVMS,    "VMS - OpenVMS"        },
	{ ELFOSABI_NSK,        "HP - Non-Stop Kernel" },
	{ ELFOSABI_AROS,       "AROS"                 },
	{ ELFOSABI_STANDALONE, "Standalone App"       },
	{ ELFOSABI_ARM,        "ARM"                  },
};

static const header_item_t header_type_desc[] = {
	{ ET_NONE, "NONE (None)"              },
	{ ET_REL,  "REL (Relocatable file)"   },
	{ ET_EXEC, "EXEC (Executable file)"   },
	{ ET_DYN,  "DYN (Shared object file)" },
	{ ET_CORE, "CORE (Core file)"         },
};

static const header_item_t header_mach_desc[] = {
	{ EM_NONE,           "None"                                   },
	{ EM_M32,            "WE32100"                                },
	{ EM_SPARC,          "Sparc"                                  },
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
	{ EM_PRISM,          "Vitesse Prism"                             },
	{ EM_X86_64,         "Advanced Micro Devices X86-64"             },
	{ EM_L1OM,           "Intel L1OM"                                },
	{ EM_S390_OLD,       "IBM S/390"                                 },
	{ EM_S390,           "IBM S/390"                                 },
	{ EM_SCORE,          "SUNPLUS S+Core"                            },
	{ EM_XSTORMY16,      "Sanyo Xstormy16 CPU core"                  },
	{ EM_OPENRISC,       "OpenRISC"                                  },
	{ EM_OR32,           "OpenRISC"                                  },
	{ EM_CRX,            "National Semiconductor CRX microprocessor" },
	{ EM_DLX,            "OpenDLX"                                   },
	{ EM_IP2K_OLD,       "Ubicom IP2xxx 8-bit microcontrollers"      },
	{ EM_IP2K,           "Ubicom IP2xxx 8-bit microcontrollers"      },
	{ EM_IQ2000,         "Vitesse IQ2000"                            },
	{ EM_XTENSA_OLD,     "Tensilica Xtensa Processor"                },
	{ EM_XTENSA,         "Tensilica Xtensa Processor"                },
	{ EM_LATTICEMICO32,  "Lattice Mico32"                            },
	{ EM_M32C_OLD,       "Renesas M32c"                              },
	{ EM_M32C,           "Renesas M32c"                              },
	{ EM_MT,             "Morpho Techologies MT processor"           },
	{ EM_BLACKFIN,       "Analog Devices Blackfin"                   },
	{ EM_NIOS32,         "Altera Nios"                               },
	{ EM_ALTERA_NIOS2,   "Altera Nios II"                            },
	{ EM_XC16X,          "Infineon Technologies xc16x"               },
	{ EM_CYGNUS_MEP,     "Toshiba MeP Media Engine"                  },
	{ EM_CR16,           "National Semiconductor's CR16"             },
	{ EM_CR16_OLD,       "National Semiconductor's CR16"             },
	{ EM_MICROBLAZE,     "Xilinx MicroBlaze"                         },
	//TODO: we use EM_MICROBLAZE_OLD elf, need for fix.
	{ EM_MICROBLAZE_OLD, "Xilinx MicroBlaze"                         },
};

static void print_header(Elf32_Obj *obj) {
	size_t i;
	Elf32_Ehdr *header = &obj->header;

	printf("ELF Header:\n");
	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++) {
		printf("%02x ", header->e_ident[i]);
	}

	printf("\n  Class:                             %s\n",
		header_class_desc[header->e_ident[EI_CLASS]].desc);
	printf("  Data:                              %s\n",
		header_data_desc[header->e_ident[EI_DATA]].desc);
	printf("  Version:                           %d %s\n",
		header->e_ident[EI_VERSION],
		(header->e_ident[EI_VERSION] == EV_CURRENT ? "(current)"
		: (header->e_ident[EI_VERSION] != EV_NONE	? "unknown" : "")));
	printf("  OS/ABI:                            %s\n",
		header_osabi_desc[header->e_ident[EI_OSABI]].desc);
	printf("  ABI Version:                       %d\n",
			header->e_ident[EI_ABIVERSION]);
	printf("  Type:                              %s\n",
		header_type_desc[header->e_type].desc);
	//TODO: need for fix.
	printf("  Machine:                           %s\n",
		header_mach_desc[header->e_machine].desc);
	printf("  Version:                           0x%x\n",
		header->e_version);
	printf("  Entry point address:               0x%08x\n",
		header->e_entry);
	printf("  Start of program headers:          %u (bytes into file)\n",
		header->e_phoff);
	printf("  Start of section headers:          %u (bytes into file)\n",
		header->e_shoff);
	printf("  Flags:                             0x%u\n",
		header->e_flags);
	printf("  Size of this header:               %d (bytes)\n",
		header->e_ehsize);
	printf("  Size of program headers:           %d (bytes)\n",
		header->e_phentsize);
	printf("  Number of program headers:         %d\n",
		header->e_phnum);
	printf("  Size of section headers:           %d (bytes)\n",
		header->e_shentsize);
	printf("  Number of section headers:         %d\n",
		header->e_shnum);
	printf("  Section header string table index: %d\n",
		header->e_shstrndx);
}

/* ============= Print Sections =============== */

static const header_item_t section_types[] = {
	{ SHT_NULL,        "NULL"     },
	{ SHT_PROGBITS,    "PROGBITS" },
	{ SHT_SYMTAB,      "SYMTAB"   },
	{ SHT_STRTAB,      "STRTAB"   },
	{ SHT_RELA,        "RELA"     },
	{ SHT_HASH,        "HASH"     },
	{ SHT_DYNAMIC,     "DYNAMIC"  },
	{ SHT_NOTE,        "NOTE"     },
	{ SHT_NOBITS,      "NOBITS"   },
	{ SHT_REL,         "REL"      },
	{ SHT_SHLIB,       "SHLIB"    },
	{ SHT_DYNSYM,      "DYNSYM"   },
};

#define SECTION_NAME(sec, string_table) \
	((sec) == NULL ? "<none>"                        \
	: (string_table) == NULL ? "<no-name>"           \
	: (const char*)((string_table) + (sec)->sh_name))

static void print_section_headers(Elf32_Obj *obj) {
	size_t x;
	Elf32_Shdr *sh;
	Elf32_Ehdr *header = &obj->header;

	printf("\nSection Headers:\n");
	printf("  [Nr] Name              Type       Addr     Off    Size   ES Flg Lk Inf Al\n");
	for (int i = 0; i < header->e_shnum; i++) {
		sh = &obj->sh_table[i];
		x = sh->sh_type;
		printf("  [%2d] %-17.17s %-10s %08x %06x %06x %02x  %02x %2d  %2d %2d\n", i,
			SECTION_NAME(sh, obj->string_table),
			section_types[x].desc,
			sh->sh_addr,
			sh->sh_offset,
			sh->sh_size,
			sh->sh_entsize,
			sh->sh_flags,
			sh->sh_link,
			sh->sh_info,
			sh->sh_addralign);
	}
}

/* ======== Print Segments ======== */

static const header_item_t segment_types[] = {
	{ PT_NULL,    "NULL"    },
	{ PT_LOAD,    "LOAD"    },
	{ PT_DYNAMIC, "DYNAMIC" },
	{ PT_INTERP,  "INTERP"  },
	{ PT_NOTE,    "NOTE"    },
	{ PT_SHLIB,   "SHLIB"   },
	{ PT_PHDR,    "PHDR"    },
	{ PT_TLS,     "TLS"     },
};

#define SEGMENT_NAME(type)\
	((x > PT_TLS) ? "<unknown>" : \
	((const char*)segment_types[x].desc))

static void print_program_headers(Elf32_Obj *obj) {
	size_t x;
	Elf32_Phdr *ph;
	Elf32_Ehdr *header = &obj->header;

	printf("\nProgram Headers:\n");
	printf("  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align\n");
	for (int i = 0; i < header->e_phnum; i++) {
		ph = &obj->ph_table[i];
		x = ph->p_type;
		printf("  %-14.14s 0x%06x 0x%08x 0x%08x 0x%05x 0x%05x  %02x 0x%x\n",
			SEGMENT_NAME(segment_types[x].desc),
			ph->p_offset,
			ph->p_vaddr,
			ph->p_paddr,
			ph->p_filesz,
			ph->p_memsz,
			ph->p_flags,
			ph->p_align);
	}
}

#define between(x, a, b) (a <= x && x < b)

static void print_section_to_segment_mapping(Elf32_Obj *obj) {
	Elf32_Shdr *sh;
	Elf32_Phdr *ph;

	printf("\n Section to Segment mapping:\n");
	printf("  Segment Sections...\n");

	for (int i = 0; i < obj->header.e_phnum; i++) {
		printf("   %02d    ", i);

		ph = obj->ph_table + i;
		for (int j = 0; j < obj->header.e_shnum; j++) {
			sh = obj->sh_table + j;
			if ((sh->sh_size)
			&& (between(sh->sh_offset, ph->p_offset, ph->p_offset + ph->p_filesz)
			|| between(sh->sh_offset + sh->sh_size - 1, ph->p_offset, ph->p_offset + ph->p_filesz))) {
				printf(" %s", (const char *) obj->string_table + sh->sh_name);
			}
		}

		printf("\n");
	}
}

/* ======== Print dynamic section ======== */

static void print_dynamic_section(Elf32_Obj *obj) {
	Elf32_Dyn *dyn;

	printf("\nDynamic section at offset ... contains %d entries:\n", obj->dyn_count);

	printf("  Tag        Type                         Name/Value\n");
	for (int i = 0; i < obj->dyn_count; i++) {
		dyn = &obj->dyn_section[i];
		printf(" 0x%08x %x\n",
				(unsigned int) dyn->d_tag,
				dyn->d_un.d_ptr);
	}
}


/* ============== Print relocations =========== */

static void print_relocations(Elf32_Obj *obj) {
	Elf32_Rel *rel_array = obj->rel_array;

	if (!rel_array || !obj->rel_count) {
		printf("There are no relocations in this file.\n");
	} else {
		//printf("Relocation section '%s' at offset 0x%x contains %d entries:\n");

		printf("\nRelocations:\n");

		printf(" Offset      Info    Sym. Name\n");
		for (int i = 0; i < obj->rel_count; i++) {
			int sym_table_i = ELF32_R_SYM(rel_array[i].r_info);

			printf("%08x  %08x   %s\n",
					rel_array[i].r_offset,
					rel_array[i].r_info,\
					obj->sym_names + obj->sym_table[sym_table_i].st_name);
		}
	}
}

/* ============== Print symbol table ======== */

static const header_item_t symb_types[] = {
	{ STT_NOTYPE,   "NOTYPE"  },
	{ STT_OBJECT,   "OBJECT"  },
	{ STT_FUNC,     "FUNC"    },
	{ STT_SECTION,  "SECTION" },
	{ STT_FILE,     "FILE"    },
	{ STT_COMMON,   "COMMON"  },
	{ STT_TLS,      "TLS"     },
	{ STT_RELC,     "RELC"    },
	{ STT_SRELC,    "SRELC"   },
};

static const header_item_t symb_binds[] = {
	{ STB_LOCAL,  "LOCAL"  },
	{ STB_GLOBAL, "GLOBAL" },
	{ STB_WEAK,   "WEAK"   },
};

static const header_item_t symb_vis[] = {
	{ STV_DEFAULT,   "DEFAULT"   },
	{ STV_INTERNAL,  "INTERNAL"  },
	{ STV_HIDDEN,    "HIDDEN"    },
	{ STV_PROTECTED, "PROTECTED" },
};

static void print_symbols(Elf32_Obj *obj, int counter) {
	size_t t, b, v;
    Elf32_Sym *sym;

    printf("Symbol table '%s' contains %d entries:\n",
    		obj->sym_names + obj->sym_table->st_name, counter);

    printf("   Num:    Value  Size Type    Bind   Vis       Ndx Name\n");
    for (int i = 0; i < counter; i++) {
		sym = obj->sym_table + i;

		t = ELF32_ST_TYPE(sym->st_info);
		b = ELF32_ST_BIND(sym->st_info);
		v = ELF32_ST_VISIBILITY(sym->st_other);

		printf("    %2d: %-8x  %4d %-7s %-6s %-7s %5d %s\n", i,
			sym->st_value,
			sym->st_size,
			symb_types[t].desc,
			symb_binds[b].desc,
			symb_vis[v].desc,
			obj->sym_table[i].st_shndx,
			obj->sym_names + sym->st_name);
	}
}

/* WARNING: overly complex function. */
int main(int argc, char **argv) {
	int show_head     = 0;
	int show_sections = 0;
	int show_segments = 0;
	int show_reloc    = 0;
	int show_symb     = 0;
	int show_dyn      = 0;
	static Elf32_Obj  elf;
	int opt, err, cnt = 0;
	int elf_file;
	int symb_count;

	elf_initialize_object(&elf);

	do {
		cnt++;
		opt = getopt(argc - 1, argv, "hHSlrs");
		switch (opt) {
		case 'h':
			show_head = 1;
			break;
		case 'S':
			show_sections = 1;
			break;
		case 'l':
			show_segments = 1;
			break;
		case 'r':
			show_reloc = 1;
			break;
		case 's':
			show_symb = 1;
			break;
		case 'd':
			show_dyn = 1;
			break;
		case 'H':
			print_usage();
			return 0;
		case -1:
			cnt -= 1;
			break;
		default:
			print_usage();
			return 1;
		}
	} while (opt != -1);

	if (cnt == 0) {
		show_head = 1;
		show_sections = 1;
		show_segments = 1;
		show_reloc = 1;
		show_symb = 1;
		show_dyn = 1;
	}

	elf_file = open(argv[argc - 1], O_RDONLY);

	if (elf_file < 0) {
		printf("Cannot open file %s\n", argv[argc - 1]);
		return -errno;
	}

	if ((err = elf_read_header(elf_file, &elf.header)) < 0) {
		close(elf_file);
		return err;
	}

	if (show_sections || show_segments || show_reloc || show_symb || show_dyn) {
		if ((err = elf_read_section_header_table(elf_file, &elf)) < 0) {
			printf("Cannot read sections table: %d\n", err);
			show_sections = 0;
			show_reloc = 0;
			show_symb = 0;
		}
	}

	if (show_sections || show_segments || show_symb || show_dyn) {
		if ((err = elf_read_string_table(elf_file, &elf)) < 0) {
			printf("Cannot read string table: %d\n", err);
			show_sections = 0;
		}
	}

	if (show_segments) {
		if ((err = elf_read_program_header_table(elf_file, &elf)) < 0) {
			printf("Cannot read segments table: %d\n", err);
			show_segments = 0;
		}
	}

	if (show_dyn) {
		if ((err = elf_read_dynamic_section(elf_file, &elf)) < 0) {
			printf("Cannot read segments table: %d\n", err);
			show_dyn = 0;
		}
	}

	if (show_reloc) {
		if ((err = elf_read_rel_table(elf_file, &elf)) < 0) {
			printf("Cannot read rel table: %d\n", err);
			show_reloc = 0;
		}
	}

	if (show_symb) {
		if ((symb_count = elf_read_symbol_table(elf_file, &elf)) < 0) {
			printf("Cannot read symbol table: %d\n", err);
			show_symb = 0;
		}
		if ((err = elf_read_symbol_names(elf_file, &elf)) < 0) {
			printf("Cannot read symbol names: %d\n", err);
		}
	}

	if (show_head) {
		print_header(&elf);
	}

	if (show_sections) {
		print_section_headers(&elf);
	}

	if (show_segments) {
		print_program_headers(&elf);
		print_section_to_segment_mapping(&elf);
	}

	if (show_dyn) {
		print_dynamic_section(&elf);
	}

	if (show_reloc) {
		print_relocations(&elf);
	}
	if (show_symb) {
		print_symbols(&elf, symb_count);
	}

	close(elf_file);

	/*elf_finilize_object(&elf);*/

	return 0;
}

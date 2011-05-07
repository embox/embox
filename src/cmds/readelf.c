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

#include <embox/cmd.h>
#include <getopt.h>
#include <ctype.h>
#include <lib/libelf.h>
#include <string.h>
#include <errno.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: readelf <option(s)> elf-file>\n");
}

typedef struct header_item {
	int i;
	unsigned char desc[70];
} header_item_t;

/** Byte ret specifies the data encoding of the
 * processor-specific data in the object file. */
static uint8_t rev;

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

static void print_header(Elf32_Ehdr *head) {
	size_t i;
	printf("ELF Header:\n");
	printf("  Magic:   ");
	for (i = 0; i < EI_NIDENT; i++) {
		printf("%02x ", (int)head->e_ident[i]);
	}

	printf("\n  Class:                             %s\n",
		header_class_desc[head->e_ident[EI_CLASS]].desc);
	printf("  Data:                              %s\n",
		header_data_desc[head->e_ident[EI_DATA]].desc);
	printf("  Version:                           %d %s\n",
		head->e_ident[EI_VERSION],
		(head->e_ident[EI_VERSION] == EV_CURRENT ? "(current)"
		: (head->e_ident[EI_VERSION] != EV_NONE	? "unknown" : "")));
	printf("  OS/ABI:                            %s\n",
		header_osabi_desc[head->e_ident[EI_OSABI]].desc);
	printf("  ABI Version:                       %d\n",
		head->e_ident[EI_ABIVERSION]);
	printf("  Type:                              %s\n",
		header_type_desc[S_REV(head->e_type, rev)].desc);
	//TODO: need for fix.
	printf("  Machine:                           %s\n",
		header_mach_desc[S_REV(head->e_machine, rev)].desc);
	printf("  Version:                           0x%x\n",
		L_REV(head->e_version, rev));
	printf("  Entry point address:               0x%08x\n",
		L_REV(head->e_entry, rev));
	printf("  Start of program headers:          %u (bytes into file)\n",
		L_REV(head->e_phoff, rev));
	printf("  Start of section headers:          %u (bytes into file)\n",
		L_REV(head->e_shoff, rev));
	printf("  Flags:                             0x%u\n",
		L_REV(head->e_flags, rev));
	printf("  Size of this header:               %d (bytes)\n",
		S_REV(head->e_ehsize, rev));
	printf("  Size of program headers:           %d (bytes)\n",
		S_REV(head->e_phentsize, rev));
	printf("  Number of program headers:         %d\n",
		S_REV(head->e_phnum, rev));
	printf("  Size of section headers:           %d (bytes)\n",
		S_REV(head->e_shentsize, rev));
	printf("  Number of section headers:         %d\n",
		S_REV(head->e_shnum, rev));
	printf("  Section header string table index: %d\n",
		S_REV(head->e_shstrndx, rev));
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

static void print_sections(Elf32_Ehdr *head,
			Elf32_Shdr *sections, int8_t *string_table) {
	size_t i, x;
	Elf32_Shdr *sec;
	printf("There are %d section headers, starting at offset 0x%x:\n",
		head->e_shnum, head->e_shoff);
	printf("\nSection Headers:\n");
	printf("  [Nr] Name\t\tType       Addr     Off    Size   ES Flg Lk Inf Al\n");
	for (i = 0; i < S_REV(head->e_shnum, rev); i++) {
		sec = &sections[i];
		x = L_REV(sec->sh_type, rev);
		printf("  [%2d] %-16s %-10s %08x %06x %06x %02x  %02x %2d  %2d %2d\n", i,
			SECTION_NAME(&sections[i], string_table),
			section_types[x].desc,
			L_REV(sec->sh_addr,      rev),
			L_REV(sec->sh_offset,    rev),
			L_REV(sec->sh_size,      rev),
			L_REV(sec->sh_entsize,   rev),
			L_REV(sec->sh_flags,     rev),
			L_REV(sec->sh_link,      rev),
			L_REV(sec->sh_info,      rev),
			L_REV(sec->sh_addralign, rev));
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

static void print_segments(Elf32_Ehdr *head, Elf32_Phdr *segments) {
	size_t i, x;
	Elf32_Phdr *seg;
	printf("There are %d program headers, starting at offset %d\n",
		S_REV(head->e_phnum, rev), head->e_phoff);
	printf("Program Headers:\n");
	printf("  Type\tOffset  \tVirtAddr\tPhysAddr\tFileSiz\tMemSiz\tFlg\tAlign\n");
	for (i = 0; i < S_REV(head->e_phnum, rev); i++) {
		seg = &segments[i];
		x = L_REV(seg->p_type, rev);
		printf("  %s\t0x%08x\t0x%08x\t0x%08x\t0x%05x\t0x%05x\t0x%x\t0x%x\n",
			segment_types[x].desc,
			L_REV(seg->p_offset, rev),
			L_REV(seg->p_vaddr,  rev),
			L_REV(seg->p_paddr,  rev),
			L_REV(seg->p_filesz, rev),
			L_REV(seg->p_memsz,  rev),
			L_REV(seg->p_flags,  rev),
			L_REV(seg->p_align,  rev));
	}
}

/* ============== Print relocations =========== */

static void print_relocations(Elf32_Rel *rel_array, int count) {
	size_t i;
//	printf("Relocation section '%s' at offset 0x%x contains %d entries:\n");
	if (count == 0) {
		printf("There are no relocations in this file.\n");
	}
	for (i = 0; i < count; i++) {
		printf("\nr_offset : %u", L_REV(rel_array[i].r_offset, rev));
		printf("\nr_info : %u\n", L_REV(rel_array[i].r_info, rev));
	}
}

/* ============== Print symbol table ========== */

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

static void print_symb(Elf32_Sym *symb, int8_t *string_table, int counter) {
        size_t i, t, b, v;
        Elf32_Sym *smb;
        printf("Symbol table '%s' contains %d entries:\n",
    		string_table + symb->st_name, counter);
        printf("   Num:    Value  Size Type    Bind   Vis       Ndx Name\n");
	for (i = 0; i < counter; i++) {
		smb = &symb[i];
		t = ELF32_ST_TYPE(smb->st_info);
		b = ELF32_ST_BIND(smb->st_info);
		v = ELF32_ST_VISIBILITY(smb->st_other);
		printf("    %2d: %-8x  %4d %-7s %-6s %-7s %5d %s\n", i,
			L_REV(smb->st_value, rev),
			L_REV(smb->st_size, rev),
			symb_types[t].desc,
			symb_binds[b].desc,
			symb_vis[v].desc,
			S_REV(symb[i].st_shndx, rev),
			&(string_table[L_REV(smb->st_name, rev)]));
	}
}

/* WARNING: overly complex function. */
static int exec(int argc, char **argv) {
	int show_head     = 0;
	int show_sections = 0;
	int show_segments = 0;
	int show_reloc    = 0;
	int show_symb     = 0;
	Elf32_Ehdr elf_header;
	Elf32_Shdr section_headers[MAX_NUMBER_OF_SECTIONS];
	Elf32_Phdr program_headers[MAX_SEGMENTS];
	Elf32_Rel  rel[MAX_REL_ARRAY_LENGTH];
	Elf32_Sym  dynamic_symbols[MAX_SYMB];
	int8_t     string_table[MAX_NAME_LENGTH];
	int8_t     symb_names[MAX_SYMB_NAMES];

	int opt, err, cnt = 0;
	FILE *elf_file;
	int rel_count, symb_count, symb_names_l;

	getopt_init();
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
	}

	elf_file = fopen(argv[argc - 1], "r");
	if (elf_file == NULL) {
		printf("Cannot open file %s\n", argv[argc - 1]);
		return -1;
	}
	if ((err = elf_read_header(elf_file, &elf_header)) < 0) {
		return -1;
	}
	rev = elf_header.e_ident[EI_DATA];

	if (show_sections || show_reloc || show_symb) {
		if ((err = elf_read_sections_table(elf_file, &elf_header,
						    section_headers)) < 0) {
			printf("Cannot read sections table: %d\n", err);
			show_sections = 0;
			show_reloc = 0;
			show_symb = 0;
		}
	}
	if (show_sections || show_symb) {
		if ((err = elf_read_string_table(elf_file, &elf_header,
		    section_headers, string_table)) < 0) {
			printf("Cannot read string table: %d\n", err);
			show_sections = 0;
		}
	}
	if (show_segments) {
		if ((err = elf_read_segments_table(elf_file, &elf_header,
							program_headers)) < 0) {
			printf("Cannot read segments table: %d\n", err);
			show_segments = 0;
		}
	}
	if (show_reloc) {
		if ((err = elf_read_rel_table(elf_file, &elf_header,
				section_headers, rel, &rel_count)) < 0) {
			printf("Cannot read rel table: %d\n", err);
			show_reloc = 0;
		}
	}
	if (show_symb) {
		if ((err = elf_read_symbol_table(elf_file, &elf_header,
		    section_headers, dynamic_symbols, &symb_count)) < 0) {
			printf("Cannot read symbol table: %d\n", err);
			show_symb = 0;
		}
		if ((err = elf_read_symbol_string_table(elf_file, &elf_header,
		    section_headers, string_table, symb_names, &symb_names_l)) < 0) {
			printf("Cannot read symbol names: %d\n", err);
			symb_names_l = 0;
		}
	}

	if (show_head) {
		print_header(&elf_header);
	}

	if (show_sections) {
		print_sections(&elf_header, section_headers, string_table);
	}

	if (show_segments) {
		print_segments(&elf_header, program_headers);
	}
	if (show_reloc) {
		print_relocations(rel, rel_count);
	}
	if (show_symb) {
		print_symb(dynamic_symbols,
			(symb_names_l != 0) ? symb_names : NULL, symb_count);
	}
	fclose(elf_file);
	return 0;
}

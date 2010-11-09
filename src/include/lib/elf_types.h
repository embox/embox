/**
 * @file
 * @brief Representation of ELF data types.
 *
 * @date 28.10.10
 * @author Nikolay Korotky
 */

#ifndef LIB_ELF_TYPES_H_
#define LIB_ELF_TYPES_H_

#include <stdio.h>
#include <types.h>
#include <errno.h>

#define __libelf_u32_t    __u32
#define __libelf_u16_t    __u16
#define __libelf_i32_t    __s32

typedef __libelf_u32_t    Elf32_Addr;
typedef __libelf_u16_t    Elf32_Half;
typedef __libelf_u32_t    Elf32_Off;
typedef __libelf_i32_t    Elf32_Sword;
typedef __libelf_u32_t    Elf32_Word;

/**
 * Until dynamic memory applied array length constrain used
 */
#define MAX_NUMBER_OF_SECTIONS          100
#define MAX_STRING_TABLE_LENGTH         500
#define MAX_SYMBOL_TABLE_LENGTH         3000
#define MAX_SYMBOL_STRING_TABLE_LENGTH  50000
#define MAX_REL_ARRAY_LENGTH            10000
#define MAX_RELA_ARRAY_LENGTH           10000
#define MAX_NAME_LENGTH                 150

#define MAX_SEGMENTS     100
#define MAX_SYMB         1000
#define MAX_SYMB_NAMES   100000

/**
 * Length in bytes of main field in Elf-header structure
 */
#define EI_NIDENT           16

/**
 * Code of information in symbols descriptions. Watch Elf-specification.
 */
#define ELF32_ST_BIND(i)       ((i) >> 4)

/**
 * Code of information in symbols descriptions. Watch Elf-specification.
 */
#define ELF32_ST_TYPE(i)       ((i) & 0xf)

#define ELF32_ST_VISIBILITY(v) ((v) & 0x3)

/**
 * Code of information in symbols descriptions. Watch Elf-specification.
 */
#define ELF32_ST_INFO(b,t)     (((b) << 4) + ((t) & 0xf))

/**
 * Code of information in Relocation table. Watch Elf-specification.
 */
#define ELF32_R_SYM(i)         ((i) >> 8)

/**
 * Code of information in Relocation table. Watch Elf-specification.
 */
#define ELF32_R_TYPE(i)     ((unsigned char) (i))

/**
 * Code of information in Relocation table. Watch Elf-specification.
 */
#define ELF32_R_INFO(s,t)   (((s) << 8) + (unsigned char)(t))

/**
 * p_type
 */
#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_TLS          7
#define PT_NUM          8
#define PT_LOOS         0x60000000
#define PT_HIOS         0x6fffffff
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7fffffff

/**
 * ELF Header. Code style from specification.
 */
typedef struct {
	uint8_t    e_ident[EI_NIDENT];
	Elf32_Half e_type;
	Elf32_Half e_machine;
	Elf32_Word e_version;
	Elf32_Addr e_entry;
	Elf32_Off  e_phoff;
	Elf32_Off  e_shoff;
	Elf32_Word e_flags;
	Elf32_Half e_ehsize;
	/**
	 * This member holds the size in bytes of one entry in the
	 * file’s program header table; all entries are the same size.
	 */
	Elf32_Half e_phentsize;
	Elf32_Half e_phnum;
	Elf32_Half e_shentsize;
	Elf32_Half e_shnum;
	Elf32_Half e_shstrndx;
} Elf32_Ehdr;

/*
 * e_ident
 */
#define EI_MAG0         0
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_OSABI        7
#define EI_ABIVERSION   8
#define EI_PAD          9

#define ELFMAG0         0x7f
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'

/*
 * e_ident[EI_CLASS]
 */
#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFCLASSNUM     3

/*
 * e_ident[EI_DATA]
 */
#define ELFDATANONE     0
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2
#define ELFDATANUM      3

/*
 * e_ident[EI_OSABI]
 */
#define ELFOSABI_NONE         0 /* UNIX System V ABI */
#define ELFOSABI_HPUX         1 /* HP-UX operating system */
#define ELFOSABI_NETBSD       2 /* NetBSD */
#define ELFOSABI_LINUX        3 /* GNU/Linux */
#define ELFOSABI_HURD         4 /* GNU/Hurd */
#define ELFOSABI_SOLARIS      6 /* Solaris */
#define ELFOSABI_AIX          7 /* AIX */
#define ELFOSABI_IRIX         8 /* IRIX */
#define ELFOSABI_FREEBSD      9 /* FreeBSD */
#define ELFOSABI_TRU64       10 /* TRU64 UNIX */
#define ELFOSABI_MODESTO     11 /* Novell Modesto */
#define ELFOSABI_OPENBSD     12 /* OpenBSD */
#define ELFOSABI_OPENVMS     13 /* OpenVMS */
#define ELFOSABI_NSK         14 /* Hewlett-Packard Non-Stop Kernel */
#define ELFOSABI_AROS        15 /* AROS */
#define ELFOSABI_ARM         97 /* ARM */
#define ELFOSABI_STANDALONE 255 /* Standalone (embedded) application */

/*
 * e_type
 */
#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4

/*
 * e_machine
 */
#define EM_NONE           0     /* No machine */
#define EM_M32            1     /* AT&T WE 32100 */
#define EM_SPARC          2     /* SUN Sparc */
#define EM_386            3     /* Intel 80386 */
#define EM_68K            4     /* Motorola m68k family */
#define EM_88K            5     /* Motorola m88k family */
#define EM_486            6     /* Intel 80486 *//* Reserved for future use */
#define EM_860            7     /* Intel 80860 */
#define EM_MIPS           8     /* MIPS R3000 (officially, big-endian only) */
#define EM_S370           9     /* IBM System/370 */
#define EM_MIPS_RS3_LE   10     /* MIPS R3000 little-endian (Oct 4 1999 Draft) Deprecated */
#define EM_res011        11     /* Reserved */
#define EM_res012        12     /* Reserved */
#define EM_res013        13     /* Reserved */
#define EM_res014        14     /* Reserved */
#define EM_PARISC        15     /* HPPA */
#define EM_res016        16     /* Reserved */
#define EM_VPP550        17     /* Fujitsu VPP500 */
#define EM_SPARC32PLUS   18     /* Sun's "v8plus" */
#define EM_960           19     /* Intel 80960 */
#define EM_PPC           20     /* PowerPC */
#define EM_PPC64         21     /* 64-bit PowerPC */
#define EM_S390          22     /* IBM S/390 */
#define EM_SPU           23     /* Sony/Toshiba/IBM SPU */
#define EM_res024        24     /* Reserved */
#define EM_res025        25     /* Reserved */
#define EM_res026        26     /* Reserved */
#define EM_res027        27     /* Reserved */
#define EM_res028        28     /* Reserved */
#define EM_res029        29     /* Reserved */
#define EM_res030        30     /* Reserved */
#define EM_res031        31     /* Reserved */
#define EM_res032        32     /* Reserved */
#define EM_res033        33     /* Reserved */
#define EM_res034        34     /* Reserved */
#define EM_res035        35     /* Reserved */
#define EM_V800          36     /* NEC V800 series */
#define EM_FR20          37     /* Fujitsu FR20 */
#define EM_RH32          38     /* TRW RH32 */
#define EM_MCORE         39     /* Motorola M*Core */ /* May also be taken by Fujitsu MMA */
#define EM_RCE           39     /* Old name for MCore */
#define EM_ARM           40     /* ARM */
#define EM_OLD_ALPHA     41     /* Digital Alpha */
#define EM_SH            42     /* Renesas (formerly Hitachi) / SuperH SH */
#define EM_SPARCV9       43     /* SPARC v9 64-bit */
#define EM_TRICORE       44     /* Siemens Tricore embedded processor */
#define EM_ARC           45     /* ARC Cores */
#define EM_H8_300        46     /* Renesas (formerly Hitachi) H8/300 */
#define EM_H8_300H       47     /* Renesas (formerly Hitachi) H8/300H */
#define EM_H8S           48     /* Renesas (formerly Hitachi) H8S */
#define EM_H8_500        49     /* Renesas (formerly Hitachi) H8/500 */
#define EM_IA_64         50     /* Intel IA-64 Processor */
#define EM_MIPS_X        51     /* Stanford MIPS-X */
#define EM_COLDFIRE      52     /* Motorola Coldfire */
#define EM_68HC12        53     /* Motorola M68HC12 */
#define EM_MMA           54     /* Fujitsu Multimedia Accelerator */
#define EM_PCP           55     /* Siemens PCP */
#define EM_NCPU          56     /* Sony nCPU embedded RISC processor */
#define EM_NDR1          57     /* Denso NDR1 microprocesspr */
#define EM_STARCORE      58     /* Motorola Star*Core processor */
#define EM_ME16          59     /* Toyota ME16 processor */
#define EM_ST100         60     /* STMicroelectronics ST100 processor */
#define EM_TINYJ         61     /* Advanced Logic Corp. TinyJ embedded processor */
#define EM_X86_64        62     /* Advanced Micro Devices X86-64 processor */
#define EM_PDSP          63     /* Sony DSP Processor */
#define EM_PDP10         64     /* Digital Equipment Corp. PDP-10 */
#define EM_PDP11         65     /* Digital Equipment Corp. PDP-11 */
#define EM_FX66          66     /* Siemens FX66 microcontroller */
#define EM_ST9PLUS       67     /* STMicroelectronics ST9+ 8/16 bit microcontroller */
#define EM_ST7           68     /* STMicroelectronics ST7 8-bit microcontroller */
#define EM_68HC16        69     /* Motorola MC68HC16 Microcontroller */
#define EM_68HC11        70     /* Motorola MC68HC11 Microcontroller */
#define EM_68HC08        71     /* Motorola MC68HC08 Microcontroller */
#define EM_68HC05        72     /* Motorola MC68HC05 Microcontroller */
#define EM_SVX           73     /* Silicon Graphics SVx */
#define EM_ST19          74     /* STMicroelectronics ST19 8-bit cpu */
#define EM_VAX           75     /* Digital VAX */
#define EM_CRIS          76     /* Axis Communications 32-bit embedded processor */
#define EM_JAVELIN       77     /* Infineon Technologies 32-bit embedded cpu */
#define EM_FIREPATH      78     /* Element 14 64-bit DSP processor */
#define EM_ZSP           79     /* LSI Logic's 16-bit DSP processor */
#define EM_MMIX          80     /* Donald Knuth's educational 64-bit processor */
#define EM_HUANY         81     /* Harvard's machine-independent format */
#define EM_PRISM         82     /* SiTera Prism */
#define EM_AVR           83     /* Atmel AVR 8-bit microcontroller */
#define EM_FR30          84     /* Fujitsu FR30 */
#define EM_D10V          85     /* Mitsubishi D10V */
#define EM_D30V          86     /* Mitsubishi D30V */
#define EM_V850          87     /* NEC v850 */
#define EM_M32R          88     /* Renesas M32R (formerly Mitsubishi M32R) */
#define EM_MN10300       89     /* Matsushita MN10300 */
#define EM_MN10200       90     /* Matsushita MN10200 */
#define EM_PJ            91     /* picoJava */
#define EM_OPENRISC      92     /* OpenRISC 32-bit embedded processor */
#define EM_ARC_A5        93     /* ARC Cores Tangent-A5 */
#define EM_XTENSA        94     /* Tensilica Xtensa Architecture */
#define EM_VIDEOCORE     95     /* Alphamosaic VideoCore processor */
#define EM_TMM_GPP       96     /* Thompson Multimedia General Purpose Processor */
#define EM_NS32K         97     /* National Semiconductor 32000 series */
#define EM_TPC           98     /* Tenor Network TPC processor */
#define EM_SNP1K         99     /* Trebia SNP 1000 processor */
#define EM_ST200        100     /* STMicroelectronics ST200 microcontroller */
#define EM_IP2K         101     /* Ubicom IP2022 micro controller */
#define EM_MAX          102     /* MAX Processor */
#define EM_CR           103     /* National Semiconductor CompactRISC */
#define EM_F2MC16       104     /* Fujitsu F2MC16 */
#define EM_MSP430       105     /* TI msp430 micro controller */
#define EM_BLACKFIN     106     /* ADI Blackfin */
#define EM_SE_C33       107     /* S1C33 Family of Seiko Epson processors */
#define EM_SEP          108     /* Sharp embedded microprocessor */
#define EM_ARCA         109     /* Arca RISC Microprocessor */
#define EM_UNICORE      110     /* Microprocessor series from PKU-Unity Ltd. and MPRC of Peking University */
#define EM_EXCESS       111     /* eXcess: 16/32/64-bit configurable embedded CPU */
#define EM_DXP          112     /* Icera Semiconductor Inc. Deep Execution Processor */
#define EM_ALTERA_NIOS2 113     /* Altera Nios II soft-core processor */
#define EM_CRX          114     /* National Semiconductor CRX */
#define EM_XGATE        115     /* Motorola XGATE embedded processor */
#define EM_C166         116     /* Infineon C16x/XC16x processor */
#define EM_M16C         117     /* Renesas M16C series microprocessors */
#define EM_DSPIC30F     118     /* Microchip Technology dsPIC30F Digital Signal Controller */
#define EM_CE           119     /* Freescale Communication Engine RISC core */
#define EM_M32C         120     /* Renesas M32C series microprocessors */
#define EM_res121       121     /* Reserved */
#define EM_res122       122     /* Reserved */
#define EM_res123       123     /* Reserved */
#define EM_res124       124     /* Reserved */
#define EM_res125       125     /* Reserved */
#define EM_res126       126     /* Reserved */
#define EM_res127       127     /* Reserved */
#define EM_res128       128     /* Reserved */
#define EM_res129       129     /* Reserved */
#define EM_res130       130     /* Reserved */
#define EM_TSK3000      131     /* Altium TSK3000 core */
#define EM_RS08         132     /* Freescale RS08 embedded processor */
#define EM_res133       133     /* Reserved */
#define EM_ECOG2        134     /* Cyan Technology eCOG2 microprocessor */
#define EM_SCORE        135     /* Sunplus Score */
#define EM_SCORE7       135     /* Sunplus S+core7 RISC processor */
#define EM_DSP24        136     /* New Japan Radio (NJR) 24-bit DSP Processor */
#define EM_VIDEOCORE3   137     /* Broadcom VideoCore III processor */
#define EM_LATTICEMICO32 138    /* RISC processor for Lattice FPGA architecture */
#define EM_SE_C17       139     /* Seiko Epson C17 family */
#define EM_res140       140     /* Reserved */
#define EM_res141       141     /* Reserved */
#define EM_res142       142     /* Reserved */
#define EM_res143       143     /* Reserved */
#define EM_res144       144     /* Reserved */
#define EM_res145       145     /* Reserved */
#define EM_res146       146     /* Reserved */
#define EM_res147       147     /* Reserved */
#define EM_res148       148     /* Reserved */
#define EM_res149       149     /* Reserved */
#define EM_res150       150     /* Reserved */
#define EM_res151       151     /* Reserved */
#define EM_res152       152     /* Reserved */
#define EM_res153       153     /* Reserved */
#define EM_res154       154     /* Reserved */
#define EM_res155       155     /* Reserved */
#define EM_res156       156     /* Reserved */
#define EM_res157       157     /* Reserved */
#define EM_res158       158     /* Reserved */
#define EM_res159       159     /* Reserved */
#define EM_MMDSP_PLUS   160     /* STMicroelectronics 64bit VLIW Data Signal Processor */
#define EM_CYPRESS_M8C  161     /* Cypress M8C microprocessor */
#define EM_R32C         162     /* Renesas R32C series microprocessors */
#define EM_TRIMEDIA     163     /* NXP Semiconductors TriMedia architecture family */
#define EM_QDSP6        164     /* QUALCOMM DSP6 Processor */
#define EM_8051         165     /* Intel 8051 and variants */
#define EM_STXP7X       166     /* STMicroelectronics STxP7x family */
#define EM_NDS32        167     /* Andes Technology compact code size embedded RISC processor family */
#define EM_ECOG1        168     /* Cyan Technology eCOG1X family */
#define EM_ECOG1X       168     /* Cyan Technology eCOG1X family */
#define EM_MAXQ30       169     /* Dallas Semiconductor MAXQ30 Core Micro-controllers */
#define EM_XIMO16       170     /* New Japan Radio (NJR) 16-bit DSP Processor */
#define EM_MANIK        171     /* M2000 Reconfigurable RISC Microprocessor */
#define EM_CRAYNV2      172     /* Cray Inc. NV2 vector architecture */
#define EM_RX           173     /* Renesas RX family */
#define EM_METAG        174     /* Imagination Technologies META processor architecture */
#define EM_MCST_ELBRUS  175     /* MCST Elbrus general purpose hardware architecture */
#define EM_ECOG16       176     /* Cyan Technology eCOG16 family */
#define EM_CR16         177     /* National Semiconductor CompactRISC 16-bit processor */
#define EM_ETPU         178     /* Freescale Extended Time Processing Unit */
#define EM_SLE9X        179     /* Infineon Technologies SLE9X core */
#define EM_L1OM         180     /* Intel L1OM */
#define EM_INTEL181     181     /* Reserved by Intel */
#define EM_INTEL182     182     /* Reserved by Intel */
#define EM_res183       183     /* Reserved by ARM */
#define EM_res184       184     /* Reserved by ARM */
#define EM_AVR32        185     /* Atmel Corporation 32-bit microprocessor family */
#define EM_STM8 186     /* STMicroeletronics STM8 8-bit microcontroller */
#define EM_TILE64       187     /* Tilera TILE64 multicore architecture family */
#define EM_TILEPRO      188     /* Tilera TILEPro multicore architecture family */
#define EM_MICROBLAZE   189     /* Xilinx MicroBlaze 32-bit RISC soft processor core */

/* If it is necessary to assign new unofficial EM_* values, please pick large
   random numbers (0x8523, 0xa7f2, etc.) to minimize the chances of collision
   with official or non-GNU unofficial values.

   NOTE: Do not just increment the most recent number by one.
   Somebody else somewhere will do exactly the same thing, and you
   will have a collision.  Instead, pick a random number.

   Normally, each entity or maintainer responsible for a machine with an
   unofficial e_machine number should eventually ask registry@sco.com for
   an officially blessed number to be added to the list above.  */

/* Old version of Sparc v9, from before the ABI;
   This should be removed shortly.  */
#define EM_OLD_SPARCV9          11

/* Old version of PowerPC, this should be removed shortly. */
#define EM_PPC_OLD              17

/* picoJava */
#define EM_PJ_OLD               99

/* Old, unofficial value for National Semiconductor CompactRISC - CR16 */
#define EM_CR16_OLD             115

/* AVR magic number.  Written in the absense of an ABI.  */
#define EM_AVR_OLD              0x1057

/* MSP430 magic number.  Written in the absense of everything.  */
#define EM_MSP430_OLD           0x1059

/* Morpho MT.   Written in the absense of an ABI.  */
#define EM_MT                   0x2530

/* FR30 magic number - no EABI available.  */
#define EM_CYGNUS_FR30          0x3330

/* OpenRISC magic number.  Written in the absense of an ABI.  */
#define EM_OPENRISC_OLD         0x3426

/* DLX magic number.  Written in the absense of an ABI.  */
#define EM_DLX                  0x5aa5

/* FRV magic number - no EABI available??.  */
#define EM_CYGNUS_FRV           0x5441

/* Infineon Technologies 16-bit microcontroller with C166-V2 core.  */
#define EM_XC16X                0x4688

/* D10V backend magic number.  Written in the absence of an ABI.  */
#define EM_CYGNUS_D10V          0x7650

/* D30V backend magic number.  Written in the absence of an ABI.  */
#define EM_CYGNUS_D30V          0x7676

/* Ubicom IP2xxx;   Written in the absense of an ABI.  */
#define EM_IP2K_OLD             0x8217

/* (Deprecated) Temporary number for the OpenRISC processor.  */
#define EM_OR32                 0x8472

/* Cygnus PowerPC ELF backend.  Written in the absence of an ABI.  */
#define EM_CYGNUS_POWERPC       0x9025

/* Alpha backend magic number.  Written in the absence of an ABI.  */
#define EM_ALPHA                0x9026

/* Cygnus M32R ELF backend.  Written in the absence of an ABI.  */
#define EM_CYGNUS_M32R          0x9041

/* V850 backend magic number.  Written in the absense of an ABI.  */
#define EM_CYGNUS_V850          0x9080

/* old S/390 backend magic number. Written in the absence of an ABI.  */
#define EM_S390_OLD             0xa390

/* Old, unofficial value for Xtensa.  */
#define EM_XTENSA_OLD           0xabc7

#define EM_XSTORMY16            0xad45

/* mn10200 and mn10300 backend magic numbers.
   Written in the absense of an ABI.  */
#define EM_CYGNUS_MN10300       0xbeef
#define EM_CYGNUS_MN10200       0xdead

/* Renesas M32C and M16C.  */
#define EM_M32C_OLD             0xFEB0

/* Vitesse IQ2000.  */
#define EM_IQ2000               0xFEBA

/* NIOS magic number - no EABI available.  */
#define EM_NIOS32               0xFEBB

#define EM_CYGNUS_MEP           0xF00D  /* Toshiba MeP */

#define EM_MOXIE                0xFEED  /* Moxie */

/* Old Sunplus S+core7 backend magic number. Written in the absence of an ABI.  */
#define EM_SCORE_OLD            95

#define EM_MICROBLAZE_OLD       0xbaab  /* Old MicroBlaze */

/*
 * e_ident[EI_VERSION], e_version
 */
#define EV_NONE         0
#define EV_CURRENT      1
#define EV_NUM          2

/* Values for section header, sh_type field.  */

#define SHT_NULL        0      /* Section header table entry unused */
#define SHT_PROGBITS    1      /* Program specific (private) data */
#define SHT_SYMTAB      2      /* Link editing symbol table */
#define SHT_STRTAB      3      /* A string table */
#define SHT_RELA        4      /* Relocation entries with addends */
#define SHT_HASH        5      /* A symbol hash table */
#define SHT_DYNAMIC     6      /* Information for dynamic linking */
#define SHT_NOTE        7      /* Information that marks file */
#define SHT_NOBITS      8      /* Section occupies no space in file */
#define SHT_REL         9      /* Relocation entries, no addends */
#define SHT_SHLIB       10     /* Reserved, unspecified semantics */
#define SHT_DYNSYM      11     /* Dynamic linking symbol table */

/* Values for symb header, st_type field.  */

#define STB_LOCAL       0      /* Symbol not visible outside obj */
#define STB_GLOBAL      1      /* Symbol visible outside obj */
#define STB_WEAK        2      /* Like globals, lower precedence */

#define STT_NOTYPE      0      /* Symbol type is unspecified */
#define STT_OBJECT      1      /* Symbol is a data object */
#define STT_FUNC        2      /* Symbol is a code object */
#define STT_SECTION     3      /* Symbol associated with a section */
#define STT_FILE        4      /* Symbol gives a file name */
#define STT_COMMON      5      /* An uninitialised common block */
#define STT_TLS         6      /* Thread local data object */
#define STT_RELC        8      /* Complex relocation expression */
#define STT_SRELC       9      /* Signed Complex relocation expression */

#define STV_DEFAULT     0      /* Visibility is specified by binding type */
#define STV_INTERNAL    1      /* OS specific version of STV_HIDDEN */
#define STV_HIDDEN      2      /* Can only be seen inside currect component */
#define STV_PROTECTED   3      /* Treat as STB_LOCAL inside current component */

/**
 * ELF Section header. Code style from specification
 */
typedef struct {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off  sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

/**
 * ELF Program Segment header. Code style from specification
 */
typedef struct {
	Elf32_Word p_type;
	Elf32_Off  p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} Elf32_Phdr;

/**
 * Symbol table entry. Code style from specification
 */
typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	uint8_t    st_info;
	uint8_t    st_other;
	Elf32_Half st_shndx;
} Elf32_Sym;

/**
 * Relocation table entry in sections with SHT_REL type.
 * Code style from specification
 */
typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

/**
 * Relocation table entry in sections with SHT_RELA type.
 * Code style from specification
 */
typedef struct{
	Elf32_Addr  r_offset;
	Elf32_Word  r_info;
	Elf32_Sword r_addend;
} Elf32_Rela;

#if 0
/*Collection of information about elf*/
typedef struct {
	Elf32_Ehdr header;
	Elf32_Shdr sectionHeaders[MAX_NUMBER_OF_SECTIONS];
	Elf32_Phdr segmentHeaders[MAX_NUMBER_OF_SECTIONS];
	char stringTable[MAX_STRING_TABLE_LENGTH];
} ElfFile;
#endif

#endif /* LIB_ELF_TYPES_H_ */


#ifndef BOOTINFO_H_
#define BOOTINFO_H_

/*
 * 0x0:
 * 0x1: extended command line
 */
#define BOOTBLOCK_VER                         0x1

#define KSTRMAX_SIZE                          128
#define KSTRMAX_SIZE_EX                       512
#define BIOS_INFO_SIGN_SIZE                   8
#define KERNEL_ARGS_STRING_EX_SIGN_SIZE       22
#define BOOT_VER_STR_SIZE                     128
#define BOOTBLOCK_SIZE                        0x1000 /* 1 PAGE_SIZE */
#define X86BOOT_SIGNATURE                     0x8086
#define ROMLOADER_SIGNATURE                   0xe200
#define BIOS_INFO_SIGNATURE                   "E2KBIOS"
#define KERNEL_ARGS_STRING_EX_SIGNATURE       "KERNEL_ARGS_STRING_EX"
#define BOOT_KERNEL_ARGS_STRING_EX_SIGNATURE  boot_va_to_pa(KERNEL_ARGS_STRING_EX_SIGNATURE)

/*
 * Below is boot information that comes out of the x86 code of Linux/E2K
 * loader proto.
 */

/* L_MAX_NODE_PHYS_BANKS = 4 sometimes is not enough, so we increase it to
 * an arbitary value (8 now). The old L_MAX_NODE_PHYS_BANKS we rename to
 * L_MAX_NODE_PHYS_BANKS_FUSTY and take in mind for boot_info compatibility.
 *
 * L_MAX_NODE_PHYS_BANKS_FUSTY and L_MAX_MEM_NUMNODES describe max size of
 * array of memory banks on all nodes and should be in accordance with old value
 * of L_MAX_PHYS_BANKS for compatibility with boot_info old structure (bank)
 * size, so L_MAX_NODE_PHYS_BANKS_FUSTY * L_MAX_MEM_NUMNODES should be
 * equal to 32.
 */
#define L_MAX_NODE_PHYS_BANKS                 8 /* max number of memory banks on one node */
#define L_MAX_NODE_PHYS_BANKS_FUSTY           4 /* fusty max number of memory banks on one node */
#define L_MAX_PHYS_BANKS_EX                   64 /* max number of memory banks in banks_ex field of boot_info */
#define L_MAX_MEM_NUMNODES                    8 /* max number of nodes in the list of memory banks on each node */

#define L_MAX_BUSY_AREAS                      4 /* max number of busy areas occupied by BIOS and should be kept unchanged by kernel to support recovery mode */
#define L_MAX_CNT_POINTS                      8 /* max number of control points */

#ifndef __ASSEMBLY__

typedef struct bank_info {
	uint64_t address;                               /* start address of bank */
	uint64_t size;                                  /* size of bank in bytes */
} bank_info_t;

typedef struct node_banks {
	bank_info_t banks[L_MAX_NODE_PHYS_BANKS_FUSTY]; /* memory banks array of a node */
} node_banks_t;

typedef struct cntp_info {
	uint64_t kernel_base;                           /* kernel image base address */
	uint64_t node_data;                             /* physical address of memory nodes data list */
	uint64_t nosave_areas;                          /* do not save all areas from this array physical address of array */
	uint16_t nosaves_num;                           /* number of areas to do not save */
	uint8_t mem_valid;                              /* control point is valid in memory */
	uint8_t disk_valid;                             /* control point is valid on disk */
} cntp_info_t;

typedef struct dump_info {
	uint64_t pa_init_task;                          /* phys address of init_task */
	uint64_t init_mm;                               /* address of init_mm */
	uint64_t targ_KERNEL_BASE;                      /* KERNEL_BASE value  */
	uint64_t mm_offset;                             /* offset mm member in task_struct */
	uint64_t tasks_offset;                          /* offset tasks member in task_struct */
	uint64_t thread_offset;                         /* offset thread member in task_struct kallsyms info to print procedure names */
	uint64_t kallsyms_addresses;
	uint64_t kallsyms_num_syms;
	uint64_t kallsyms_names;
	uint64_t kallsyms_token_table;
	uint64_t kallsyms_token_index;
	uint64_t kallsyms_markers;
	/* sections of original kernel image */
	uint64_t _start;
	uint64_t _end;
	uint64_t _stext;
	uint64_t _etext;
	uint64_t _sinittext;
	uint64_t _einittext;
	/* Trace buffer description of original kernel */
	uint64_t reserved1;
	uint64_t reserved2;
} dump_info_t;

typedef struct boot_times {
	uint64_t arch;
	uint64_t unpack;
	uint64_t pci;
	uint64_t drivers1;
	uint64_t drivers2;
	uint64_t menu;
	uint64_t sm;
	uint64_t kernel;
	uint64_t reserved[8];
} boot_times_t;

typedef struct bios_info {
	uint8_t signature[BIOS_INFO_SIGN_SIZE];         /* signature, 'E2KBIOS' */
	uint8_t boot_ver[BOOT_VER_STR_SIZE];            /* boot version */
	uint8_t mb_type;                                /* mother board type */
	uint8_t chipset_type;                           /* chipset type */
	uint8_t cpu_type;                               /* cpu type */
	uint8_t kernel_args_string_ex[KSTRMAX_SIZE_EX]; /* extended command line of kernel used to pass command line from e2k BIOS */
	uint8_t reserved1;                              /* reserved1 */
	uint32_t cache_lines_damaged;                   /* number of damaged cache lines */
	uint64_t nodes_mem_slabs_deprecated[52];        /* array of slabs accessible memory on each node accessible memory on each node */
	bank_info_t banks_ex[L_MAX_PHYS_BANKS_EX];      /* extended array of descriptors of banks of available physical memory */
	uint64_t devtree;                               /* devtree pointer */
	uint32_t bootlog_addr;                          /* bootlog address */
	uint32_t bootlog_len;                           /* bootlog length */
#if !defined(DISABLE_BOOT_BY_UUID_OR_VOLNAME) && !defined(DISABLE_PASS_UUID_TO_OS)
	uint8_t uuid[16];                               /* UUID for boot device */
#endif
} bios_info_t;

typedef struct boot_info {
	uint16_t signature;                             /* signature, 0x8086 */
	uint16_t reserved1;                             /* reserved1 */
	uint16_t reserved2;                             /* reserved2 */
	uint8_t vga_mode;                               /* vga mode */
	uint8_t num_of_banks;                           /* number of available physical memory banks see below bank array total number on all nodes or 0 */
	uint64_t kernel_base;                           /* base address to load kernel image if 0 then BIOS can load at any address but address should be large page size aligned - 4 Mb */
	uint64_t kernel_size;                           /* kernel image byte's size */
	uint64_t ramdisk_base;                          /* base address to load RAM-disk now not used */
	uint64_t ramdisk_size;                          /* RAM-disk byte's size */
	uint16_t num_of_cpus;                           /* number of started physical CPU(s) */
	uint16_t mach_flags;                            /* machine identifacition flags should be set by our romloader and BIOS */
	uint16_t num_of_busy;                           /* number of busy areas occupied by BIOS see below busy array */
	uint16_t num_of_nodes;                          /* number of nodes on NUMA system */
	uint64_t mp_table_base;                         /* MP-table base address */
	uint64_t serial_base;                           /* base address of serial port for Am85c30 in e3s architecture, used for debugging purpose */
	uint64_t nodes_map;                             /* online nodes map */
	uint64_t mach_serialn;                          /* serial number of the machine */
	uint8_t mac_addr[6];                            /* base MAC address for ethernet cards */
	uint16_t reserved3;                             /* reserved3 */
	char kernel_args_string[KSTRMAX_SIZE];          /* command line of kernel, used to pass command line from e2k BIOS */
	node_banks_t nodes_mem[L_MAX_MEM_NUMNODES];     /* array of descriptors of banks of available physical memory on each node */
	bank_info_t busy[L_MAX_BUSY_AREAS];             /* descriptors of areas occupied by BIOS, all this shoud be kept in system recovery mode */
	cntp_info_t cntp_info[L_MAX_CNT_POINTS];        /* control points info to save and restore them state */
	dump_info_t dmp;                                /* Info for future work of dump analyzer */
	uint64_t reserved4[13];                         /* reserved4 */
	uint8_t mb_name[16];                            /* Motherboard product name */
	uint32_t reserved5;                             /* reserved5 */
	uint32_t kernel_csum;                           /* kernel image control sum */
	bios_info_t bios;                               /* extended BIOS info SHOULD BE LAST ITEM into this structure */
} boot_info_t;

typedef struct bootblock_struct {
	boot_info_t info;                               /* general kernel<->BIOS info */
	uint8_t gap[BOOTBLOCK_SIZE -                    /* zip area to make size of bootblock struct - constant */
              sizeof(boot_info_t) -    
              sizeof(boot_times_t) -
              1 - /* uint8_t  : bootblock_ver */
              4 - /* uint32_t : reserved1 */
              2 - /* uint16_t : kernel_flags */
              1 - /* uint8_t  : reserved2 */
              1 - /* uint8_t  : number of cnt points */
              1 - /* uint8_t  : current # of cnt point */
              1 - /* uint8_t  : number of cnt points ready in the memory */
              1 - /* uint8_t  : number of cnt points saved on the disk */
              1 - /* uint8_t  : all control points is created */
              8 - /* uint64_t : dump sector */
              8 - /* uint64_t : cnt point sector */
              2 - /* uint16_t : dump device */
              2 - /* uint16_t : cnt point device */
              2 - /* uint16_t : boot_flags */
              2]; /* uint16_t : x86_marker */
	uint8_t bootblock_ver;                          /* bootblock version number */
	uint32_t reserved1;                             /* reserved1 */
	boot_times_t boot_times;                        /* boot load times */
	uint16_t kernel_flags;                          /* kernel flags, boot should not modify it */
	uint8_t reserved2;                              /* reserved2 */
	uint8_t cnt_points_num_deprecated;              /* number of control points, all memory will be devided on this number of parts */
	uint8_t cur_cnt_point;                          /* current # of active control point (running part) */
	uint8_t mem_cnt_points;                         /* number of started control points (ready in the memory) */
	uint8_t disk_cnt_points;                        /* number of control points saved on the disk (ready to be loaded from disk) */
	uint8_t cnt_points_created;                     /* all control points created in the memory and on disk */
	uint64_t dump_sector;                           /* start sector # to dump physical memory */
	uint64_t cnt_point_sector_deprecated;           /* start sector # to save restore control points */
	uint16_t dump_dev;                              /* disk # to dump memory */
	uint16_t cnt_point_dev_deprecated;              /* disk # for save/restore control point */
	uint16_t boot_flags;                            /* boot flags: if non zero then this structure is recovery info structure instead of boot info structure */
	uint16_t x86_marker;                            /* marker of the end of x86 boot block (0xAA55) */
} bootblock_struct_t;

#endif /* ! __ASSEMBLY__ */

/*
 * Boot block flags to elaborate boot modes
 */

#define RECOVERY_BB_FLAG               0x0001 /* recovery flag: if non zero then this structure is recovery info structure instead of boot info structure */
                                              /* BIOS should not clear memory and should keep current state of physical memory */
#define CNT_POINT_BB_FLAG              0x0002 /* kernel restarted in the mode of control point creation */
                                              /* BIOS should read kernel image from the disk to the specified area of the memory and start kernel */
#define DUMP_ANALYZE_BB_FLAG           0x0008 /* This flag is used only by kernel to indicate dump analyzer mode */
#define MEMORY_DUMP_BB_FLAG            0x0010 /* BIOS should dump all physical memory before start all other actions */

/*
 * The machine identification flags
 */

#define SIMULATOR_MACH_FLAG            0x0001 /* system is running on simulator */
#define PROTOTYPE_MACH_FLAG_DEPRECATED 0x0002 /* machine is prototype */
#define IOHUB_MACH_FLAG                0x0004 /* machine has IOHUB */
#define OLDMGA_MACH_FLAG               0x0008 /* MGA card has old firmware */
#define MULTILINK_MACH_FLAG            0x0010 /* some nodes are connected by sevral IP links */
#define MSI_MACH_FLAG                  0x0020 /* boot inits right values in apic to support MSI. Meanfull for e2k only. For v9 it always true */

/*
 * The chipset types
 */

#define CHIPSET_TYPE_PIIX4             0x01 /* PIIX4 */
#define CHIPSET_TYPE_IOHUB             0x02 /* IOHUB */

#define CPU_TYPE_E3M         0x1 /* E3M */
#define CPU_TYPE_E3S         0x2 /* E3S */
#define CPU_TYPE_E2S        0x03 /* E2S */
#define CPU_TYPE_E8C        0x07 /* E8C */
#define CPU_TYPE_E1CP       0x08 /* E1C+ */
#define CPU_TYPE_E8C2       0x09 /* E8C */
#define CPU_TYPE_E12C       0x0a /* E12C */
#define CPU_TYPE_E16C       0x0b /* E16C */
#define CPU_TYPE_E2C3       0x0c /* E2C3 */


extern bootblock_struct_t BOOTBLOCK;

#endif /* BOOTINFO_H_ */

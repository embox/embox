/**
 * @file
 * @brief the header for Multiboot
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#ifndef X86_MULTIBOOT_H_
#define X86_MULTIBOOT_H_

/* The magic number for the Multiboot header. */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* The flags for the Multiboot header. */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS		0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS		0x00010003
#endif

/* C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)			_ ## sym
#else
# define EXT_C(sym)			sym
#endif

#ifndef __ASSEMBLER__

/* The Multiboot header. */
typedef struct multiboot_header {
	/* Must be MULTIBOOT_MAGIC */
	unsigned long magic;
	/* Feature flags */
	unsigned long flags;
	unsigned long checksum;
	/* These are only valid if MULTIBOOT_AOUT_KLUDGE is set. */
	unsigned long header_addr;
	unsigned long load_addr;
	unsigned long load_end_addr;
	unsigned long bss_end_addr;
	unsigned long entry_addr;
} multiboot_header_t;

#define MULTIBOOT_MUSTKNOW           0x0000ffff
/* Align all boot modules on page (4KB) boundaries. */
#define MULTIBOOT_PAGE_ALIGN         0x00000001
/* Must be provided memory information in multiboot_info structure */
#define MULTIBOOT_MEMORY_INFO        0x00000002
#define MULTIBOOT_AOUT_KLUDGE        0x00010000
/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_BOOTLOADER_MAGIC   0x2BADB002

/* The symbol table for a.out. */
typedef struct aout_symbol_table {
	unsigned long tabsize;
	unsigned long strsize;
	unsigned long addr;
	unsigned long reserved;
} aout_symbol_table_t;

/* The section header table for ELF. */
typedef struct elf_section_header_table {
	unsigned long num;
	unsigned long size;
	unsigned long addr;
	unsigned long shndx;
} elf_section_header_table_t;

/* The Multiboot information.
   The boot loader passes this data structure to the embox in
   register EBX on entry. */
typedef struct multiboot_info {
	/* These flags indicate which parts of the multiboot_info are valid */
	unsigned long flags;
	/* Lower/Upper memory installed in the machine. */
	unsigned long mem_lower;
	unsigned long mem_upper;
	/* BIOS disk device the kernel was loaded from. */
	unsigned long boot_device;
	/* Command-line for the OS kernel: a null-terminated ASCII string. */
	unsigned long cmdline;
	/* List of boot modules loaded with the kernel. */
	unsigned long mods_count;
	unsigned long mods_addr;
	/* Symbol information for a.out or ELF executables. */
	union {
		aout_symbol_table_t aout_sym;
		elf_section_header_table_t elf_sec;
	} u;
	/* Memory map buffer. */
	unsigned long mmap_length;
	unsigned long mmap_addr;
} multiboot_info_t;

#define MULTIBOOT_MEMORY        0x00000001
#define MULTIBOOT_BOOT_DEVICE   0x00000002
#define MULTIBOOT_CMDLINE       0x00000004
#define MULTIBOOT_MODS          0x00000008
#define MULTIBOOT_AOUT_SYMS     0x00000010
#define MULTIBOOT_ELF_SHDR      0x00000020
#define MULTIBOOT_MEM_MAP       0x00000040

/* The module structure. */
typedef struct module {
	unsigned long mod_start;
	unsigned long mod_end;
	unsigned long string;
	unsigned long reserved;
} module_t;

/* The memory map. Be careful that the offset 0 is base_addr_low
   but no size. */
typedef struct memory_map {
	unsigned long size;
	unsigned long base_addr_low;
	unsigned long base_addr_high;
	unsigned long length_low;
	unsigned long length_high;
	unsigned long type;
} memory_map_t;

#endif /* __ASSEMBLER__ */
#endif /* X86_MULTIBOOT_H_ */


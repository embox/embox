/**
 * @file
 *
 * @date Jan 25, 2013
 * @author: Anton Bondarev
 */

#include <asm/multiboot.h>

	.section .multiboot
	/* Align 32 bits boundary.  */
	.align	4

	/* Multiboot header.  */
multiboot_header:
	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long	MULTIBOOT_HEADER_FLAGS
	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
/* we always use elf format */
	/* header_addr */
	.long	0
	/* load_addr */
	.long	0
	/* load_end_addr */
	.long	0
	/* bss_end_addr */
	.long	0
	/* entry_addr */
	.long	0

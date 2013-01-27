/**
 * @file
 *
 * @brief http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 *
 * @details The layout of the Multiboot header must be as follows:
 *          Offset 	Type 	Field Name 	Note
 *            0 	u32 	magic 	required
 *            4 	u32 	flags 	required
 *            8 	u32 	checksum 	required
 *            12 	u32 	header_addr 	if flags[16] is set
 *            16 	u32 	load_addr 	if flags[16] is set
 *            20 	u32 	load_end_addr 	if flags[16] is set
 *            24 	u32 	bss_end_addr 	if flags[16] is set
 *            28 	u32 	entry_addr 	if flags[16] is set
 *            32 	u32 	mode_type 	if flags[2] is set
 *            36 	u32 	width 	if flags[2] is set
 *            40 	u32 	height 	if flags[2] is set
 *            44 	u32 	depth 	if flags[2] is set
 *
 * @date Jan 25, 2013
 * @author: Anton Bondarev
 */

#include <asm/multiboot.h>
#include <framework/mod/options.h>
#include <module/embox/arch/x86/boot/multiboot.h>

#define VIDEO_MODE_ENABLE OPTION_GET(NUMBER,video_mode_set)
#define VIDEO_MODE_WIDTH OPTION_GET(NUMBER,video_width)
#define VIDEO_MODE_HEIGTH OPTION_GET(NUMBER,video_height)
#define VIDEO_MODE_DEPTH OPTION_GET(NUMBER,video_depth)

#define MULTIBOOT_HEADER_FULL_FLAGS (MULTIBOOT_HEADER_FLAGS | (VIDEO_MODE_ENABLE << 2))

	.section .multiboot
	/* Align 32 bits boundary.  */
	.align	4

	/* Multiboot header.  */
multiboot_header:
	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long	MULTIBOOT_HEADER_FULL_FLAGS
	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FULL_FLAGS)
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

	/* we must setup bit 2 in flags for validate next value */
	/* mode type */
	.long  0 /* we use only for graphic mode now */
	/* width */
	.long  VIDEO_MODE_WIDTH
	/* height */
	.long  VIDEO_MODE_HEIGTH
	/* depth */
	.long  VIDEO_MODE_DEPTH

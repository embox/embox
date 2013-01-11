/**
 * @file
 *
 * @date Dec 14, 2012
 * @author: Anton Bondarev
 */

#include <asm/io.h>
#include <drivers/video/vesa_mode.h>
#include <drivers/video/display.h>
#include <drivers/video/vga.h>
#include <util/array.h>



//http://wiki.osdev.org/VGA_Hardware
//http://www.monstersoft.com/tutorial1/VESA_intro.html
//http://www.faqs.org/faqs/pc-hardware-faq/supervga-programming/

/*****************************************************************************
*****************************************************************************/
void vga_read_regs(unsigned char *regs) {
	unsigned i;

/* read MISCELLANEOUS reg */
	*regs = vga_misc_read();
	regs++;

/* read SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		*regs = vga_rseq(0, i);
		regs++;
	}

/* read CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		*regs = vga_rcrt(0,i);
		regs++;
	}
/* read GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++) {
		*regs = vga_rgfx(0, i);
		regs++;
	}
/* read ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++) {
		*regs = vga_rattr(0, i);
		regs++;
	}
/* lock 16-color palette and unblank display */
	vga_display_enable(1); /* we must do it because we use AC register end clear enable bit */
}


/*****************************************************************************
*****************************************************************************/
void vga_write_regs(unsigned char *regs) {
	unsigned i;

/* write MISCELLANEOUS reg */
	vga_misc_write(*regs);
	regs++;
/* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		vga_wseq(0, i, *regs);
		regs++;
	}
/* unlock CRTC registers */
	vga_wcrt(NULL, VGA_CRTC_H_BLANK_END, vga_rcrt(NULL, VGA_CRTC_H_BLANK_END) | 0x80);
	vga_wcrt(NULL, VGA_CRTC_V_SYNC_END, vga_rcrt(NULL, VGA_CRTC_V_SYNC_END) & ~0x80);
/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
/* write CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		vga_wcrt(0, i,*regs);
		regs++;
	}
/* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++) {
		vga_wgfx(0, i, *regs);
		regs++;
	}
/* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++) {
		vga_wattr(0, i, *regs);
		regs++;
	}
/* lock 16-color palette and unblank display */
	vga_display_enable(1);
}


struct display * vga_setup_mode(struct display *display, enum vesa_video_mode mode) {
	const struct vga_mode_description *mode_desc;

	if (NULL == (mode_desc = vga_mode_description_lookup(mode))) {
		return NULL;
	}

	display->set_pixel = (void *)mode_desc->set_pixel;
	display->height = mode_desc->height;
	display->width = mode_desc->width;

	vga_write_regs((unsigned char *)mode_desc->regs);
	if(NULL != mode_desc->setup) {
		mode_desc->setup((struct vga_mode_description *)mode_desc);
	}
	display_clear_screen(display);

	return display;
}


#if 0
static void dump(unsigned char *regs, unsigned count)
{
	unsigned i;

	i = 0;
	printf("\t");
	for(; count != 0; count--)
	{
		printf("0x%02X,", *regs);
		i++;
		if(i >= 8)
		{
			i = 0;
			printf("\n\t");
		}
		else
			printf(" ");
		regs++;
	}
	printf("\n");
}
/*****************************************************************************
*****************************************************************************/
static void dump_regs(unsigned char *regs)
{
	printf("unsigned char g_mode[] =\n");
	printf("{\n");
/* dump MISCELLANEOUS reg */
	printf("/* MISC */\n");
	printf("\t0x%02X,\n", *regs);
	regs++;
/* dump SEQUENCER regs */
	printf("/* SEQ */\n");
	dump(regs, VGA_NUM_SEQ_REGS);
	regs += VGA_NUM_SEQ_REGS;
/* dump CRTC regs */
	printf("/* CRTC */\n");
	dump(regs, VGA_NUM_CRTC_REGS);
	regs += VGA_NUM_CRTC_REGS;
/* dump GRAPHICS CONTROLLER regs */
	printf("/* GC */\n");
	dump(regs, VGA_NUM_GC_REGS);
	regs += VGA_NUM_GC_REGS;
/* dump ATTRIBUTE CONTROLLER regs */
	printf("/* AC */\n");
	dump(regs, VGA_NUM_AC_REGS);
	regs += VGA_NUM_AC_REGS;
	printf("};\n");
}

/*****************************************************************************
READ AND DUMP VGA REGISTER VALUES FOR CURRENT VIDEO MODE
This is where g_40x25_text[], g_80x50_text[], etc. came from :)
*****************************************************************************/
void vesa_dump_state(void)
{
	unsigned char state[VGA_NUM_REGS];

	read_regs(state);
	dump_regs(state);
}
#endif

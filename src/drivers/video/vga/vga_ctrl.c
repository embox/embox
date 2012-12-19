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


#define inportb(P)	      in8(P)
#define outportb(P,V)	   out8(V, P)

/* CauseWay DOS extender only */
#define peekb(S,O)	      *(unsigned char *)(16uL * (S) + (O))
#define pokeb(S,O,V)	    *(unsigned char *)(16uL * (S) + (O)) = (V)
#define pokew(S,O,V)	    *(unsigned short *)(16uL * (S) + (O)) = (V)
#define _vmemwr(DS,DO,S,N)      memcpy((char *)((DS) * 16 + (DO)), S, N)


extern void vmemwr(unsigned dst_off, unsigned char *src, unsigned count);

extern void vpokeb(unsigned off, unsigned val);

extern unsigned vpeekb(unsigned off);

extern void set_plane(unsigned p);



//http://wiki.osdev.org/VGA_Hardware
//http://www.monstersoft.com/tutorial1/VESA_intro.html

/*****************************************************************************
*****************************************************************************/
void vga_read_regs(unsigned char *regs) {
	unsigned i;

/* read MISCELLANEOUS reg */
	*regs = inportb(VGA_MISC_READ);
	regs++;

/* read SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		outportb(VGA_SEQ_INDEX, i);
		*regs = inportb(VGA_SEQ_DATA);
		regs++;
	}

/* read CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		outportb(VGA_CRTC_INDEX, i);
		*regs = inportb(VGA_CRTC_DATA);
		regs++;
	}
/* read GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++) {
		outportb(VGA_GC_INDEX, i);
		*regs = inportb(VGA_GC_DATA);
		regs++;
	}
/* read ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++) {
		(void)inportb(VGA_INSTAT_READ);
		outportb(VGA_AC_INDEX, i);
		*regs = inportb(VGA_AC_READ);
		regs++;
	}
/* lock 16-color palette and unblank display */
	inportb(VGA_INSTAT_READ);
	outportb(VGA_AC_INDEX, 0x20);
}


/*****************************************************************************
*****************************************************************************/
void vga_write_regs(unsigned char *regs) {
	unsigned i;

/* write MISCELLANEOUS reg */
	outportb(VGA_MISC_WRITE, *regs);
	regs++;
/* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		outportb(VGA_SEQ_INDEX, i);
		outportb(VGA_SEQ_DATA, *regs);
		regs++;
	}
/* unlock CRTC registers */
	outportb(VGA_CRTC_INDEX, 0x03);
	outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) | 0x80);
	outportb(VGA_CRTC_INDEX, 0x11);
	outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x80);
/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
/* write CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		outportb(VGA_CRTC_INDEX, i);
		outportb(VGA_CRTC_DATA, *regs);
		regs++;
	}
/* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++) {
		outportb(VGA_GC_INDEX, i);
		outportb(VGA_GC_DATA, *regs);
		regs++;
	}
/* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++) {
		inportb(VGA_INSTAT_READ);
		outportb(VGA_AC_INDEX, i);
		outportb(VGA_AC_WRITE, *regs);
		regs++;
	}
/* lock 16-color palette and unblank display */
	inportb(VGA_INSTAT_READ);
	outportb(VGA_AC_INDEX, 0x20);
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

/**
 * @file
 *
 * @date Dec 13, 2012
 * @author: Anton Bondarev
 */

#ifndef VIDEO_VGA_H_
#define VIDEO_VGA_H_


#define VGA_AC_INDEX        0x3C0
#define VGA_AC_WRITE        0x3C0
#define VGA_AC_READ         0x3C1
#define VGA_MISC_WRITE      0x3C2
#define VGA_SEQ_INDEX       0x3C4
#define VGA_SEQ_DATA        0x3C5
#define VGA_DAC_READ_INDEX  0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA        0x3C9
#define VGA_MISC_READ       0x3CC
#define VGA_GC_INDEX        0x3CE
#define VGA_GC_DATA         0x3CF

/*		      COLOR emulation	 MONO emulation */
#define VGA_CRTC_INDEX      0x3D4	   /* 0x3B4 */
#define VGA_CRTC_DATA       0x3D5	   /* 0x3B5 */
#define VGA_INSTAT_READ     0x3DA

#define VGA_NUM_SEQ_REGS   5
#define VGA_NUM_CRTC_REGS  25
#define VGA_NUM_GC_REGS    9
#define VGA_NUM_AC_REGS    21

#define VGA_NUM_REGS	    \
	(1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)


extern void vga_read_regs(unsigned char *regs);

extern void vga_write_regs(unsigned char *regs);


#include <util/array.h>
#include <drivers/vesa.h>

struct display;
struct vga_mode_description;

typedef void (*vga_set_pixel_ft)(struct display *display, int x, int y, int c);
typedef void (*vga_putc_ft)(struct display *display, int x, int y, int c);
typedef void (*vga_setup_ft)(struct vga_mode_description *vga_mode);

struct vga_mode_description {
	int mode;
	const unsigned char *regs;
	vga_set_pixel_ft set_pixel;
	vga_putc_ft putc;
	vga_setup_ft setup;
	int width;
	int height;
};

#define VGA_MODE_DEFINE(vga_mode,vga_regs,setp,put_char,setup_f,scr_width,scr_height)  \
	extern const struct vga_mode_description *__vga_mode_descriptions[];               \
	static const struct vga_mode_description vga_mode_##mode = {                       \
			.mode = vga_mode,                                                          \
			.regs = vga_regs,                                                          \
			.set_pixel = setp,                                                         \
			.putc = put_char,                                                     \
			.setup = setup_f,                                                     \
			.width = scr_width,                                                  \
			.height = scr_height                                                 \
	};                                                                           \
	ARRAY_SPREAD_ADD(__vga_mode_descriptions,&vga_mode_##mode);



const struct vga_mode_description *vga_mode_description_lookup(enum vesa_video_mode);


//FIXME this is temporary
/* CauseWay DOS extender only */
#define peekb(S,O)	      *(unsigned char *)(16uL * (S) + (O))
#define pokeb(S,O,V)	    *(unsigned char *)(16uL * (S) + (O)) = (V)
#define pokew(S,O,V)	    *(unsigned short *)(16uL * (S) + (O)) = (V)
#define _vmemwr(DS,DO,S,N)      memcpy((char *)((DS) * 16 + (DO)), S, N)


#endif /* VIDEO_VGA_H_ */

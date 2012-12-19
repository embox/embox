/**
 * @file
 *
 * @date Dec 13, 2012
 * @author: Anton Bondarev
 */

#ifndef VIDEO_VGA_H_
#define VIDEO_VGA_H_

/* AC - attribute controller */
#define VGA_AC_INDEX        0x3C0
#define VGA_AC_WRITE        0x3C0
#define VGA_AC_READ         0x3C1

/* MISC registers */
#define VGA_MISC_WRITE      0x3C2
#define VGA_MISC_READ       0x3CC

/* Sequence registers */
#define VGA_SEQ_INDEX       0x3C4
#define VGA_SEQ_DATA        0x3C5

/* DAC - digit to analog converter registers */
#define VGA_DAC_READ_INDEX  0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA        0x3C9

/* GC - graphic controller */
#define VGA_GC_INDEX        0x3CE
#define VGA_GC_DATA         0x3CF

/* COLOR emulation	 MONO emulation */
#define VGA_CRTC_INDEX      0x3D4	   /* 0x3B4 */
#define VGA_CRTC_DATA       0x3D5	   /* 0x3B5 */
#define VGA_INSTAT_READ     0x3DA



#define VGA_NUM_SEQ_REGS   5
#define VGA_NUM_CRTC_REGS  25
#define VGA_NUM_GC_REGS    9
#define VGA_NUM_AC_REGS    21

#include <types.h>

struct vga_registers {
	uint16_t seq[VGA_NUM_SEQ_REGS];
	uint16_t crtc[VGA_NUM_CRTC_REGS];
	uint16_t gc[VGA_NUM_GC_REGS];
	uint16_t ac[VGA_NUM_AC_REGS];
};


#define VGA_NUM_REGS	    \
	(1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)


extern void vga_read_regs(unsigned char *regs);

extern void vga_write_regs(unsigned char *regs);


//FIXME this is temporary
/* CauseWay DOS extender only */
#define peekb(S,O)	      *(unsigned char *)(16uL * (S) + (O))
#define pokeb(S,O,V)	    *(unsigned char *)(16uL * (S) + (O)) = (V)
#define pokew(S,O,V)	    *(unsigned short *)(16uL * (S) + (O)) = (V)
#define _vmemwr(DS,DO,S,N)      memcpy((char *)((DS) * 16 + (DO)), S, N)


#endif /* VIDEO_VGA_H_ */

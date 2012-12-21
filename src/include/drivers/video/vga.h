/**
 * @file
 *
 * @date Dec 13, 2012
 * @author: Anton Bondarev
 */

#ifndef VIDEO_VGA_H_
#define VIDEO_VGA_H_



//FIXME this is temporary
/* CauseWay DOS extender only */
#define peekb(S,O)	      *(unsigned char *)(16uL * (S) + (O))
#define pokeb(S,O,V)	    *(unsigned char *)(16uL * (S) + (O)) = (V)
#define pokew(S,O,V)	    *(unsigned short *)(16uL * (S) + (O)) = (V)
#define _vmemwr(DS,DO,S,N)      memcpy((char *)((DS) * 16 + (DO)), S, N)


/* AC - attribute controller */
#define VGA_AC_INDEX        0x3C0
#define VGA_AC_WRITE        0x3C0
#define VGA_AC_READ         0x3C1

/* MISC registers */
#define VGA_MISC_WRITE      0x3C2
#define VGA_MISC_READ       0x3CC

/* Sequencer registers */
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

/* Input status register */
#define VGA_INSTAT_READ     0x3DA


#define VGA_CRTC_H_TOTAL       0x0
#define VGA_CRTC_H_DISP_END    0x1
#define VGA_CRTC_H_BLANK_START 0x2
#define VGA_CRTC_H_BLANK_END   0x3
#define VGA_CRTC_H_SYNC_START  0x4
#define VGA_CRTC_H_SYNC_END    0x5
#define VGA_CRTC_V_TOTAL       0x6
#define VGA_CRTC_OVERFLOW      0x7

#define VGA_CRTC_V_SYNC_START  0x10
#define VGA_CRTC_V_SYNC_END    0x11
#define VGA_CRTC_V_DISP_END    0x12
#define VGA_CRTC_V_BLANK_START 0x15
#define VGA_CRTC_V_BLANK_END   0x16
#define VGA_CRTC_MODE_CONTROL  0x17
#define VGA_CRTC_LINE_COMPARE  0x18

#define VGA_GC_READ_MAP_SEL   0x4
#define VGA_GC_MISCELLANEOUS  0x6


#define VGA_SEQ_PLANE_MASK   0x2




#define VGA_NUM_SEQ_REGS   5
#define VGA_NUM_CRTC_REGS  25
#define VGA_NUM_GC_REGS    9
#define VGA_NUM_AC_REGS    21

#include <types.h>
#include <asm/io.h>

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



static inline void vga_misc_write(unsigned char value) {
	out8(value, VGA_MISC_WRITE);
}

static inline unsigned char vga_misc_read(void) {
	return in8(VGA_MISC_READ);
}

static inline void vga_wcrt(uint32_t *regbase, unsigned char value, unsigned char index) {
	out8(index, VGA_CRTC_INDEX);
	out8(value, VGA_CRTC_DATA);
}

static inline unsigned char vga_rcrt(uint32_t *regbase, unsigned index) {
	out8(index, VGA_CRTC_INDEX);
	return in8(VGA_CRTC_DATA);
}

static inline void vga_wseq(uint32_t *regbase, unsigned char value, unsigned char index) {
	out8(index, VGA_SEQ_INDEX);
	out8(value, VGA_SEQ_DATA);
}

static inline unsigned char vga_rseq(uint32_t *regbase, unsigned char index) {
	out8(index, VGA_SEQ_INDEX);
	return in8(VGA_SEQ_DATA);
}


static inline void vga_gc_write(unsigned char value, unsigned char index) {
	out8(index, VGA_GC_INDEX);
	out8(value, VGA_GC_DATA);
}

static inline unsigned char vga_gc_read(unsigned char index) {
	out8(index, VGA_GC_INDEX);
	return in8(VGA_GC_DATA);
}

static inline void vga_ac_write(unsigned char value, unsigned char index) {
	in8(VGA_INSTAT_READ);
	out8(index, VGA_AC_INDEX);
	out8(value, VGA_AC_WRITE);
}

static inline unsigned char vga_ac_read(unsigned char index) {
	out8(index, VGA_AC_INDEX);
	return in8(VGA_AC_READ);
}

static inline void vga_display_enable(int stat) {
	in8(VGA_INSTAT_READ);
	out8(stat << 5, VGA_AC_INDEX);
}

#endif /* VIDEO_VGA_H_ */

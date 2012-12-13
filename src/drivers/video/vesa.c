/**
 * @file
 * @brief This file provides low-level video-mode routines for XWnd.
 *
 * @date Sep 14, 2012
 * @author Alexandr Chernakov
 */

#include <string.h> /* movedata(), memcpy() */
#include <stdio.h> /* printf() */
#include <drivers/video/vga.h>
#include <drivers/vesa.h>
#include <asm/io.h> /* inp(), outp() */

#define FP_SEG(x) (uint16_t)((uint32_t)(x) >> 16)
#define FP_OFF(x) (uint16_t)((uint32_t)(x))
#define MK_FP(seg,off) ((void *) (((unsigned long)(seg) << 16) | (unsigned)(off))


#define inportb(P)	      in8(P)
#define outportb(P,V)	   out8(V, P)

/* CauseWay DOS extender only */
#define peekb(S,O)	      *(unsigned char *)(16uL * (S) + (O))
#define pokeb(S,O,V)	    *(unsigned char *)(16uL * (S) + (O)) = (V)
#define pokew(S,O,V)	    *(unsigned short *)(16uL * (S) + (O)) = (V)
#define _vmemwr(DS,DO,S,N)      memcpy((char *)((DS) * 16 + (DO)), S, N)

/*#define       peekb(S,O)	      *(unsigned char *)MK_FP(S, O)
#define pokeb(S,O,V)	    *(unsigned char *)MK_FP(S, O) = (V)
#define pokew(S,O,V)	    *(unsigned short *)MK_FP(S, O) = (V)
#define _vmemwr(DS,DO,S,N)      movedata(FP_SEG(S), FP_OFF(S), DS, DO, N)
*/


/*****************************************************************************
VGA REGISTER DUMPS FOR VARIOUS TEXT MODES

()=to do
	40x25   (40x30) 40x50   (40x60)
	(45x25) (45x30) (45x50) (45x60)
	80x25   (80x30) 80x50   (80x60)
	(90x25) 90x30   (90x50) 90x60
*****************************************************************************/


extern void setup_font(unsigned font_height);

static unsigned char old_mode[64];

unsigned char g_40x25_text[] =
{
/* MISC */
	0x67,
/* SEQ */
	0x03, 0x08, 0x03, 0x00, 0x02,
/* CRTC */
	0x2D, 0x27, 0x28, 0x90, 0x2B, 0xA0, 0xBF, 0x1F,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0xA0,
	0x9C, 0x8E, 0x8F, 0x14, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00,
};

unsigned char g_40x50_text[] =
{
/* MISC */
	0x67,
/* SEQ */
	0x03, 0x08, 0x03, 0x00, 0x02,
/* CRTC */
	0x2D, 0x27, 0x28, 0x90, 0x2B, 0xA0, 0xBF, 0x1F,
	0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x04, 0x60,
	0x9C, 0x8E, 0x8F, 0x14, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00,
};

unsigned char g_80x25_text[] =
{
/* MISC */
	0x67,
/* SEQ */
	0x03, 0x00, 0x03, 0x00, 0x02,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00
};

unsigned char g_80x50_text[] =
{
/* MISC */
	0x67,
/* SEQ */
	0x03, 0x00, 0x03, 0x00, 0x02,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
	0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x01, 0x40,
	0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00,
};

unsigned char g_90x30_text[] =
{
/* MISC */
	0xE7,
/* SEQ */
	0x03, 0x01, 0x03, 0x00, 0x02,
/* CRTC */
	0x6B, 0x59, 0x5A, 0x82, 0x60, 0x8D, 0x0B, 0x3E,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x2D, 0x10, 0xE8, 0x05, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00,
};

unsigned char g_90x60_text[] =
{
/* MISC */
	0xE7,
/* SEQ */
	0x03, 0x01, 0x03, 0x00, 0x02,
/* CRTC */
	0x6B, 0x59, 0x5A, 0x82, 0x60, 0x8D, 0x0B, 0x3E,
	0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x2D, 0x08, 0xE8, 0x05, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00,
};
/*****************************************************************************
VGA REGISTER DUMPS FOR VARIOUS GRAPHICS MODES
*****************************************************************************/
unsigned char g_640x480x2[] =
{
/* MISC */
	0xE3,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};
/*****************************************************************************
*** NOTE: the mode described by g_320x200x4[]
is different from BIOS mode 05h in two ways:
- Framebuffer is at A000:0000 instead of B800:0000
- Framebuffer is linear (no screwy line-by-line CGA addressing)
*****************************************************************************/
unsigned char g_320x200x4[] =
{
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x09, 0x03, 0x00, 0x02,
/* CRTC */
	0x2D, 0x27, 0x28, 0x90, 0x2B, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x14, 0x00, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x02, 0x00,
	0xFF,
/* AC */
	0x00, 0x13, 0x15, 0x17, 0x02, 0x04, 0x06, 0x07,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x01, 0x00, 0x03, 0x00, 0x00
};
/* mode 12h */
unsigned char g_640x480x16[] =
{
/* MISC */
	0xE3,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

unsigned char g_720x480x16[] =
{
/* MISC */
	0xE7,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x6B, 0x59, 0x5A, 0x82, 0x60, 0x8D, 0x0B, 0x3E,
	0x00, 0x40, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x2D, 0x08, 0xE8, 0x05, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x01, 0x00, 0x0F, 0x00, 0x00,
};
/* mode 13h */
unsigned char g_320x200x256[] =
{
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00
};

unsigned char g_320x200x256_modex[] =
{
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28, 0x00, 0x96, 0xB9, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00, 0x00
};

/* g_360x480x256_modex - to do */


/*****************************************************************************
MAIN/DEMO ROUTINES
*****************************************************************************/
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
void dump_regs(unsigned char *regs)
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
*****************************************************************************/
void read_regs(unsigned char *regs)
{
	unsigned i;

/* read MISCELLANEOUS reg */
	*regs = inportb(VGA_MISC_READ);
	regs++;
/* read SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outportb(VGA_SEQ_INDEX, i);
		*regs = inportb(VGA_SEQ_DATA);
		regs++;
	}
/* read CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outportb(VGA_CRTC_INDEX, i);
		*regs = inportb(VGA_CRTC_DATA);
		regs++;
	}
/* read GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outportb(VGA_GC_INDEX, i);
		*regs = inportb(VGA_GC_DATA);
		regs++;
	}
/* read ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)inportb(VGA_INSTAT_READ);
		outportb(VGA_AC_INDEX, i);
		*regs = inportb(VGA_AC_READ);
		regs++;
	}
/* lock 16-color palette and unblank display */
	(void)inportb(VGA_INSTAT_READ);
	outportb(VGA_AC_INDEX, 0x20);
}
/*****************************************************************************
*****************************************************************************/
void write_regs(unsigned char *regs)
{
	unsigned i;

/* write MISCELLANEOUS reg */
	outportb(VGA_MISC_WRITE, *regs);
	regs++;
/* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
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
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outportb(VGA_CRTC_INDEX, i);
		outportb(VGA_CRTC_DATA, *regs);
		regs++;
	}
/* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outportb(VGA_GC_INDEX, i);
		outportb(VGA_GC_DATA, *regs);
		regs++;
	}
/* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)inportb(VGA_INSTAT_READ);
		outportb(VGA_AC_INDEX, i);
		outportb(VGA_AC_WRITE, *regs);
		regs++;
	}
/* lock 16-color palette and unblank display */
	(void)inportb(VGA_INSTAT_READ);
	outportb(VGA_AC_INDEX, 0x20);
}
/*****************************************************************************
*****************************************************************************/
void set_plane(unsigned p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;
/* set read plane */
	outportb(VGA_GC_INDEX, 4);
	outportb(VGA_GC_DATA, p);
/* set write plane */
	outportb(VGA_SEQ_INDEX, 2);
	outportb(VGA_SEQ_DATA, pmask);
}
/*****************************************************************************
VGA framebuffer is at A000:0000, B000:0000, or B800:0000
depending on bits in GC 6
*****************************************************************************/
static unsigned get_fb_seg(void)
{
	unsigned seg;

	outportb(VGA_GC_INDEX, 6);
	seg = inportb(VGA_GC_DATA);
	seg >>= 2;
	seg &= 3;
	switch(seg)
	{
	case 0:
	case 1:
		seg = 0xA000;
		break;
	case 2:
		seg = 0xB000;
		break;
	case 3:
		seg = 0xB800;
		break;
	}
	return seg;
}
/*****************************************************************************
*****************************************************************************/
void vmemwr(unsigned dst_off, unsigned char *src, unsigned count)
{
	_vmemwr(get_fb_seg(), dst_off, src, count);
}
/*****************************************************************************
*****************************************************************************/
void vpokeb(unsigned off, unsigned val)
{
	unsigned fb_seg = get_fb_seg();
	pokeb(fb_seg, off, val);
}
/*****************************************************************************
*****************************************************************************/
/*static unsigned vpeekb(unsigned off)
{
	unsigned fb_seg = get_fb_seg();
	return peekb(fb_seg, off);
}*/

/*****************************************************************************
*****************************************************************************/
static void (*g_write_pixel)(unsigned x, unsigned y, unsigned c);
static unsigned g_wd, g_ht;

/*static void write_pixel1(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off, mask;

	c = (c & 1) * 0xFF;
	wd_in_bytes = g_wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	vpokeb(off, (vpeekb(off) & ~mask) | (c & mask));
}*/
/*****************************************************************************
*****************************************************************************/
/*static void write_pixel2(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes, off, mask;

	c = (c & 3) * 0x55;
	wd_in_bytes = g_wd / 4;
	off = wd_in_bytes * y + x / 4;
	x = (x & 3) * 2;
	//mask = 0xC0 >> x;
	mask = 0x03 << (sizeof(mask) - x);
	vpokeb(off, (vpeekb(off) & ~mask) | (c & mask));
}*/
/*****************************************************************************
*****************************************************************************/
/*static void write_pixel4p(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes, off, mask, p, pmask;

	wd_in_bytes = g_wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	for(p = 0; p < 4; p++)
	{
		set_plane(p);
		if(pmask & c)
			vpokeb(off, vpeekb(off) | mask);
		else
			vpokeb(off, vpeekb(off) & ~mask);
		pmask <<= 1;
	}
}*/
/*****************************************************************************
*****************************************************************************/
static void write_pixel8(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = g_wd;
	off = wd_in_bytes * y + x;
	vpokeb(off, c);
}
/*****************************************************************************
*****************************************************************************/
/*static void write_pixel8x(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = g_wd / 4;
	off = wd_in_bytes * y + x / 4;
	set_plane(x & 3);
	vpokeb(off, c);
}*/

void vesa_clear_screen (void) {
	unsigned x, y;
	/* clear screen */
	for(y = 0; y < g_ht; y++)
		for(x = 0; x < g_wd; x++)
			g_write_pixel(x, y, 0);
}
/*****************************************************************************
*****************************************************************************/
static void draw_x(void)
{
	unsigned x, y;

/* clear screen */
	for(y = 0; y < g_ht; y++)
		for(x = 0; x < g_wd; x++)
			g_write_pixel(x, y, 0);
/* draw 2-color X */
	for(y = 0; y < g_ht; y++)
	{
		g_write_pixel((g_wd - g_ht) / 2 + y, y, 1);
		g_write_pixel((g_ht + g_wd) / 2 - y, y, 2);
	}
}

void vesa_put_pixel (unsigned x, unsigned y, unsigned c) {
	g_write_pixel (x, y, c);
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
/*****************************************************************************
SET TEXT MODES
*****************************************************************************/
void set_text_mode(int hi_res)
{
	unsigned rows, cols, ht, i;

	if(hi_res)
	{
		write_regs(g_90x60_text);
		cols = 90;
		rows = 60;
		ht = 8;
	}
	else
	{
		write_regs(g_80x25_text);
		cols = 80;
		rows = 25;
		ht = 16;
	}
/* set font */
	if(ht >= 16) {
		//write_font(g_8x16_font, 16);
		setup_font(16);
	}
	else {
		//write_font(g_8x8_font, 8);
		setup_font(8);
	}
/* tell the BIOS what we've done, so BIOS text output works OK */
	pokew(0x40, 0x4A, cols);	/* columns on screen */
	pokew(0x40, 0x4C, cols * rows * 2); /* framebuffer size */
	pokew(0x40, 0x50, 0);	   /* cursor pos'n */
	pokeb(0x40, 0x60, ht - 1);      /* cursor shape */
	pokeb(0x40, 0x61, ht - 2);
	pokeb(0x40, 0x84, rows - 1);    /* rows on screen - 1 */
	pokeb(0x40, 0x85, ht);	  /* char height */
/* set white-on-black attributes for all text */
	for(i = 0; i < cols * rows; i++)
		pokeb(0xB800, i * 2 + 1, 7);
}
/*****************************************************************************
DEMO GRAPHICS MODES
*****************************************************************************/
void vesa_demo_graphics(void)
{
	printf("Screen-clear in 16-color mode will be VERY SLOW\n"
		"don't Press a key to continue\n");
/* 4-color */
/*  write_regs(g_320x200x4);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel2;
	draw_x();*/
/* 16-color */
/*      write_regs(g_640x480x16);
	g_wd = 640;
	g_ht = 480;
	g_write_pixel = write_pixel4p;
	draw_x();*/
/* 256-color */
	write_regs(g_320x200x256);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel8;
	draw_x();
/* 256-color Mode-X */
/*      write_regs(g_320x200x256_modex);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel8x;
	draw_x();*/
/* go back to 80x25 text mode */
	/*set_text_mode(0);*/
}
/*****************************************************************************
*****************************************************************************/
unsigned char reverse_bits(unsigned char arg)
{
	unsigned char ret_val = 0;

	if(arg & 0x01)
		ret_val |= 0x80;
	if(arg & 0x02)
		ret_val |= 0x40;
	if(arg & 0x04)
		ret_val |= 0x20;
	if(arg & 0x08)
		ret_val |= 0x10;
	if(arg & 0x10)
		ret_val |= 0x08;
	if(arg & 0x20)
		ret_val |= 0x04;
	if(arg & 0x40)
		ret_val |= 0x02;
	if(arg & 0x80)
		ret_val |= 0x01;
	return ret_val;
}


unsigned int vesa_get_width (void) {
	return g_wd;
}
unsigned int vesa_get_height (void) {
	return g_ht;
}

static void vesa_save_old_regs (void) {
	read_regs(old_mode);
}

void vesa_init_mode (enum vesa_video_mode mode) {
	vesa_save_old_regs();
	write_regs(g_320x200x256);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel8;
}

void vesa_quit_mode (void) {
	write_regs(old_mode);
	g_write_pixel = NULL;
	set_text_mode(0);
}

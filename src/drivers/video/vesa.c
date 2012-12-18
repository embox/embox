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


//svga http://www.monstersoft.com/tutorial1/VESA_intro.html
//http://files.osdev.org/mirrors/geezer/osd/graphics/

//http://devotes.narod.ru/Books/3/ch05_10d.htm

//http://src-code.net/registry-videokontrollera/

#define inportb(P)	      in8(P)
#define outportb(P,V)	   out8(V, P)




//extern void setup_font(unsigned font_height);

//static unsigned char old_mode[64];




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

unsigned vpeekb(unsigned off)
{
	unsigned fb_seg = get_fb_seg();
	return peekb(fb_seg, off);
}

/*****************************************************************************
*****************************************************************************/
//static void (*g_write_pixel)(unsigned x, unsigned y, unsigned c);
//static unsigned g_wd, g_ht;




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








#if 0
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
      write_regs(g_640x480x16);
	g_wd = 640;
	g_ht = 480;
	g_write_pixel = write_pixel4p;
	draw_x();
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
#endif

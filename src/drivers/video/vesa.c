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
#include <drivers/video/vesa.h>
#include <asm/io.h> /* inp(), outp() */


//svga http://www.monstersoft.com/tutorial1/VESA_intro.html
//http://files.osdev.org/mirrors/geezer/osd/graphics/
//http://devotes.narod.ru/Books/3/ch05_10d.htm
//http://src-code.net/registry-videokontrollera/


/*****************************************************************************
*****************************************************************************/
void set_plane(unsigned p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;
/* set read plane */
	vga_wgfx(0, VGA_GFX_PLANE_READ, p);
/* set write plane */
	vga_wseq(0, VGA_SEQ_PLANE_MASK, pmask);
}

/*****************************************************************************
VGA framebuffer is at A000:0000, B000:0000, or B800:0000
depending on bits in GC registers index 6
*****************************************************************************/
static unsigned get_fb_seg(void)
{
	unsigned seg;

	seg = vga_rgfx(0, VGA_GFX_MISC);
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


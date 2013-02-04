/**
 * @file
 * @brief This file provides low-level video-mode routines for XWnd.
 *
 * @date Sep 14, 2012
 * @author Alexandr Chernakov
 */

#ifndef VESA_H_
#define VESA_H_

enum vesa_video_mode {
	VESA_MODE_TEXT_40x25x16_M = 0,
	VESA_MODE_TEXT_40x25x16 = 1,
	VESA_MODE_TEXT_80x25x16_M = 2,
	VESA_MODE_TEXT_80x25x16 = 3,
	VESA_MODE_CGA_320x200x4 = 4,
	VESA_MODE_CGA_320x200x4_M = 5,
	VESA_MODE_CGA_640x200x2 = 6,
	VESA_MODE_MDA_80x25 = 7,
	/* ... */
	VESA_MODE_EGA_320x200x16 = 0xD,
	VESA_MODE_EGA_640x200x16 = 0xE,
	VESA_MODE_VGA_640x350x1 = 0x10,
	VESA_MODE_VGA_640x480x1 = 0x11,
	VESA_MODE_VGA_640x480x16 = 0x12,
	VESA_MODE_VGA_320x200x256 = 0x13,
	/* ... */
	VESA_MODE_SVGA_640x480x256 = 0x101, /*101*/
	VESA_MODE_SVGA_800x600x16 = 0x102,
	VESA_MODE_SVGA_800x600x256 = 0x103,
	VESA_MODE_SVGA_1024x768x16 = 0x104,
	VESA_MODE_SVGA_1024x768x256 = 0x105,
	VESA_MODE_SVGA_1280x1024x16 = 0x106,
	VESA_MODE_SVGA_1280x1024x256 = 0x107,
	VESA_MODE_SVGA_TEXT_80x60 = 0x108,
	VESA_MODE_SVGA_TEXT_132x25 = 0x109,
	VESA_MODE_SVGA_TEXT_132x43 = 0x10A,
	VESA_MODE_SVGA_TEXT_132x50 = 0x10B,
	VESA_MODE_SVGA_TEXT_132x60 = 0x10C,
	VESA_MODE_SVGA_320x200x32k = 0x10D, /* (1:5:5:5) */
	VESA_MODE_SVGA_320x200x64K = 0x10E, /*  (5:6:5) */
	VESA_MODE_SVGA_320x200x16M = 0x10F, /* (8:8:8) */
	VESA_MODE_SVGA_640x480x32K = 0x110,
	VESA_MODE_SVGA_640x480x64K =  0x111,
	VESA_MODE_SVGA_640x480x16M = 0x112,
	VESA_MODE_SVGA_800x600x32K = 0x113,
	VESA_MODE_SVGA_800x600x64K = 0x114,
	VESA_MODE_SVGA_800x600x16M = 0x115,
	VESA_MODE_SVGA_1024x768x32K = 0x116,
	VESA_MODE_SVGA_1024x768x64K = 0x117,
	VESA_MODE_SVGA_1024x768x16M = 0x118,
	VESA_MODE_SVGA_1280x1024x32K = 0x119,
	VESA_MODE_SVGA_1280x1024x64K = 0x11A,
	VESA_MODE_SVGA_1280x1024x16M = 0x11B,
	VESA_MODE_SVGA_1600x1200x256 = 0x11C
};

#if 0
101    640x480   256     300K
110    640x480   32K     600K
111    640x480   64K     600K
112    640x480   16M     900K

103    800x600   256     469K
113    800x600   32K     938K
114    800x600   64K     938K
115    800x600   16M    1406K

105    1024x768  256     768K
116    1024x768  32K    1536K
117    1024x768  64K    1536K
118    1024x768  16M    2304K

107    1280x1024 256    1280K
119    1280x1024 32K    2560K
11A    1280x1024 64K    2560K
11B    1280x1024 16M    3840K
#endif

struct vesa_mode_desc {
	int xres; /* horizontal resolution */
	int yres; /* vertical resolution */
	int bpp;  /* number of color bits per pixel */
};

struct dislay;
extern struct display *vga_setup_mode(struct display *display, enum vesa_video_mode mode);

extern struct vesa_mode_desc *vesa_mode_get_desc(enum vesa_video_mode mode);


#endif /* VESA_H_ */

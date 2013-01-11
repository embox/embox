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
	VESA_MODE_SVGA_640x480x256 = 0x101,
	VESA_MODE_SVGA_1280x1024x16M = 0x11B

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

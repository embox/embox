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
	VESA_MODE_EGA_320x200x16 = 0xD,
	VESA_MODE_EGA_640x200x16 = 0xE,
	VESA_MODE_VGA_640x350x1 = 0x10,
	VESA_MODE_VGA_640x480x1 = 0x11,
	VESA_MODE_VGA_640x480x16 = 0x12,
	VESA_MODE_VGA_320x200x256 = 0x13
};

struct dislay;
extern struct display *vga_setup_mode(struct display *display, enum vesa_video_mode mode);
//extern void vesa_quit_mode(void);

//extern unsigned int vesa_get_width(void);
//extern unsigned int vesa_get_height(void);


//extern void vesa_demo_graphics(void);
//extern void vesa_dump_state(void);


#endif /* VESA_H_ */

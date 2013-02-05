/*
 * @file
 *
 * @date Jan 11, 2013
 * @author: Anton Bondarev
 */

#include <assert.h>
#include <types.h>
#include <drivers/video/vesa.h>
#include <util/array.h>

static struct vesa_mode_desc vesa_mode_text_descriptions[] = {
		{40,25,4},     /* VESA_MODE_TEXT_40x25x16_M */
		{40,25,4},     /* VESA_MODE_TEXT_40x25x16 */
		{80,25,4},     /* VESA_MODE_TEXT_80x25x16_M */
		{80,25,4},     /* VESA_MODE_TEXT_80x25x16 */
		{320, 200, 2}, /* VESA_MODE_CGA_320x200x4 */
		{320, 200, 2}, /* VESA_MODE_CGA_320x200x4_M */
		{640, 200, 1}, /* VESA_MODE_CGA_640x200x2 */
		{80, 25, 1}    /* VESA_MODE_MDA_80x25 */
};

static struct vesa_mode_desc vesa_mode_vga_descriptions[] = {
		{320, 200, 4}, /* VESA_MODE_EGA_320x200x16 */
		{640, 200, 4}, /* VESA_MODE_EGA_640x200x16 */
		{640, 350, 1}, /* VESA_MODE_VGA_640x350x1 */
		{640, 480, 1}, /* VESA_MODE_VGA_640x480x1 */
		{640, 480, 4}, /* VESA_MODE_VGA_640x480x16 */
		{320, 200, 8}, /* VESA_MODE_VGA_320x200x256 */
};

static struct vesa_mode_desc vesa_mode_svga_descriptions[] = {
		{640, 480, 8}, /* 0x101 VESA_MODE_SVGA_640x480x256 */
		{800, 600, 4}, /* 0x102   */
		{800, 600, 8}, /* 0x103   */
		{1024, 768, 4}, /* 0x104  */
		{1024, 768, 8}, /* 0x105 VESA_MODE_SVGA_1024x768x256 */
		{1280, 1024, 4}, /* 0x106 */
		{1280, 1024, 8}, /* 0x107 VESA_MODE_SVGA_1280x1024x256 */
		{80, 60, 0},    /* 0x108  */
		{132, 43, 0},  /* 0x109   */
		{132, 50, 0},  /* 0x10A   */
		{132, 60, 0},   /* 0x10B  */
		{320, 200, 15},  /* 0x10C */
		{320, 200, 16}, /* 0x10D   */
		{320, 200, 24}, /* 0x10E   */
		{640, 480, 15}, /* 0x10F   */
		{640, 480, 16}, /* 0x110   */
		{640, 480, 24}, /* 0x111   */
		{640, 480, 24}, /* 0x112   */
		{800, 600, 15}, /* 0x113   */
		{800, 600, 16}, /* 0x113   */
		{800, 600, 24}, /* 0x115   */
		{1024, 768, 15},   /* 0x116 */
		{1024, 768, 16},   /* 0x117 */
		{1024, 768, 24},  /* 0x118  */
		{1280, 1024, 15}, /* 0x119  */
		{1280, 1024, 16}, /* 0x11A  */
		{1280, 1024, 24}, /* 0x11B  */

		{1600, 1200, 8}

};


struct vesa_mode_desc *vesa_mode_get_desc(enum vesa_video_mode mode) {
	if (mode <= VESA_MODE_MDA_80x25) {
		/* setup text mode */
		assert(mode < ARRAY_SIZE(vesa_mode_text_descriptions));
		return &vesa_mode_text_descriptions[mode];
	}

	if (mode >= VESA_MODE_EGA_320x200x16 && mode <= VESA_MODE_VGA_320x200x256) {
		/* setup text mode */
		assert(mode - VESA_MODE_EGA_320x200x16 < ARRAY_SIZE(vesa_mode_vga_descriptions));
		return &vesa_mode_vga_descriptions[mode - VESA_MODE_EGA_320x200x16];
	}

	if (mode >= VESA_MODE_SVGA_640x480x256 && mode <= VESA_MODE_SVGA_1280x1024x16M) {
		assert(mode - VESA_MODE_SVGA_640x480x256 < ARRAY_SIZE(vesa_mode_svga_descriptions));
		return &vesa_mode_svga_descriptions[mode - VESA_MODE_SVGA_640x480x256];
	}

	return NULL;
}

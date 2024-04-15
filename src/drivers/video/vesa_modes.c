/*
 * @file
 * @brief
 *
 * @date 11.01.13
 * @author Anton Bondarev
 */

#include <assert.h>
#include <stddef.h>
#include <lib/libds/array.h>
#include <drivers/video/vesa_modes.h>

static const struct video_resbpp vesa_mode_text_descriptions[] = {
		{  40,  25, 4 }, /* 0x0 - VESA_MODE_TEXT_40x25x16_M */
		{  40,  25, 4 }, /* 0x1 - VESA_MODE_TEXT_40x25x16 */
		{  80,  25, 4 }, /* 0x2 - VESA_MODE_TEXT_80x25x16_M */
		{  80,  25, 4 }, /* 0x3 - VESA_MODE_TEXT_80x25x16 */
		{ 320, 200, 2 }, /* 0x4 - VESA_MODE_CGA_320x200x4 */
		{ 320, 200, 2 }, /* 0x5 - VESA_MODE_CGA_320x200x4_M */
		{ 640, 200, 1 }, /* 0x6 - VESA_MODE_CGA_640x200x2 */
		{  80,  25, 0 }  /* 0x7 - VESA_MODE_MDA_80x25 */
};

static const struct video_resbpp vesa_mode_vga_descriptions[] = {
		{ 320, 200, 4 }, /* 0xD - VESA_MODE_EGA_320x200x16 */
		{ 640, 200, 4 }, /* 0xE - VESA_MODE_EGA_640x200x16 */
		{ 640, 350, 0 }, /* 0xF - VESA_MODE_EGA_640x350 */
		{ 640, 350, 4 }, /* 0x10 - VESA_MODE_VGA_640x350x4 */
		{ 640, 480, 1 }, /* 0x11 - VESA_MODE_VGA_640x480x1 */
		{ 640, 480, 4 }, /* 0x12 - VESA_MODE_VGA_640x480x16 */
		{ 320, 200, 8 }  /* 0x13 - VESA_MODE_VGA_320x200x256 */
};

static const struct video_resbpp vesa_mode_svga_descriptions[] = {
		{  640,  480,  8 }, /* 0x101 - VESA_MODE_SVGA_640x480x256 */
		{  800,  600,  4 }, /* 0x102 - VESA_MODE_SVGA_800x600x16 */
		{  800,  600,  8 }, /* 0x103 - VESA_MODE_SVGA_800x600x256 */
		{ 1024,  768,  4 }, /* 0x104 - VESA_MODE_SVGA_1024x768x16 */
		{ 1024,  768,  8 }, /* 0x105 - VESA_MODE_SVGA_1024x768x256 */
		{ 1280, 1024,  4 }, /* 0x106 - VESA_MODE_SVGA_1280x1024x16 */
		{ 1280, 1024,  8 }, /* 0x107 - VESA_MODE_SVGA_1280x1024x256 */
		{   80,   60,  0 }, /* 0x108 - VESA_MODE_SVGA_TEXT_80x60 */
		{  132,   25,  0 }, /* 0x109 - VESA_MODE_SVGA_TEXT_132x25 */
		{  132,   43,  0 }, /* 0x10A - VESA_MODE_SVGA_TEXT_132x43 */
		{  132,   50,  0 }, /* 0x10B - VESA_MODE_SVGA_TEXT_132x50 */
		{  132,   60,  0 }, /* 0x10C - VESA_MODE_SVGA_TEXT_132x60 */
		{  320,  200, 15 }, /* 0x10D - VESA_MODE_SVGA_320x200x32K */
		{  320,  200, 16 }, /* 0x10E - VESA_MODE_SVGA_320x200x64K */
		{  320,  200, 24 }, /* 0x10F - VESA_MODE_SVGA_320x200x16M */
		{  640,  480, 15 }, /* 0x110 - VESA_MODE_SVGA_640x480x32K */
		{  640,  480, 16 }, /* 0x111 - VESA_MODE_SVGA_640x480x64K */
		{  640,  480, 24 }, /* 0x112 - VESA_MODE_SVGA_640x480x16M */
		{  800,  600, 15 }, /* 0x113 - VESA_MODE_SVGA_800x600x32K */
		{  800,  600, 16 }, /* 0x114 - VESA_MODE_SVGA_800x600x64K */
		{  800,  600, 24 }, /* 0x115 - VESA_MODE_SVGA_800x600x16M */
		{ 1024,  768, 15 }, /* 0x116 - VESA_MODE_SVGA_1024x768x32K */
		{ 1024,  768, 16 }, /* 0x117 - VESA_MODE_SVGA_1024x768x64K */
		{ 1024,  768, 24 }, /* 0x118 - VESA_MODE_SVGA_1024x768x16M */
		{ 1280, 1024, 15 }, /* 0x119 - VESA_MODE_SVGA_1280x1024x32K */
		{ 1280, 1024, 16 }, /* 0x11A - VESA_MODE_SVGA_1280x1024x64K */
		{ 1280, 1024, 24 }, /* 0x11B - VESA_MODE_SVGA_1280x1024x16M */
		{ 1600, 1200,  8 }  /* 0x11C - VESA_MODE_SVGA_1600x1200x256 */
};

const struct video_resbpp * video_resbpp_by_vesamode (enum video_vesa_mode mode) {
	static_assert(VESA_MODE_MDA_80x25 - VESA_MODE_TEXT_40x25x16_M
			== ARRAY_SIZE(vesa_mode_text_descriptions) - 1, "");
	if ((mode >= VESA_MODE_TEXT_40x25x16_M) && (mode <= VESA_MODE_MDA_80x25)) {
		return &vesa_mode_text_descriptions[mode - VESA_MODE_TEXT_40x25x16_M];
	}

	static_assert(VESA_MODE_VGA_320x200x256 - VESA_MODE_EGA_320x200x16
			== ARRAY_SIZE(vesa_mode_vga_descriptions) - 1, "");
	if ((mode >= VESA_MODE_EGA_320x200x16) && (mode <= VESA_MODE_VGA_320x200x256)) {
		return &vesa_mode_vga_descriptions[mode - VESA_MODE_EGA_320x200x16];
	}

	static_assert(VESA_MODE_SVGA_1600x1200x256 - VESA_MODE_SVGA_640x480x256
			== ARRAY_SIZE(vesa_mode_svga_descriptions) - 1, "");
	if ((mode >= VESA_MODE_SVGA_640x480x256) && (mode <= VESA_MODE_SVGA_1600x1200x256)) {
		return &vesa_mode_svga_descriptions[mode - VESA_MODE_SVGA_640x480x256];
	}

	return NULL;
}

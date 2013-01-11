/*
 * @file
 *
 * @date Jan 11, 2013
 * @author: Anton Bondarev
 */
#include <types.h>
#include <drivers/video/vesa.h>

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

//static struct vesa_mode_desc vesa_mode_svga_descriptions[] = {
//		{}
//};

struct vesa_mode_desc *vesa_mode_get_desc(enum vesa_video_mode mode) {
	if(mode <= VESA_MODE_MDA_80x25) {
		/* setup text mode */
		return &vesa_mode_text_descriptions[mode];
	}

	if(mode >= VESA_MODE_EGA_320x200x16 && mode <= VESA_MODE_VGA_320x200x256) {
		/* setup text mode */
		return &vesa_mode_vga_descriptions[mode];
	}

//	if(mode >= VESA_MODE_SVGA_640x480x256 && mode <= VESA_MODE_SVGA_1280x1024x16M) {
//		return &
//	}
	return NULL;
}


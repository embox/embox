/**
 * @file
 *
 * @date Dec 18, 2012
 * @author: Anton Bondarev
 */
#include <drivers/video/vga.h>
#include <drivers/video/vesa_mode.h>
#include <util/array.h>

ARRAY_SPREAD_DEF(const struct vga_mode_description *, __vga_mode_descriptions);

const struct vga_mode_description *vga_mode_description_lookup(enum vesa_video_mode mode) {
	const struct vga_mode_description *desc;

	array_foreach(desc, __vga_mode_descriptions, ARRAY_SPREAD_SIZE(__vga_mode_descriptions)) {
		if(desc->mode == mode) {
			return desc;
		}
	}

	return NULL;
}

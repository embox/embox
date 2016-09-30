/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */

#ifndef VIDEO_TERM_H_
#define VIDEO_TERM_H_

#include <drivers/vtesc.h>

struct vterm_video;

struct vterm_video_ops {
	void (*init)(struct vterm_video *t);
	void (*cursor)(struct vterm_video *t, unsigned short x, unsigned short y);
	void (*putc)( struct vterm_video *t, char ch, unsigned short x, unsigned short y);
	void (*clear_rows)(struct vterm_video *t, short row, unsigned short count);
	void (*copy_rows)(struct vterm_video *t, unsigned short to, unsigned short from, short nrows);
};

struct vterm_video {
	const struct vterm_video_ops *ops;

	unsigned short width;
	unsigned short height;

	unsigned short cur_x;
	unsigned short cur_y;

	struct vtesc_executor executor;
};

extern void vterm_video_putc(struct vterm_video *vi, char ch);

extern int vterm_video_init(struct vterm_video *vi, const struct vterm_video_ops *ops,
		unsigned short width, unsigned short height);

#endif /* VIDEO_TERM_H_ */


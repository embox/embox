/**
 * @file
 * @brief
 *
 * @author  Felix Sulima
 * @date    06.03.2013
 */

#ifndef CONSOLE_MPX_H_
#define CONSOLE_MPX_H_

#include <drivers/video/fb.h>
#include <drivers/input/input_dev.h>

__BEGIN_DECLS

#define VC_MPX_N 7

#define VC_FBUFLEN (1680 * 1024)

struct vc;

struct vc_callbacks {
	void (*handle_input_event)(struct vc *, struct input_event *);
	void (*visualized)(struct vc *, struct fb_info *);
	void (*schedule_devisualization)(struct vc *);
};

struct vc {
	const char *name;
	const struct vc_callbacks *callbacks;
	struct fb_info *fb; /**< current fb to draw, NULL is valid */
};


extern int mpx_register_vc(struct vc *);
extern int mpx_deregister_vc(struct vc *);
extern int mpx_devisualized(struct vc *vc);
extern int mpx_set_vmode(struct vc *, int mode);

__END_DECLS

#endif /* CONSOLE_MPX_H_ */

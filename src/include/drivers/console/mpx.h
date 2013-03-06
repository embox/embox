/*
 * mpx.h
 *
 *  Created on: 6 mars 2013
 *      Author: fsulima
 */

#ifndef MPX_H_
#define MPX_H_

#include <drivers/video/fb.h>

struct vc;

struct vc_callbacks {
	void (*handle_input_event)(struct vc *);
	void (*visualized)(struct vc *, struct fb_info *);
	void (*schedule_devisualization)(struct vc *);
};

struct vc {
	const char *name;
	struct vc_callbacks *callbacks;
};

int mpx_register_vc(struct vc *);
int mpx_deregister_vc(struct vc *);
void mpx_devisualized(struct vc *);
int mpx_set_vmode(struct vc *, int mode);

#endif /* MPX_H_ */

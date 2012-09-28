/*
 * vesa.h
 *
 *  Created on: Sep 14, 2012
 *      Author: user
 */

#ifndef VESA_H_
#define VESA_H_

typedef unsigned char pixel;
enum vesa_state { VESA_READY, VESA_NOT_INITIALIZED, VESA_MAX_STATE };

struct vesa_fb {
	unsigned int width;
	unsigned int height;
	pixel * buf;
};

int vesa_init_mode (void);
void vesa_quit_mode (void);
struct pixel * vesa_get_fb (void);
unsigned int vesa_get_width (void);
unsigned int vesa_get_height (void);
void demo_graphics(void);
void dump_state (void);
void set_text_mode (int hi_res);
void font512(void);


#endif /* VESA_H_ */

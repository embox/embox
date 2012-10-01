/*
 * vesa.h
 *
 *  Created on: Sep 14, 2012
 *      Author: user
 */

#ifndef VESA_H_
#define VESA_H_

int vesa_init_mode (void);
/*void vesa_quit_mode (void);*/

unsigned int vesa_get_width (void);
unsigned int vesa_get_height (void);

void vesa_clear_screen (void);
void vesa_put_pixel (unsigned x, unsigned y, unsigned c);

void vesa_demo_graphics (void);
void vesa_dump_state (void);

/*void set_text_mode (int hi_res);
void font512(void);*/


#endif /* VESA_H_ */

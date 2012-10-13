/**
 * @file
 * @brief This file provides low-level video-mode routines for XWnd.
 *
 * @date Sep 14, 2012
 * @author Alexandr Chernakov
 */

#ifndef VESA_H_
#define VESA_H_

enum vesa_video_mode {VESA_MODE_DEFAULT, VESA_MODE_MAX};

void vesa_init_mode (enum vesa_video_mode mode);
void vesa_quit_mode (void);

unsigned int vesa_get_width (void);
unsigned int vesa_get_height (void);

void vesa_clear_screen (void);
void vesa_put_pixel (unsigned x, unsigned y, unsigned c);

void vesa_demo_graphics (void);
void vesa_dump_state (void);

/*void set_text_mode (int hi_res);
void font512(void);*/


#endif /* VESA_H_ */

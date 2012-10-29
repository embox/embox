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

extern void vesa_init_mode (enum vesa_video_mode mode);
extern void vesa_quit_mode (void);

extern unsigned int vesa_get_width (void);
extern unsigned int vesa_get_height (void);

extern void vesa_clear_screen (void);
extern void vesa_put_pixel (unsigned x, unsigned y, unsigned c);

extern void vesa_demo_graphics (void);
extern void vesa_dump_state (void);

/*void set_text_mode (int hi_res);
void font512(void);*/


#endif /* VESA_H_ */

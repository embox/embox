/**
 * @file
 * @brief lib for drawing in framebuffer
 * @author Filipp Chubukov
 * @version
 * @date 22.08.2019
*/

#ifndef LIB_FB_DRAW_H_
#define LIB_FB_DRAW_H_

struct screen {
	uint8_t *fbp;
	uint32_t width;
	uint32_t height;
	int bpp;
};

/**
  * @brief init screen and put info screen struct
  *
  * @param *screen - pointer to the screen_info
  * @param screen_id - id of the new screen
  *
  * @return 0 - success or -1 if error
*/
extern int fb_draw_init_screen(struct screen *screen_info, int screen_id);

/**
  * @brief make 16bpp pix
  *
  * @param r - red channel
  * @param g - green channel
  * @param b - blue channel
  * @param bpp - bpp of rgb
  *
  * @return 16bpp pixel
*/
extern int fb_draw_put_pix(uint8_t r, uint8_t g, uint8_t b, int color_bpp, struct screen *screen_info, long int scr_pos);

#endif /* LIB_FB_DRAW_H_ */

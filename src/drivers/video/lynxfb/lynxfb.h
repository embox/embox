/**
 * @file
 *
 * @date Mar 29, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_VIDEO_LYNXFB_LYNXFB_H_
#define SRC_DRIVERS_VIDEO_LYNXFB_LYNXFB_H_

#include <stdint.h>
#include <stddef.h>

/* please use revision id to distinguish sm750le and sm750*/
#define LYNXFB_SPC_SM750          0
#define LYNXFB_SPC_SM712          1
#define LYNXFB_SPC_SM502          2

struct fb_info;

/**
 * lynx_share stands for a presentation of two frame buffer
 *  that use one smi adaptor , it is similar to a basic class of C++
 */
struct lynx_share {
	/* common members */
	uint16_t devid;
	uint8_t revid;

	struct fb_info *fbinfo[2];

	/* all smi graphic adaptor got below attributes */
	size_t vidmem_start;
	size_t vidreg_start;
	size_t vidmem_size;
	size_t vidreg_size;

	/* function pointers */
	void (*suspend)(struct lynx_share *);
	void (*resume)(struct lynx_share *);
};

struct lynxfb_crtc {
	unsigned char *vCursor; /*virtual address of cursor */
	unsigned char *vScreen; /*virtual address of on_screen */
	int oCursor; /*cursor address offset in vidmem */
	int oScreen; /*onscreen address offset in vidmem */
	int channel; /* which channel this crtc stands for */
	int pallete_was_updated;
	size_t vidmem_size; /* this view's video memory max size */

	/* below attributes belong to info->fix, their value depends on specific adaptor */
	uint16_t line_pad; /* padding information:0, 1, 2, 4, 8, 16, ... */
	uint16_t xpanstep;
	uint16_t ypanstep;
	uint16_t ywrapstep;

	void *priv;

	int (*proc_setMode)(struct lynxfb_crtc *, struct fb_var_screeninfo *,
			struct fb_fix_screeninfo *);
	int (*proc_checkMode)(struct lynxfb_crtc *, struct fb_var_screeninfo *);
	int (*proc_setColReg)(struct lynxfb_crtc *, uint16_t, uint16_t, uint16_t,
			uint16_t);
	void (*clear)(struct lynxfb_crtc *);
	/* pan display */
	int (*proc_panDisplay)(struct lynxfb_crtc *,
			const struct fb_var_screeninfo *, const struct fb_info *);
	/* cursor information */
	//struct lynx_cursor cursor;
};

struct lynxfb_output {
	int dpms;
	int paths;
	/*
	 * which paths(s) this output stands for, for sm750:
	 * paths=1:means output for panel paths
	 * paths=2:means output for crt paths
	 * paths=3:means output for both panel and crt paths
	 */

	int *channel;
	/*
	 * which channel these outputs linked with, for sm750:
	 * channel=0 means primary channel
	 * channel=1 means secondary channel
	 * output->channel ==> &crtc->channel
	 */
	void *priv;

	int (*proc_setMode)(struct lynxfb_output *, struct fb_var_screeninfo *,
			struct fb_fix_screeninfo *);

	int (*proc_checkMode)(struct lynxfb_output *, struct fb_var_screeninfo *);
	int (*proc_setBLANK)(struct lynxfb_output *, int);
	void (*clear)(struct lynxfb_output *);
};

struct lynxfb_par {
	/* either 0 or 1 for dual head adaptor, 0 is the older one registered */
	int index;
	unsigned int pseudo_palette[256];
	struct lynxfb_crtc crtc;
	struct lynxfb_output output;
	struct fb_info *info;
	struct lynx_share *share;
};

#endif /* SRC_DRIVERS_VIDEO_LYNXFB_LYNXFB_H_ */

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

/* 	lynx_share stands for a presentation of two frame buffer
	that use one smi adaptor , it is similar to a basic class of C++
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
	void (*suspend) (struct lynx_share *);
	void (*resume) (struct lynx_share *);
};

#endif /* SRC_DRIVERS_VIDEO_LYNXFB_LYNXFB_H_ */

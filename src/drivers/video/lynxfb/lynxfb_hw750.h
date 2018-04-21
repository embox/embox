/**
 * @file
 *
 * @date Mar 29, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_VIDEO_LYNXFB_LYNXFB_HW750_H_
#define SRC_DRIVERS_VIDEO_LYNXFB_LYNXFB_HW750_H_

#include <stdint.h>

#include "lynxfb.h"


#define DEFAULT_SM750_CHIP_CLOCK 		290
#define DEFAULT_SM750LE_CHIP_CLOCK  	333
#ifndef SM750LE_REVISION_ID
#define SM750LE_REVISION_ID ((uint8_t)0xfe)
#endif



enum sm750_pnltype {

	sm750_24TFT = 0,	/* 24bit tft */

	sm750_dualTFT = 2,	/* dual 18 bit tft */

	sm750_doubleTFT = 1,	/* 36 bit double pixel tft */
};

/* vga channel is not concerned  */
enum sm750_dataflow {
	sm750_simul_pri,	/* primary => all head */

	sm750_simul_sec,	/* secondary => all head */

	sm750_dual_normal,	/*   primary => panel head and secondary => crt */

	sm750_dual_swap,	/*     primary => crt head and secondary => panel */
};


enum sm750_channel {
	sm750_primary = 0,
	/* enum value equal to the register filed data */
	sm750_secondary = 1,
};

enum sm750_path {
	sm750_panel = 1,
	sm750_crt = 2,
	sm750_pnc = 3,		/* panel and crt */
};

struct sm750_init_status {
	unsigned short powerMode;
	/* below three clocks are in unit of MHZ */
	unsigned short chip_clk;
	unsigned short mem_clk;
	unsigned short master_clk;
	unsigned short setAllEngOff;
	unsigned short resetMemory;
};

struct sm750_state {
	struct sm750_init_status initParm;
	enum sm750_pnltype pnltype;
	enum sm750_dataflow dataflow;
	int nocrt;
	int xLCD;
	int yLCD;
};

struct sm750_share {
	/* it's better to put lynx_share struct to the first place of sm750_share */
	struct lynx_share share;
	struct sm750_state state;
	int hwCursor;
	/*
	 * 0: no hardware cursor
	 * 1: primary crtc hw cursor enabled,
	 * 2: secondary crtc hw cursor enabled
	 * 3: both ctrc hw cursor enabled
	 */
};

struct fb_var_screeninfo;
extern int hw_sm750_crtc_setMode(struct lynxfb_crtc *crtc,
		  const struct fb_var_screeninfo *var,
		  unsigned int line_length);

extern int hw_sm750_output_setMode(struct lynxfb_output *output,
			    const struct fb_var_screeninfo *var);

extern void lynxfb_hw750_setup(struct sm750_share *spec_share);
struct pci_slot_dev;
extern int lynxfb_hw750_map(struct sm750_share *spec_share, struct pci_slot_dev *pdev);
extern int lynxfb_hw750_inithw(struct sm750_share *spec_share);

#endif /* SRC_DRIVERS_VIDEO_LYNXFB_LYNXFB_HW750_H_ */

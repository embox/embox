/**
 * @file
 *
 * @date Mar 29, 2018
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <errno.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>

#include "lynxfb.h"
#include "lynxfb_hw750.h"
#include "ddk750_display.h"
#include "ddk750_mode.h"
#include "ddk750_reg.h"

#include "ddk750_chip.h"
void volatile *mmio750;


int lynxfb_hw750_map(struct sm750_share *spec_share, struct pci_slot_dev *pdev) {
	struct lynx_share *share = &spec_share->share;
	int ret = 0;

	share->vidreg_start = pci_resource_start(pdev, 1);
	share->vidreg_size = 0x00200000; /*SZ_2M */

	/* reserve the vidreg space of smi adaptor
	 * if you do this, u need to add release region code
	 * in lynxfb_remove, or memory will not be mapped again
	 * successfully
	 * */


	/* now map mmio and vidmem */
	share->pvReg = mmap_device_memory((void *)share->vidreg_start,
	    share->vidreg_size, PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED,
	    (uintptr_t)share->vidreg_start);
	if (MAP_FAILED == share->pvReg) {
		log_error("mmio failed");
		ret = -EFAULT;
		goto exit;
	}
#if 0
	share->accel.dprBase = share->pvReg + DE_BASE_ADDR_TYPE1;
	share->accel.dpPortBase = share->pvReg + DE_PORT_ADDR_TYPE1;

	ddk750_set_mmio(share->pvReg, share->devid, share->revid);
#endif
	mmio750 = share->pvReg;
	sm750_set_chip_type(share->devid, share->revid);

	share->vidmem_start = pci_resource_start(pdev, 0);
	/* don't use pdev_resource[x].end - resource[x].start to
	 * calculate the resource size, its only the maximum available
	 * size but not the actual size, use
	 * @hw_sm750_getVMSize function can be safe.
	 * */
	share->vidmem_size = ddk750_get_vm_size();//share);

	log_info("video memory size = %lld mb", share->vidmem_size >> 20);

	/* reserve the vidmem space of smi adaptor */

	share->pvMem = mmap_device_memory((void *)share->vidmem_start,
	    share->vidmem_size, PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED,
	    (uintptr_t)share->vidmem_start);

	if (MAP_FAILED == share->pvMem) {
		log_error("Map video memory failed");
		ret = -EFAULT;
		goto exit;
	}

	log_info("video memory vaddr = %p", share->pvMem);
exit:

	return ret;
}

int lynxfb_hw750_inithw(struct sm750_share *spec_share) {
	struct lynx_share *share = &spec_share->share;
	struct sm750_init_status *parm;

	parm = &spec_share->state.initParm;
	if (parm->chip_clk == 0)
		parm->chip_clk = (sm750_get_chip_type() == SM750LE) ?
						DEFAULT_SM750LE_CHIP_CLOCK :
						DEFAULT_SM750_CHIP_CLOCK;

	if (parm->mem_clk == 0)
		parm->mem_clk = parm->chip_clk;
	if (parm->master_clk == 0)
		parm->master_clk = parm->chip_clk / 3;

	//ddk750_init_hw((struct initchip_param *)&sm750_dev->initParm);
	ddk750_init_hw(parm);
	/* for sm718, open pci burst */
	if (share->devid == 0x718) {
		poke32(SYSTEM_CTRL,
		       peek32(SYSTEM_CTRL) | SYSTEM_CTRL_PCI_BURST);
	}

	if (sm750_get_chip_type() != SM750LE) {
		unsigned int val;
		/* does user need CRT? */
		if (spec_share->state.nocrt) {
			poke32(MISC_CTRL,
			       peek32(MISC_CTRL) | MISC_CTRL_DAC_POWER_OFF);
			/* shut off dpms */
			val = peek32(SYSTEM_CTRL) & ~SYSTEM_CTRL_DPMS_MASK;
			val |= SYSTEM_CTRL_DPMS_VPHN;
			poke32(SYSTEM_CTRL, val);
		} else {
			poke32(MISC_CTRL,
			       peek32(MISC_CTRL) & ~MISC_CTRL_DAC_POWER_OFF);
			/* turn on dpms */
			val = peek32(SYSTEM_CTRL) & ~SYSTEM_CTRL_DPMS_MASK;
			val |= SYSTEM_CTRL_DPMS_VPHP;
			poke32(SYSTEM_CTRL, val);
		}

		val = peek32(PANEL_DISPLAY_CTRL) &
			~(PANEL_DISPLAY_CTRL_DUAL_DISPLAY |
			  PANEL_DISPLAY_CTRL_DOUBLE_PIXEL);
		switch (spec_share->state.pnltype) {
		case sm750_24TFT:
			break;
		case sm750_doubleTFT:
			val |= PANEL_DISPLAY_CTRL_DOUBLE_PIXEL;
			break;
		case sm750_dualTFT:
			val |= PANEL_DISPLAY_CTRL_DUAL_DISPLAY;
			break;
		}
		poke32(PANEL_DISPLAY_CTRL, val);
	} else {
#if 0
		/*
		 * for 750LE, no DVI chip initialization
		 * makes Monitor no signal
		 *
		 * Set up GPIO for software I2C to program DVI chip in the
		 * Xilinx SP605 board, in order to have video signal.
		 */
		sm750_sw_i2c_init(0, 1);

		/*
		 * Customer may NOT use CH7301 DVI chip, which has to be
		 * initialized differently.
		 */
		if (sm750_sw_i2c_read_reg(0xec, 0x4a) == 0x95) {
			/*
			 * The following register values for CH7301 are from
			 * Chrontel app note and our experiment.
			 */
			pr_info("yes,CH7301 DVI chip found\n");
			sm750_sw_i2c_write_reg(0xec, 0x1d, 0x16);
			sm750_sw_i2c_write_reg(0xec, 0x21, 0x9);
			sm750_sw_i2c_write_reg(0xec, 0x49, 0xC0);
			pr_info("okay,CH7301 DVI chip setup done\n");
		}
#endif
	}
#if 0
	/* init 2d engine */
	if (!sm750_dev->accel_off)
		hw_sm750_initAccel(sm750_dev);
#endif
	return 0;
}

/* 	chip specific g_option configuration routine */
void lynxfb_hw750_setup(struct sm750_share *spec_share) {
	int swap = 0;

	spec_share->state.initParm.chip_clk = 0;
	spec_share->state.initParm.mem_clk = 0;
	spec_share->state.initParm.master_clk = 0;
	spec_share->state.initParm.powerMode = 0;
	spec_share->state.initParm.setAllEngOff = 0;
	spec_share->state.initParm.resetMemory = 1;

	if (spec_share->share.revid != SM750LE_REVISION_ID) {
		if (swap) {
			spec_share->state.dataflow = sm750_simul_sec;
		}
		else {
			spec_share->state.dataflow = sm750_simul_pri;
		}
	} else {
		/* SM750LE only have one crt channel */
		spec_share->state.dataflow = sm750_simul_sec;
		/* sm750le do not have complex attributes */
		spec_share->state.nocrt = 0;
	}
}

int lynxfb_hw750_set_drv(struct lynxfb_par *par) {
	return 0;
}


/* set the controller's mode for @crtc charged with @var and @fix parameters */
int hw_sm750_crtc_setMode(struct lynxfb_crtc *crtc,
			  const struct fb_var_screeninfo *var,
			  unsigned int line_length)
{
	int ret;//, fmt;
	uint32_t reg;
	struct mode_parameter modparm;
	clock_type_t clock;
	//struct lynxfb_par *par;

	ret = 0;
#if 0
	par = container_of(crtc, struct lynxfb_par, crtc);
	sm750_dev = par->dev;

	if (!sm750_dev->accel_off) {
		/* set 2d engine pixel format according to mode bpp */
		switch (var->bits_per_pixel) {
		case 8:
			fmt = 0;
			break;
		case 16:
			fmt = 1;
			break;
		case 32:
		default:
			fmt = 2;
			break;
		}
		sm750_hw_set2dformat(&sm750_dev->accel, fmt);
	}
#endif
	/* set timing */
	modparm.pixel_clock = ps_to_hz(var->pixclock);
	modparm.vertical_sync_polarity = (var->sync & FB_SYNC_HOR_HIGH_ACT)
					 ? POS : NEG;
	modparm.horizontal_sync_polarity = (var->sync & FB_SYNC_VERT_HIGH_ACT)
					   ? POS : NEG;
	modparm.clock_phase_polarity = (var->sync & FB_SYNC_COMP_HIGH_ACT)
				       ? POS : NEG;
	modparm.horizontal_display_end = var->xres;
	modparm.horizontal_sync_width = var->hsync_len;
	modparm.horizontal_sync_start = var->xres + var->right_margin;
	modparm.horizontal_total = var->xres + var->left_margin +
				   var->right_margin + var->hsync_len;
	modparm.vertical_display_end = var->yres;
	modparm.vertical_sync_height = var->vsync_len;
	modparm.vertical_sync_start = var->yres + var->lower_margin;
	modparm.vertical_total = var->yres + var->upper_margin +
				 var->lower_margin + var->vsync_len;

	/* choose pll */
	if (crtc->channel != sm750_secondary)
		clock = PRIMARY_PLL;
	else
		clock = SECONDARY_PLL;

	log_debug("Request pixel clock = %lu", modparm.pixel_clock);
	ret = ddk750_setModeTiming(&modparm, clock);
	if (ret) {
		log_error("Set mode timing failed");
		goto exit;
	}

	if (crtc->channel != sm750_secondary) {
		/* set pitch, offset, width, start address, etc... */
		poke32(PANEL_FB_ADDRESS,
		       crtc->oScreen & PANEL_FB_ADDRESS_ADDRESS_MASK);

		reg = var->xres * (var->bits_per_pixel >> 3);
		/*
		 * crtc->channel is not equal to par->index on numeric,
		 * be aware of that
		 */
		reg = ALIGN(reg, crtc->line_pad);
		reg = (reg << PANEL_FB_WIDTH_WIDTH_SHIFT) &
		       PANEL_FB_WIDTH_WIDTH_MASK;
		reg |= (line_length & PANEL_FB_WIDTH_OFFSET_MASK);
		poke32(PANEL_FB_WIDTH, reg);

		reg = ((var->xres - 1) << PANEL_WINDOW_WIDTH_WIDTH_SHIFT) &
		       PANEL_WINDOW_WIDTH_WIDTH_MASK;
		reg |= (var->xoffset & PANEL_WINDOW_WIDTH_X_MASK);
		poke32(PANEL_WINDOW_WIDTH, reg);

		reg = (var->yres_virtual - 1) <<
		      PANEL_WINDOW_HEIGHT_HEIGHT_SHIFT;
		reg &= PANEL_WINDOW_HEIGHT_HEIGHT_MASK;
		reg |= (var->yoffset & PANEL_WINDOW_HEIGHT_Y_MASK);
		poke32(PANEL_WINDOW_HEIGHT, reg);

		poke32(PANEL_PLANE_TL, 0);

		reg = ((var->yres - 1) << PANEL_PLANE_BR_BOTTOM_SHIFT) &
		       PANEL_PLANE_BR_BOTTOM_MASK;
		reg |= ((var->xres - 1) & PANEL_PLANE_BR_RIGHT_MASK);
		poke32(PANEL_PLANE_BR, reg);

		/* set pixel format */
		reg = peek32(PANEL_DISPLAY_CTRL);
		poke32(PANEL_DISPLAY_CTRL, reg | (var->bits_per_pixel >> 4));
	} else {
		/* not implemented now */
		poke32(CRT_FB_ADDRESS, crtc->oScreen);
		reg = var->xres * (var->bits_per_pixel >> 3);
		/*
		 * crtc->channel is not equal to par->index on numeric,
		 * be aware of that
		 */
		reg = ALIGN(reg, crtc->line_pad) << CRT_FB_WIDTH_WIDTH_SHIFT;
		reg &= CRT_FB_WIDTH_WIDTH_MASK;
		reg |= (line_length & CRT_FB_WIDTH_OFFSET_MASK);
		poke32(CRT_FB_WIDTH, reg);

		/* SET PIXEL FORMAT */
		reg = peek32(CRT_DISPLAY_CTRL);
		reg |= ((var->bits_per_pixel >> 4) &
			CRT_DISPLAY_CTRL_FORMAT_MASK);
		poke32(CRT_DISPLAY_CTRL, reg);
	}

exit:
	return ret;
}

int hw_sm750_output_setMode(struct lynxfb_output *output,
			    const struct fb_var_screeninfo *var)
{
	int ret;
	disp_output_t dispSet;
	int channel;

	ret = 0;
	dispSet = 0;
	channel = *output->channel;

	if (sm750_get_chip_type() != SM750LE) {
		if (channel == sm750_primary) {
			log_info("primary channel");
			if (output->paths & sm750_panel)
				dispSet |= do_LCD1_PRI;
			if (output->paths & sm750_crt)
				dispSet |= do_CRT_PRI;

		} else {
			log_info("secondary channel");
			if (output->paths & sm750_panel)
				dispSet |= do_LCD1_SEC;
			if (output->paths & sm750_crt)
				dispSet |= do_CRT_SEC;
		}
		ddk750_setLogicalDispOut(dispSet);
	} else {
		/* just open DISPLAY_CONTROL_750LE register bit 3:0 */
		uint32_t reg;

		reg = peek32(DISPLAY_CONTROL_750LE);
		reg |= 0xf;
		poke32(DISPLAY_CONTROL_750LE, reg);
	}

	log_info("ddk setlogicdispout done");
	return ret;
}

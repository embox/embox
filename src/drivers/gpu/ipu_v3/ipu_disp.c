/**
 * @file ipu_disp.c
 * @brief Configure channels, waves etc
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 28.08.2018
 */

#include <util/log.h>

#include <errno.h>
#include <stdint.h>

#include <drivers/lvds/imx/ldb.h>
#include <drivers/clk/ccm_imx6.h>
#include <drivers/video/fb.h>

#include "ipu_regs.h"
#include "ipu_priv.h"

#define SYNC_WAVE 0
#define NULL_WAVE (-1)

#define DC_DISP_ID_SYNC(di)	(di)

static void _ipu_di_data_wave_config(struct ipu_soc *ipu,
				int di, int wave_gen,
				int access_size, int component_size)
{
	uint32_t reg;
	reg = (access_size << DI_DW_GEN_ACCESS_SIZE_OFFSET) |
	    (component_size << DI_DW_GEN_COMPONENT_SIZE_OFFSET);
	ipu_di_write(ipu, di, reg, DI_DW_GEN(wave_gen));
}

static void _ipu_di_data_pin_config(struct ipu_soc *ipu,
			int di, int wave_gen, int di_pin, int set,
			int up, int down)
{
	uint32_t reg;

	reg = ipu_di_read(ipu, di, DI_DW_GEN(wave_gen));
	reg &= ~(0x3 << (di_pin * 2));
	reg |= set << (di_pin * 2);
	ipu_di_write(ipu, di, reg, DI_DW_GEN(wave_gen));

	ipu_di_write(ipu, di, (down << 16) | up, DI_DW_SET(wave_gen, set));
}

static void _ipu_di_sync_config(struct ipu_soc *ipu,
				int di, int wave_gen,
				int run_count, int run_src,
				int offset_count, int offset_src,
				int repeat_count, int cnt_clr_src,
				int cnt_polarity_gen_en,
				int cnt_polarity_clr_src,
				int cnt_polarity_trigger_src,
				int cnt_up, int cnt_down)
{
	uint32_t reg;

	reg = (run_count << 19) | (++run_src << 16) |
	    (offset_count << 3) | ++offset_src;
	ipu_di_write(ipu, di, reg, DI_SW_GEN0(wave_gen));
	reg = (cnt_polarity_gen_en << 29) | (++cnt_clr_src << 25) |
	    (++cnt_polarity_trigger_src << 12) | (++cnt_polarity_clr_src << 9);
	reg |= (cnt_down << 16) | cnt_up;
	if (repeat_count == 0) {
		/* Enable auto reload */
		reg |= 0x10000000;
	}
	ipu_di_write(ipu, di, reg, DI_SW_GEN1(wave_gen));
	reg = ipu_di_read(ipu, di, DI_STP_REP(wave_gen));
	reg &= ~(0xFFFF << (16 * ((wave_gen - 1) & 0x1)));
	reg |= repeat_count << (16 * ((wave_gen - 1) & 0x1));
	ipu_di_write(ipu, di, reg, DI_STP_REP(wave_gen));
}

static void _ipu_dc_map_config(struct ipu_soc *ipu,
		int map, int byte_num, int offset, int mask)
{
	log_debug("Enter %s", __func__);
	int ptr = map * 3 + byte_num;
	uint32_t reg;

	reg = ipu_dc_read(ipu, DC_MAP_CONF_VAL(ptr));
	reg &= ~(0xFFFF << (16 * (ptr & 0x1)));
	reg |= ((offset << 8) | mask) << (16 * (ptr & 0x1));
	ipu_dc_write(ipu, reg, DC_MAP_CONF_VAL(ptr));

	reg = ipu_dc_read(ipu, DC_MAP_CONF_PTR(map));
	reg &= ~(0x1F << ((16 * (map & 0x1)) + (5 * byte_num)));
	reg |= ptr << ((16 * (map & 0x1)) + (5 * byte_num));
	ipu_dc_write(ipu, reg, DC_MAP_CONF_PTR(map));
}

static void _ipu_dc_map_clear(struct ipu_soc *ipu, int map)
{
	log_debug("Enter %s", __func__);
	uint32_t reg = ipu_dc_read(ipu, DC_MAP_CONF_PTR(map));
	ipu_dc_write(ipu, reg & ~(0xFFFF << (16 * (map & 0x1))),
		     DC_MAP_CONF_PTR(map));
}

static void _ipu_dc_write_tmpl(struct ipu_soc *ipu,
			int word, uint32_t opcode, uint32_t operand, int map,
			int wave, int glue, int sync, int stop)
{
	log_debug("Enter %s", __func__);
	uint32_t reg;

	if (opcode == WRG) {
		reg = sync;
		reg |= (glue << 4);
		reg |= (++wave << 11);
		reg |= ((operand & 0x1FFFF) << 15);
		ipu_dc_tmpl_write(ipu, reg, word * 8);

		reg = (operand >> 17);
		reg |= opcode << 7;
		reg |= (stop << 9);
		ipu_dc_tmpl_write(ipu, reg, word * 8 + 4);
	} else {
		reg = sync;
		reg |= (glue << 4);
		reg |= (++wave << 11);
		reg |= (++map << 15);
		reg |= (operand << 20) & 0xFFF00000;
		ipu_dc_tmpl_write(ipu, reg, word * 8);

		reg = (operand >> 12);
		reg |= opcode << 4;
		reg |= (stop << 9);
		ipu_dc_tmpl_write(ipu, reg, word * 8 + 4);
	}

	log_debug("Exit %s", __func__);
}

static void _ipu_dc_link_event(struct ipu_soc *ipu,
		int chan, int event, int addr, int priority)
{
	uint32_t reg;
	reg = ipu_dc_read(ipu, DC_RL_CH(chan, event));
	reg &= ~(0xFFFF << (16 * (event & 0x1)));
	reg |= ((addr << 8) | priority) << (16 * (event & 0x1));
	ipu_dc_write(ipu, reg, DC_RL_CH(chan, event));
}

void _ipu_dc_init(struct ipu_soc *ipu, int dc_chan, int di, bool interlaced, uint32_t pixel_fmt)
{
	log_debug("enter %s, dc_chan=%d, di=%d\n", __func__, dc_chan, di);
	uint32_t reg;

	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NL, 5, 3);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_EOL, 6, 2);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NEW_DATA, 12, 1);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NF, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NFIELD, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_EOF, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_EOFIELD, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NEW_CHAN, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NEW_ADDR, 0, 0);

	reg = 0x2;
	reg |= DC_DISP_ID_SYNC(di) << DC_WR_CH_CONF_PROG_DISP_ID_OFFSET;
	reg |= di << 2;

	ipu_dc_write(ipu, reg, DC_WR_CH_CONF(dc_chan));

	ipu_dc_write(ipu, 0x00000000, DC_WR_CH_ADDR(dc_chan));

	ipu_dc_write(ipu, 0x00000084, DC_GEN);
}

void _ipu_dc_uninit(struct ipu_soc *ipu, int dc_chan)
{
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NL, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_EOL, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NEW_DATA, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NF, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NFIELD, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_EOF, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_EOFIELD, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NEW_CHAN, 0, 0);
	_ipu_dc_link_event(ipu, dc_chan, DC_EVT_NEW_ADDR, 0, 0);
}

void _ipu_dp_dc_enable(struct ipu_soc *ipu, ipu_channel_t channel)
{
	uint32_t reg;
	uint32_t dc_chan = 5;

	reg = ipu_cm_read(ipu, IPU_SRM_PRI2) | 0x8;
	ipu_cm_write(ipu, reg, IPU_SRM_PRI2);

	reg = ipu_dc_read(ipu, DC_WR_CH_CONF(dc_chan));
	reg |= 4 << DC_WR_CH_CONF_PROG_TYPE_OFFSET;
	ipu_dc_write(ipu, reg, DC_WR_CH_CONF(dc_chan));

	clk_enable("ipu1_di0");
}

enum {
	IPU_RGB24_MAP = 0,
	IPU_RGB565_MAP = 1,
};

void _ipu_init_dc_mappings(struct ipu_soc *ipu) {
	/* Configure maps (used in ipu_init_sync_panel)
	 *
	 * Configuration is done in the following way:
	 *	_ipu_dc_map_config(ipu, MAP, COLOR, OFFSET, CONST)
	 *
	 *	last const is currently unknown
	 *
	 *	MAP is just a number 0 ... 5
	 *
	 *	COLOR is: 0 for RED
	 *	          1 for GREEN
	 *	          2 for BLUE
	 *
	 *	OFFSET is the highest bit of the color
	 * */
	/* RGB24 */
	_ipu_dc_map_clear(ipu, IPU_RGB24_MAP);
	_ipu_dc_map_config(ipu, IPU_RGB24_MAP, 0, 7, 0xFF);
	_ipu_dc_map_config(ipu, IPU_RGB24_MAP, 1, 15, 0xFF);
	_ipu_dc_map_config(ipu, IPU_RGB24_MAP, 2, 23, 0xFF);
	/* RGB565 */
	_ipu_dc_map_clear(ipu, IPU_RGB565_MAP);
	_ipu_dc_map_config(ipu, IPU_RGB565_MAP, 0, 5, 0xFC);
	_ipu_dc_map_config(ipu, IPU_RGB565_MAP, 1, 11, 0xFC);
	_ipu_dc_map_config(ipu, IPU_RGB565_MAP, 2, 17, 0xFC);
}

int32_t ipu_init_sync_panel(struct ipu_soc *ipu, int disp,
			    struct fb_info *fbi,
			    uint32_t pixel_fmt) {
	uint16_t width = fbi->var.xres;
	uint16_t height = fbi->var.yres;
	uint16_t h_start_width = fbi->var.left_margin;
	uint16_t h_sync_width = fbi->var.hsync_len;
	uint16_t h_end_width = fbi->var.right_margin;
	uint16_t v_start_width = fbi->var.upper_margin;
	uint16_t v_sync_width = fbi->var.vsync_len;
	uint16_t v_end_width = fbi->var.lower_margin;
	uint32_t reg;
	uint32_t di_gen;
	uint32_t div;
	uint32_t h_total, v_total;
	int map;

	if ((v_sync_width == 0) || (h_sync_width == 0))
		return -EINVAL;

	if (v_end_width < 2) {
		v_end_width = 2;
		log_debug("Adjusted v_end_width");
	}

	h_total = width + h_sync_width + h_start_width + h_end_width;
	v_total = height + v_sync_width + v_start_width + v_end_width;

	/* try ipu clk first*/
	ipu_di_write(ipu, disp, 3 << 20, DI_GENERAL);

	div = 1;

	ipu_di_write(ipu, 0, div << 4, DI_BS_CLKGEN0);
	ipu_di_write(ipu, 0, div << 16, DI_BS_CLKGEN1);

	_ipu_di_data_wave_config(ipu, disp, SYNC_WAVE, div - 1, div - 1);
	_ipu_di_data_pin_config(ipu, disp, SYNC_WAVE, DI_PIN15, 3, 0, div * 2);

	/* XXX hardcoded for use with LVDS */
	if (ldb_bits() == 18) {
		map = IPU_RGB565_MAP;
	} else { /* Assume 24 bits */
		map = IPU_RGB24_MAP;
	}

	/*clear DI*/
	di_gen = ipu_di_read(ipu, disp, DI_GENERAL);
	di_gen &= (0x3 << 20);
	ipu_di_write(ipu, disp, di_gen, DI_GENERAL);
	/* Setup internal HSYNC waveform */
	_ipu_di_sync_config(ipu, disp, 1, h_total - 1, DI_SYNC_CLK,
				0, DI_SYNC_NONE, 0, DI_SYNC_NONE, 0, DI_SYNC_NONE,
				DI_SYNC_NONE, 0, 0);
	/* Setup external (delayed) HSYNC waveform */
	_ipu_di_sync_config(ipu, disp, DI_SYNC_HSYNC, h_total - 1,
			    DI_SYNC_CLK, div * 0, DI_SYNC_CLK,
			    0, DI_SYNC_NONE, 1, DI_SYNC_NONE,
			    DI_SYNC_CLK, 0, h_sync_width * 2);
	/* Setup VSYNC waveform */
	_ipu_di_sync_config(ipu, disp, DI_SYNC_VSYNC, v_total - 1,
			    DI_SYNC_INT_HSYNC, 0, DI_SYNC_NONE, 0,
			    DI_SYNC_NONE, 1, DI_SYNC_NONE,
			    DI_SYNC_INT_HSYNC, 0, v_sync_width * 2);
	ipu_di_write(ipu, disp, v_total - 1, DI_SCR_CONF);

	/* Setup active data waveform to sync with DC */
	_ipu_di_sync_config(ipu, disp, 4, 0, DI_SYNC_HSYNC,
			    v_sync_width + v_start_width, DI_SYNC_HSYNC, height,
			    DI_SYNC_VSYNC, 0, DI_SYNC_NONE,
			    DI_SYNC_NONE, 0, 0);
	_ipu_di_sync_config(ipu, disp, 5, 0, DI_SYNC_CLK,
			    h_sync_width + h_start_width, DI_SYNC_CLK,
			    width, 4, 0, DI_SYNC_NONE, DI_SYNC_NONE, 0, 0);

	_ipu_dc_write_tmpl(ipu, 5, WROD(0), 0, map, SYNC_WAVE, 8, 5, 1);
	_ipu_dc_write_tmpl(ipu, 6, WROD(0), 0, map, SYNC_WAVE, 4, 5, 0);
	_ipu_dc_write_tmpl(ipu, 7, WRG, 0, map, NULL_WAVE, 0, 0, 1);
	_ipu_dc_write_tmpl(ipu, 12, WROD(0), 0, map, SYNC_WAVE, 0, 5, 1);

	di_gen |= DI_GEN_POLARITY_DISP_CLK;
	di_gen |= (1 << 20);
	ipu_di_write(ipu, disp, di_gen, DI_GENERAL);

	ipu_di_write(ipu, disp, (DI_SYNC_HSYNC << DI_VSYNC_SEL_OFFSET) |
			0x00000002, DI_SYNC_AS_GEN);
	reg = ipu_di_read(ipu, disp, DI_POL);
	reg &= ~(DI_POL_DRDY_DATA_POLARITY | DI_POL_DRDY_POLARITY_15);
	reg |= DI_POL_DRDY_POLARITY_15;
	ipu_di_write(ipu, disp, reg, DI_POL);

	ipu_dc_write(ipu, width, DC_DISP_CONF2(DC_DISP_ID_SYNC(disp)));

	return 0;
}

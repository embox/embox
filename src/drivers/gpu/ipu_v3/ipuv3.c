/**
 * @file ipuv3.c
 * @brief Probe/irq handle/reset/channel init/etc
 * @date Sep 15, 2017
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <string.h>
#include <assert.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/video/fb.h>
#include <drivers/clk/ccm_imx6.h>
#include <kernel/irq.h>

#include "ipu_regs.h"
#include "ipu_priv.h"
#include "ipu_param_mem.h"

extern void _ipu_dc_init(struct ipu_soc *ipu, int dc_chan, int di, bool interlaced, uint32_t pixel_fmt);
extern void _ipu_dc_uninit(struct ipu_soc *ipu, int dc_chan);
extern void _ipu_init_dc_mappings(struct ipu_soc *ipu);
extern void _ipu_dp_dc_enable(struct ipu_soc *ipu, ipu_channel_t channel);

static struct ipu_soc ipu_soc;

#define idma_mask(ch)		(1UL << (ch & 0x1F))
#define idma_is_set(ipu, reg, dma)	(ipu_idmac_read(ipu, reg(dma)) & idma_mask(dma))

/**
 * @brief Perform software reset for all IPU modules
 */
static void ipu_reset(void) {
#define SRC_BASE 0x20D8000
# define SRC_IPU_RESET (1 << 3)
	REG32_ORIN(SRC_BASE, SRC_IPU_RESET);
	int timeout=0xfffff;
	while(timeout--) {
		if (!(REG32_LOAD(SRC_BASE) & SRC_IPU_RESET)) {
			break;
		}
	}

	if (timeout == 0) {
		log_error("IPU reset timeout!\n");
	} else {
		log_debug("IPU reset ok!\n");
	}
}

static int ipu_mem_reset(struct ipu_soc *ipu) {
	int i = 1000000;

	ipu_cm_write(ipu, 0x807FFFFF, IPU_MEM_RST);

	while (i-- > 0) {
		if (!(ipu_cm_read(ipu, IPU_MEM_RST) & (1u << 31)))
			break;
	}

	if (i <= 0) {
		log_error("Failed to reset IPU internal memories");
	}

	return 0;
}

struct ipu_soc *ipu_get() {
	return &ipu_soc;
}

int ipu_probe(void) {
	struct ipu_soc *ipu = ipu_get();

	clk_enable("ipu1");
	memset(ipu, 0, sizeof(struct ipu_soc));

	ipu->cm_reg		= (void*) IPU_BASE + IPU_CM_REG_BASE;
	ipu->ic_reg		= (void*) IPU_BASE + IPU_IC_REG_BASE;
	ipu->idmac_reg		= (void*) IPU_BASE + IPU_IDMAC_REG_BASE;
	ipu->dp_reg		= (void*) IPU_BASE + IPU_SRM_REG_BASE;
	ipu->dc_reg		= (void*) IPU_BASE + IPU_DC_REG_BASE;
	ipu->dmfc_reg		= (void*) IPU_BASE + IPU_DMFC_REG_BASE;
	ipu->di_reg[0]		= (void*) IPU_BASE + IPU_DI0_REG_BASE;
	ipu->di_reg[1]		= (void*) IPU_BASE + IPU_DI1_REG_BASE;
	ipu->smfc_reg		= (void*) IPU_BASE + IPU_SMFC_REG_BASE;
	ipu->cpmem_base		= (void*) IPU_BASE + IPU_CPMEM_REG_BASE;
	ipu->tpmem_base		= (void*) IPU_BASE + IPU_TPM_REG_BASE;
	ipu->dc_tmpl_reg	= (void*) IPU_BASE + IPU_DC_TMPL_REG_BASE;
	ipu->vdi_reg		= (void*) IPU_BASE + IPU_VDI_REG_BASE;
	ipu->disp_base[1]	= (void*) IPU_BASE + IPU_DISP1_BASE;

	ipu_reset();

	/* Disable all channels */
	ipu_idmac_write(ipu, 0x0, IDMAC_WM_EN(0));
	ipu_idmac_write(ipu, 0x0, IDMAC_WM_EN(32));

	ipu_idmac_write(ipu, 0x0, IDMAC_WM_EN(0));
	ipu_idmac_write(ipu, 0x0, IDMAC_WM_EN(32));

	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_CHA_CUR_BUF(0));
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_CHA_CUR_BUF(32));

	ipu_cm_write(ipu, 0x0, IPU_CHA_DB_MODE_SEL(0));
	ipu_cm_write(ipu, 0x0, IPU_CHA_DB_MODE_SEL(32));

	memset((void*) ipu->cpmem_base, 0, IPU_CPMEM_REG_LEN);

	clk_enable("ipu1");
	clk_enable("ipu1_di0");
	clk_enable("ldb_di0");

	ipu_mem_reset(ipu);

	_ipu_init_dc_mappings(ipu);

	/* Enable error interrupts by default */
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(5));
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(6));
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(9));
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(10));

	return 0;
}

int32_t ipu_init_channel(struct ipu_soc *ipu, ipu_channel_t channel, ipu_channel_params_t *params) {
	/* note: assume channel is MEM_BG_SYNC
	 *       assume DI=0			*/
	log_debug("init channel = %d", IPU_CHAN_VIDEO_IN_DMA(channel));
	/* Re-enable error interrupts every time a channel is initialized */
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(5));
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(6));
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(9));
	ipu_cm_write(ipu, 0xFFFFFFFF, IPU_INT_CTRL(10));

	ipu->dc_di_assignment[5] = 0;

	_ipu_dc_init(ipu, 5, 0, 0, IPU_PIX_FMT_RGB666);

	return 0;
}

void ipu_uninit_channel(struct ipu_soc *ipu, ipu_channel_t channel) {
	uint32_t ipu_conf;

	_ipu_dc_uninit(ipu, 5);

	ipu_conf = ipu_cm_read(ipu, IPU_CONF);
	ipu_conf &= ~IPU_CONF_DC_EN;
	ipu_conf &= ~IPU_CONF_DP_EN;
	ipu_conf &= ~IPU_CONF_DMFC_EN;
	ipu_conf &= ~IPU_CONF_DI0_EN;
	ipu_cm_write(ipu, ipu_conf, IPU_CONF);
}

int32_t ipu_init_channel_buffer(struct ipu_soc *ipu, ipu_channel_t channel,
				ipu_buffer_t type,
				uint32_t pixel_fmt,
				uint16_t width, uint16_t height,
				uint32_t stride,
				dma_addr_t phyaddr_0) {
	uint32_t dma_chan = IPU_CHAN_VIDEO_IN_DMA(channel);

	assert(dma_chan == 23); /* XXX */

	_ipu_ch_param_init(ipu, dma_chan, pixel_fmt, width, height, stride, 0, 0, 0,
			   phyaddr_0);

	return 0;
}

int32_t ipu_enable_channel(struct ipu_soc *ipu, ipu_channel_t channel) {
	uint32_t reg;
	uint32_t ipu_conf;
	uint32_t in_dma = IPU_CHAN_VIDEO_IN_DMA(channel);

	assert(in_dma == 23); /* XXX */

	ipu_conf = IPU_CONF_DP_EN | IPU_CONF_DC_EN | IPU_CONF_DMFC_EN;
	ipu_conf |= IPU_CONF_DI0_EN;
	ipu_cm_write(ipu, ipu_conf, IPU_CONF);

	reg = ipu_idmac_read(ipu, IDMAC_CHA_EN(in_dma));
	ipu_idmac_write(ipu, reg | idma_mask(in_dma), IDMAC_CHA_EN(in_dma));
	reg = ipu_idmac_read(ipu, IDMAC_WM_EN(in_dma));
	ipu_idmac_write(ipu, reg | idma_mask(in_dma), IDMAC_WM_EN(in_dma));

	_ipu_dp_dc_enable(ipu, channel);

	ipu_cm_write(ipu, 0x1400000, IPU_DISP_GEN);

	return 0;
}

void _ipu_clear_buffer_ready(struct ipu_soc *ipu, ipu_channel_t channel, ipu_buffer_t type,
		uint32_t bufNum) {
	uint32_t dma_ch = IPU_CHAN_VIDEO_IN_DMA(channel);

	assert(dma_ch == 23); /* XXX */

	ipu_cm_write(ipu, 0xF0300000, IPU_GPR); /* write one to clear */
	ipu_cm_write(ipu, idma_mask(dma_ch), IPU_CHA_BUF0_RDY(dma_ch));
	ipu_cm_write(ipu, 0x0, IPU_GPR); /* write one to set */
}

void ipu_clear_buffer_ready(struct ipu_soc *ipu, ipu_channel_t channel, ipu_buffer_t type,
		uint32_t bufNum) {
	_ipu_clear_buffer_ready(ipu, channel, type, bufNum);
}

int32_t ipu_disable_channel(struct ipu_soc *ipu, ipu_channel_t channel, bool wait_for_stop) {
	uint32_t reg;
	uint32_t in_dma = IPU_CHAN_VIDEO_IN_DMA(channel);

	assert(in_dma == 23); /* XXX */

	reg = ipu_idmac_read(ipu, IDMAC_WM_EN(in_dma));
	ipu_idmac_write(ipu, reg & ~idma_mask(in_dma), IDMAC_WM_EN(in_dma));

	reg = ipu_idmac_read(ipu, IDMAC_CHA_EN(in_dma));
	ipu_idmac_write(ipu, reg & ~idma_mask(in_dma), IDMAC_CHA_EN(in_dma));
	ipu_cm_write(ipu, idma_mask(in_dma), IPU_CHA_CUR_BUF(in_dma));

	_ipu_clear_buffer_ready(ipu, channel, IPU_VIDEO_IN_BUFFER, 0);

	return 0;
}

void ipu_disable_irq(struct ipu_soc *ipu, uint32_t irq) {
	uint32_t reg;

	reg = ipu_cm_read(ipu, IPUIRQ_2_CTRLREG(irq));
	reg &= ~IPUIRQ_2_MASK(irq);
	ipu_cm_write(ipu, reg, IPUIRQ_2_CTRLREG(irq));
}

void ipu_clear_irq(struct ipu_soc *ipu, uint32_t irq) {
	ipu_cm_write(ipu, IPUIRQ_2_MASK(irq), IPUIRQ_2_STATREG(irq));
}

int ipu_request_irq(struct ipu_soc *ipu, uint32_t irq,
		    /* irq_return_t(*handler) (int, void *), */
		    uint32_t irq_flags, const char *devname, void *dev_id) {
	uint32_t reg;

	ipu_cm_write(ipu, IPUIRQ_2_MASK(irq), IPUIRQ_2_STATREG(irq));
	reg = ipu_cm_read(ipu, IPUIRQ_2_CTRLREG(irq));
	reg |= IPUIRQ_2_MASK(irq);
	ipu_cm_write(ipu, reg, IPUIRQ_2_CTRLREG(irq));

	return 0;
}

void ipu_free_irq(struct ipu_soc *ipu, uint32_t irq, void *dev_id) {
	uint32_t reg;

	reg = ipu_cm_read(ipu, IPUIRQ_2_CTRLREG(irq));
	reg &= ~IPUIRQ_2_MASK(irq);
	ipu_cm_write(ipu, reg, IPUIRQ_2_CTRLREG(irq));
}

PERIPH_MEMORY_DEFINE(ipu, IPU_BASE, 0x200000);

#define DCIC1_BASE 0x20E4000
#define DCIC2_BASE 0x20E8000

PERIPH_MEMORY_DEFINE(ipu_dcic1, DCIC1_BASE, 0x20);

PERIPH_MEMORY_DEFINE(ipu_dcic2, DCIC2_BASE, 0x20);

PERIPH_MEMORY_DEFINE(ipu_src, SRC_BASE, 0x4);

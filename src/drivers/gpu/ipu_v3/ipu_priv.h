/*
 * Copyright 2005-2013 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
#ifndef __INCLUDE_IPU_PRV_H__
#define __INCLUDE_IPU_PRV_H__
#include <kernel/irq.h>
#include "uboot_ipu_compat.h"
#include <util/log.h>
/* Globals */
extern int dmfc_type_setup;

#define IDMA_CHAN_INVALID	0xFF
#define HIGH_RESOLUTION_WIDTH	1024

struct ipu_irq_node {
	irq_return_t (*handler) (int, void *);    /*!< the ISR */
	const char *name;       /*!< device associated with the interrupt */
	void *dev_id;           /*!< some unique information for the ISR */
	uint32_t flags;            /*!< not used */
};


enum csc_type_t {
	RGB2YUV = 0,
	YUV2RGB,
	RGB2RGB,
	YUV2YUV,
	CSC_NONE,
	CSC_NUM
};

enum imx_ipu_type {
	IMX6Q_IPU,
};

struct ipu_pltfm_data {
	u32 id;
	u32 devtype;
	int (*init) (int);
	void (*pg) (int);

	/*
	 * Bypass reset to avoid display channel being
	 * stopped by probe since it may starts to work
	 * in bootloader.
	 */
	bool bypass_reset;
};

struct ipu_soc {
	/*reg*/
	void  *cm_reg;
	void  *idmac_reg;
	void  *dp_reg;
	void  *ic_reg;
	void  *dc_reg;
	void  *dc_tmpl_reg;
	void  *dmfc_reg;
	void  *di_reg[2];
	void  *smfc_reg;
	void  *csi_reg[2];
	void  *cpmem_base;
	void  *tpmem_base;
	void  *disp_base[2];
	void  *vdi_reg;

	struct device *dev;

	ipu_channel_t csi_channel[2];
	ipu_channel_t using_ic_dirct_ch;
	unsigned char dc_di_assignment[10];
	uint32_t channel_enable_mask;
};

static inline u32 ipu_cm_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->cm_reg + offset);
}

static inline void ipu_cm_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->cm_reg + offset);
}

static inline u32 ipu_idmac_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->idmac_reg + offset);
}

static inline void ipu_idmac_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->idmac_reg + offset);
}

static inline u32 ipu_dc_read(struct ipu_soc *ipu, unsigned offset) {
	log_debug("%p", (void*) offset);
	return readl(ipu->dc_reg + offset);
}

static inline void ipu_dc_write(struct ipu_soc *ipu,
		u32 value, unsigned offset) {
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->dc_reg + offset);
}

static inline u32 ipu_dc_tmpl_read(struct ipu_soc *ipu, unsigned offset) {
	log_debug("%p", (void*) offset);
	return readl(ipu->dc_tmpl_reg + offset);
}

static inline void ipu_dc_tmpl_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->dc_tmpl_reg + offset);
}

static inline u32 ipu_dmfc_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->dmfc_reg + offset);
}

static inline void ipu_dmfc_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->dmfc_reg + offset);
}

static inline u32 ipu_dp_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->dp_reg + offset);
}

static inline void ipu_dp_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->dp_reg + offset);
}

static inline u32 ipu_di_read(struct ipu_soc *ipu, int di, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->di_reg[di] + offset);
}

static inline void ipu_di_write(struct ipu_soc *ipu, int di,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->di_reg[di] + offset);
}

static inline u32 ipu_csi_read(struct ipu_soc *ipu, int csi, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->csi_reg[csi] + offset);
}

static inline void ipu_csi_write(struct ipu_soc *ipu, int csi,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->csi_reg[csi] + offset);
}

static inline u32 ipu_smfc_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->smfc_reg + offset);
}

static inline void ipu_smfc_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->smfc_reg + offset);
}

static inline u32 ipu_vdi_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->vdi_reg + offset);
}

static inline void ipu_vdi_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->vdi_reg + offset);
}

static inline u32 ipu_ic_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return readl(ipu->ic_reg + offset);
}

static inline void ipu_ic_write(struct ipu_soc *ipu,
		u32 value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	writel(value, ipu->ic_reg + offset);
}

#endif				/* __INCLUDE_IPU_PRV_H__ */

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

#include <stdint.h>

#include <kernel/irq.h>
#include <hal/reg.h>
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
	uint32_t id;
	uint32_t devtype;
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

static inline uint32_t ipu_cm_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->cm_reg + offset);
}

static inline void ipu_cm_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->cm_reg + offset, value);
}

static inline uint32_t ipu_idmac_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->idmac_reg + offset);
}

static inline void ipu_idmac_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->idmac_reg + offset, value);
}

static inline uint32_t ipu_dc_read(struct ipu_soc *ipu, unsigned offset) {
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->dc_reg + offset);
}

static inline void ipu_dc_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset) {
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->dc_reg + offset, value);
}

static inline uint32_t ipu_dc_tmpl_read(struct ipu_soc *ipu, unsigned offset) {
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->dc_tmpl_reg + offset);
}

static inline void ipu_dc_tmpl_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->dc_tmpl_reg + offset, value);
}

static inline uint32_t ipu_dmfc_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->dmfc_reg + offset);
}

static inline void ipu_dmfc_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->dmfc_reg + offset, value);
}

static inline uint32_t ipu_dp_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->dp_reg + offset);
}

static inline void ipu_dp_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->dp_reg + offset, value);
}

static inline uint32_t ipu_di_read(struct ipu_soc *ipu, int di, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->di_reg[di] + offset);
}

static inline void ipu_di_write(struct ipu_soc *ipu, int di,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->di_reg[di] + offset, value);
}

static inline uint32_t ipu_csi_read(struct ipu_soc *ipu, int csi, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->csi_reg[csi] + offset);
}

static inline void ipu_csi_write(struct ipu_soc *ipu, int csi,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->csi_reg[csi] + offset, value);
}

static inline uint32_t ipu_smfc_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->smfc_reg + offset);
}

static inline void ipu_smfc_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->smfc_reg + offset, value);
}

static inline uint32_t ipu_vdi_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->vdi_reg + offset);
}

static inline void ipu_vdi_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->vdi_reg + offset, value);
}

static inline uint32_t ipu_ic_read(struct ipu_soc *ipu, unsigned offset)
{
	log_debug("%p", (void*) offset);
	return REG32_LOAD(ipu->ic_reg + offset);
}

static inline void ipu_ic_write(struct ipu_soc *ipu,
		uint32_t value, unsigned offset)
{
	log_debug("0x%x->%p", value, (void*) offset);
	REG32_STORE(ipu->ic_reg + offset, value);
}

#endif				/* __INCLUDE_IPU_PRV_H__ */

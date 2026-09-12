/**
 * @file
 * @brief
 *
 * @date    06.08.2026
 * @author  Anton Bondarev
 */

#ifndef DRIVERS_DMA_DMA_NEW_H_
#define DRIVERS_DMA_DMA_NEW_H_

#include <stdint.h>
#include <stdlib.h>

#include <lib/libds/array_spread.h>
#include <util/macro.h>

struct dma_dev;

struct dma_config {
    uint32_t    dc_opts;
    uint32_t    dc_chan;
    int (*dc_callback)(struct dma_dev *dev, void *data, int irq_num, int res);
};

struct dma_req {
    struct dma_dev *dr_dma_dev;
    int             dr_chan;
     
	uintptr_t       dr_src;
	int             dr_src_width;
	int             dr_src_inc;
	int             dr_src_type;
	uintptr_t       dr_dest;
	int             dr_dest_width;
	int             dr_dest_inc;
	int             dr_dest_type;
	int             dr_size;

	uint32_t        dr_flags;
	uint32_t        dr_status;
	int            (*dr_callback)(struct dma_req *req, void *data, int res);
};

struct dma_ops {
    int (*do_config)(struct dma_dev *dev, struct dma_config *conf);
    int (*do_transfer)(struct dma_dev *dev, int ch, struct dma_req *req);
    int (*do_activate)(struct dma_dev *dev, uint32_t ch_mask);
    int (*do_get_type)(struct dma_dev *dev, char *type);   
};

struct dma_dev {
    int                 dd_idx;
    struct dma_ops     *dd_ops;
    uintptr_t           dd_label;
    uint32_t            dd_chan_mask;
    void               *dd_priv;

    uint32_t            dd_flags;
    struct dma_config   dd_config;
};

extern struct dma_dev *dma_find_by_id( int) ;

extern int dma_config(struct dma_dev *dev, struct dma_config *conf);
extern int dma_transfer(struct dma_dev *dev, int ch, struct dma_req *req);
extern int dma_activate(struct dma_dev *dev, uint32_t ch_mask);
extern int dma_get_type(struct dma_dev *dev, char *type);

#define PWM_DEV_GLOBAL_PTR(id) \
					MACRO_CONCAT(ptr_pwm_dev_, id)

#define DMA_DEV_DEF(id, label, ops, priv, dd_chan_mask) \
	static struct dma_dev MACRO_CONCAT(dma_dev_, id) = \
						{ \
							.dd_idx = id,  \
							.dd_ops = ops, \
							.dd_label = label, \
							.dd_priv = priv, \
                            .dd_chan_max = dd_chan_mask, \
						} ; \
	ARRAY_SPREAD_DECLARE(const struct dma_dev *, __dma_device_registry); \
	ARRAY_SPREAD_ADD(__dma_device_registry,  &MACRO_CONCAT(dma_dev_, id)); \
	const struct dma_dev *DMA_DEV_GLOBAL_PTR(id) = &MACRO_CONCAT(dma_dev_, id)

#endif /* DRIVERS_DMA_DMA_NEW_H_ */

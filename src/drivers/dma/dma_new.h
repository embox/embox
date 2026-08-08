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
#include <kernel/irq.h>

struct dma_dev;

struct dma_config {
    uint32_t    dc_opts;
    uint32_t    dc_chan;
    uint32_t    dc_word_size;
    int (*dc_callback)(struct dma_dev *dev, void *data, int irq_num, int res);
};

struct dma_msg {
    int       dm_idx;
    uintptr_t dm_dst;
    uintptr_t dm_src;
    int       dm_len;
    uint32_t  dm_opts;
    uint32_t  dm_word_size;

    int (*dm_callback)(struct dma_dev *dev, void *data, int res);
};

struct dma_ops {
    int (*do_dma_config)(struct dma_dev *dev, struct dma_config *conf);
    int (*do_transfer)(struct dma_dev *dev, struct dma_msg *msg);
};

struct dma_dev {
    int                 dd_idx;
    struct dma_ops     *dd_ops;
    uintptr_t           dd_label;
    void               *dd_priv;
    uint32_t            dd_flags;
    struct dma_config   dd_config;
};

extern struct dma_dev *dma_find_by_id( int) ;

extern int dma_config(struct dma_dev *dev, struct dma_config *conf);
extern int dma_transfer(struct dma_dev *dev, struct dma_msg *msg);

#endif /* DRIVERS_DMA_DMA_NEW_H_ */

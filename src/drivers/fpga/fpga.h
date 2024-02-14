/**
 * @file fpga.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 27.01.2020
 */

#ifndef FPGA_H
#define FPGA_H

#include <stdint.h>

#include <drivers/device.h>
#include <kernel/task/resource/idesc.h>

struct fpga_ops;
struct fpga {
	struct dev_module *dev;
	size_t             id;
	struct fpga_ops   *ops;
	void              *priv;
};

struct fpga_ops {
	/* This function should prepare FPGA for data transfer */
	int (*config_init)(struct fpga *fpga);
	/* This function should transfer the config from HPS to FPGA */
	int (*config_write)(struct fpga *fpga, const uint8_t *buf, size_t len);
	/* This function should prepare FPGA for user mode */
	int (*config_complete)(struct fpga *fpga);
};

extern struct fpga *fpga_register(struct fpga_ops *ops, void *priv);
extern int fpga_free(struct fpga *fpga);
extern struct fpga *fpga_by_id(size_t id);
extern size_t fpga_max_id(void);

#endif /* FPGA_H */

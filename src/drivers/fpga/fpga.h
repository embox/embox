/**
 * @file fpga.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 27.01.2020
 */

#ifndef DRIVERS_FPGA_FPGA_H_
#define DRIVERS_FPGA_FPGA_H_

#include <stddef.h>
#include <stdint.h>

#include <drivers/char_dev.h>
#include <kernel/task/resource/idesc.h>

struct fpga_ops;
struct fpga {
	struct char_dev cdev;
	size_t id;
	struct fpga_ops *ops;
	void *priv;
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

#endif /* DRIVERS_FPGA_FPGA_H_ */

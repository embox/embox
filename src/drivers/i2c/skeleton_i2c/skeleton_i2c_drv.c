/**
 * @file
 *
 * @date Oct 23, 2025
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <drivers/i2c/i2c.h>


static int skeleton_i2c_tx(const struct i2c_bus *bus, uint16_t addr,
    uint8_t *buff, size_t sz) {

	return 0;
}

static int skeleton_i2c_rx(const struct i2c_bus *bus, uint16_t addr,
    uint8_t *buff, size_t sz) {

	return 0;
}

static int skeleton_i2c_master_xfer(const struct i2c_bus *bus, struct i2c_msg *msgs,
    size_t num_msgs) {
	int res;
	int i;

	for (i = 0; i < num_msgs; i++) {
		if (msgs[i].flags & I2C_M_RD) {
			res = skeleton_i2c_rx(bus, msgs->addr, msgs[i].buf, msgs[i].len);
		}
		else {
			res = skeleton_i2c_tx(bus, msgs->addr, msgs[i].buf, msgs[i].len);
		}
	}

	return res;
}

static int skeleton_i2c_init(const struct i2c_bus *bus) {

	return 0;
}

const struct i2c_ops skeleton_i2c_ops = {
    .i2c_master_xfer = skeleton_i2c_master_xfer,
    .i2c_init = skeleton_i2c_init,
};

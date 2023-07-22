#include <kernel/irq.h>
#include <framework/mod/options.h>
#include <embox/unit.h>

#include "I2C_driver/I2C_driver.h"

#include <drivers/i2c/i2c.h>


#define I2C_FREQUENCY OPTION_GET(NUMBER, i2c_frequency)

static int k1921vk035_i2c_master_xfer(struct i2c_adapter *adapter, struct i2c_msg *msgs, int num) {
	I2C_driver_operation_t ops[num];

	for (int i = 0; i < num; ++i) {
		if (msgs[i].flags & I2C_M_RD) {
			ops[i].address = (msgs[i].addr << 1) | I2C_DRIVER_READ_FLAG;
		} else {
			ops[i].address = (msgs[i].addr << 1) | I2C_DRIVER_WRITE_FLAG;
		}

		ops[i].data = msgs[i].buf;
		ops[i].size = (uint8_t)msgs[i].len;
		ops[i].start = 0;
	}

	I2C_driver_execute(ops, num);

	I2C_driver_state_t s;
	while (!(s = I2C_driver_is_done())) {}

	if (s == I2C_DRIVER_OK) {
		return msgs[num - 1].len;
	}
	return -1;
}

const struct i2c_algorithm k1921vk035_i2c_algo = {
	.i2c_master_xfer = k1921vk035_i2c_master_xfer,
};

static struct i2c_adapter k1921vk035_i2c_adap = {
	.i2c_algo_data = NULL,
	.i2c_algo = &k1921vk035_i2c_algo,
};

EMBOX_UNIT_INIT(k1921vk035_i2c_init);

static int k1921vk035_i2c_init() {
	I2C_driver_init(I2C_FREQUENCY);
	return i2c_bus_register(&k1921vk035_i2c_adap, 0, "i2c");
}


irq_return_t i2c_irq_handler(unsigned int irq_nr, void *data) {
	I2C_IRQHandler();
	return IRQ_HANDLED;
}

#define I2C_IRQn 36
STATIC_IRQ_ATTACH(I2C_IRQn, i2c_irq_handler, NULL);

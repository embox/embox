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



struct niiet_i2c_regs {                                                            
	volatile uint32_t SDA;                                               /*!< Data register */
	volatile uint32_t ST;                                                /*!< Status register */
	volatile uint32_t CST;                                               /*!< Status and control register */
 	volatile uint32_t CTL0;                                              /*!< Control register 0 */                                                   
    volatile uint32_t ADDR;                                              /*!< Register own address */
    volatile uint32_t CTL1;                                              /*!< Control register 1 */
	volatile uint32_t TOPR;                                              /*!< Prescaler load register */
	volatile uint32_t CTL2;                                              /*!< Control register 2 */
	volatile uint32_t CTL3;                                              /*!< Control register 3 */
	volatile uint32_t CTL4;                                              /*!< Control Register 4 */
};

static int vg015_i2c_tx(const struct i2c_bus *bus, uint16_t addr,
    uint8_t *buff, size_t sz) {

	return 0;
}

static int vg015_i2c_rx(const struct i2c_bus *bus, uint16_t addr,
    uint8_t *buff, size_t sz) {

	return 0;
}

static int vg015_i2c_master_xfer(const struct i2c_bus *bus, struct i2c_msg *msgs,
    size_t num_msgs) {
	int res;
	int i;

	for (i = 0; i < num_msgs; i++) {
		if (msgs[i].flags & I2C_M_RD) {
			res = vg015_i2c_rx(bus, msgs->addr, msgs[i].buf, msgs[i].len);
		}
		else {
			res = vg015_i2c_tx(bus, msgs->addr, msgs[i].buf, msgs[i].len);
		}
	}

	return res;
}

extern int vg015_i2c_hw_init0(const struct i2c_bus *bus);

static int vg015_i2c_init(const struct i2c_bus *bus) {

	vg015_i2c_hw_init0(bus);
	
	return 0;
}

const struct i2c_ops vg015_i2c_ops = {
    .i2c_master_xfer = vg015_i2c_master_xfer,
    .i2c_init = vg015_i2c_init,
};

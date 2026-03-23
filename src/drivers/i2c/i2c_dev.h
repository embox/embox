/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#ifndef DRIVERS_I2C_DEV_H_
#define DRIVERS_I2C_DEV_H_

#include <sys/cdefs.h>

struct i2c_dev {
	const char *i2cd_type_name;

	int i2cd_bus;
	int i2cd_addr;
};

__BEGIN_DECLS

extern int i2c_dev_probe(const struct i2c_dev *dev);

__END_DECLS

#endif /* DRIVERS_I2C_DEV_H_ */

/**
 * @file
 *
 * @date 01.12.25
 * @author Anton Bondarev
 */

#ifndef __I2C_H__
#define __I2C_H__

#include <drivers/i2c/i2c.h>

#define __user 

/* This is the structure as used in the I2C_RDWR ioctl call */
struct i2c_rdwr_ioctl_data {
	struct i2c_msg __user *msgs;	/* pointers to i2c_msgs */
	uint32_t nmsgs;			/* number of i2c_msgs */
};


#endif /* __I2C_H__ */
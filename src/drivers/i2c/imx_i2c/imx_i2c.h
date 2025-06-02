/**
 * @file
 *
 * @date Nov 17, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_I2C_IMX6_I2C_IMX6_H_
#define SRC_DRIVERS_I2C_IMX6_I2C_IMX6_H_

#include <stdint.h>

#define IMX_I2C_IADR 0x00 /* I2C Address Register */
#define IMX_I2C_IFDR 0x04 /* I2C Frequency Divider Register */
#define IMX_I2C_I2CR 0x08 /* I2C Control Register */
#define IMX_I2C_I2SR 0x0C /* I2C Status Register */
#define IMX_I2C_I2DR 0x10 /* I2C Data I/O Register */

#define IMX_I2C_I2CR_IEN  0x80 /* 1 << 7 */
#define IMX_I2C_I2CR_IIEN 0x40 /* 1 << 6 */
#define IMX_I2C_I2CR_MSTA 0x20 /* 1 << 5 */
#define IMX_I2C_I2CR_MTX  0x10 /* 1 << 4 */
#define IMX_I2C_I2CR_TXAK 0x08 /* 1 << 3 */
#define IMX_I2C_I2CR_RSTA 0x04 /* 1 << 2 */

#define IMX_I2C_I2SR_ICF  0x80 /* 1 << 7 */
#define IMX_I2C_I2SR_IAAS 0x40 /* 1 << 6 */
#define IMX_I2C_I2SR_IBB  0x20 /* 1 << 5 */
#define IMX_I2C_I2SR_IAL  0x10 /* 1 << 4 */
#define IMX_I2C_I2SR_SRW  0x04 /* 1 << 2 */
#define IMX_I2C_I2SR_IIF  0x02 /* 1 << 1 */
#define IMX_I2C_I2SR_RXAK 0x01 /* 1 << 0 */

struct imx_i2c_priv {
	void (*init_pins)(void);
	uintptr_t base_addr;
	int irq_num;
};

extern const struct i2c_ops imx_i2c_ops;

#endif /* SRC_DRIVERS_I2C_IMX6_I2C_IMX6_H_ */

/**
 * @file
 *
 * @date 02.04.2026
 * @author Dmitry Pilyuk
 */

#ifndef MPU6000_TRANSFER_H_
#define MPU6000_TRANSFER_H_

struct mpu6000_dev;

extern int mpu6000_hw_init(struct mpu6000_dev *dev);
extern int mpu6000_readb(struct mpu6000_dev *dev, int offset, uint8_t *ret);
extern int mpu6000_writeb(struct mpu6000_dev *dev, int offset, uint8_t val);

#endif /* MPU6000_TRANSFER_H_ */

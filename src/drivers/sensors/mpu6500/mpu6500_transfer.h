#ifndef MPU6500_TRANSFER_H_
#define MPU6500_TRANSFER_H_

#include <stdint.h>

extern int mpu6500_hw_init(void);
extern int mpu6500_readb(uint8_t reg, uint8_t *value);

#endif /* MPU6500_TRANSFER_H_ */

/**
 * @file
 *
 * @date 07.11.2026
 * @author Anton Bondarev
 */

#ifndef LSM6DS3_TRANSFER_H_
#define LSM6DS3_TRANSFER_H_

#include <stdint.h>

struct lsm6ds3_dev;

extern int lsm6ds3_hw_init(struct lsm6ds3_dev *dev);
extern int lsm6ds3_readb(struct lsm6ds3_dev *dev, int offset, uint8_t *ret);
extern int lsm6ds3_writeb(struct lsm6ds3_dev *dev, int offset, uint8_t val);

#endif /* LSM6DS3_TRANSFER_H_ */

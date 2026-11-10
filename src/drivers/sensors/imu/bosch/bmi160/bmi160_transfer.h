/**
 * @file
 *
 * @date 07.11.2026
 * @author Anton Bondarev
 */

#ifndef BMI160_TRANSFER_H_
#define BMI160_TRANSFER_H_

#include <stdint.h>

struct bmi160_dev;

extern int bmi160_hw_init(struct bmi160_dev *dev);
extern int bmi160_readb(struct bmi160_dev *dev, int offset, uint8_t *ret);
extern int bmi160_writeb(struct bmi160_dev *dev, int offset, uint8_t val);

#endif /* BMI160_TRANSFER_H_ */

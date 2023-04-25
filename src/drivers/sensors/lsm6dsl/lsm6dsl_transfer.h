/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#ifndef LSM6DSL_TRANSFER_H_
#define LSM6DSL_TRANSFER_H_

struct lsm6dsl_dev;

extern int lsm6dsl_hw_init(struct lsm6dsl_dev *dev);
extern int lsm6dsl_readb(struct lsm6dsl_dev *dev, int offset, uint8_t *ret);
extern int lsm6dsl_writeb(struct lsm6dsl_dev *dev, int offset, uint8_t val);

#endif /* LSM6DSL_TRANSFER_H_ */

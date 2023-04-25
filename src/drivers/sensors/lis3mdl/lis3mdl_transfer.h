/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#ifndef LIS3MDL_TRANSFER_H_
#define LIS3MDL_TRANSFER_H_

struct lis3mdl_dev;

extern int lis3mdl_hw_init(struct lis3mdl_dev *dev);
extern int lis3mdl_readb(struct lis3mdl_dev *dev, int offset, uint8_t *ret);
extern int lis3mdl_writeb(struct lis3mdl_dev *dev, int offset, uint8_t val);

#endif /* LIS3MDL_TRANSFER_H_ */

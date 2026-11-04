/**
 * @file
 * @brief
 *
 * @date    09.04.2026
 * @author  Efim Perevalov
 */
#ifndef BMP280_TRANSFER_H_
#define BMP280_TRANSFER_H_

#include <stddef.h>
#include <stdint.h>

struct bmp280_dev;

extern int bmp280_hw_init(struct bmp280_dev *dev);
extern int bmp280_read(struct bmp280_dev *dev, int offset, uint8_t *ret,
    size_t len);
extern int bmp280_readb(struct bmp280_dev *dev, int offset, uint8_t *ret);
extern int bmp280_writeb(struct bmp280_dev *dev, int offset, uint8_t val);

#endif /* BMP280_TRANSFER_H_ */

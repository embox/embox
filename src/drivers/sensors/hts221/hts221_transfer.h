/**
 * @file
 *
 * @date 06.02.2020
 * @author Alexander Kalmuk
 */

#ifndef HTS221_TRANSFER_H_
#define HTS221_TRANSFER_H_

struct hts221_dev;

extern int hts221_hw_init(struct hts221_dev *dev);
extern int hts221_readb(struct hts221_dev *dev, int offset, uint8_t *ret);
extern int hts221_writeb(struct hts221_dev *dev, int offset, uint8_t val);

#endif /* HTS221_TRANSFER_H_ */

/**
 * @file
 * @brief
 *
 * @date 21.04.2023
 * @author Andrew Bursian
 */

#ifndef LPS22HB_TRANSFER_H_
#define LPS22HB_TRANSFER_H_

struct lps22hb_dev;

extern int lps22hb_hw_init(struct lps22hb_dev *dev);
extern int lps22hb_readb(struct lps22hb_dev *dev, int offset, uint8_t *ret);
extern int lps22hb_writeb(struct lps22hb_dev *dev, int offset, uint8_t val);

#endif /* LPS22HB_TRANSFER_H_ */

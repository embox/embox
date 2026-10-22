#ifndef SPL06_TRANSFER_H_
#define SPL06_TRANSFER_H_

struct spl06_dev;

extern int spl06_hw_init(struct spl06_dev *dev);
extern int spl06_readb(struct spl06_dev *dev, int offset, uint8_t *ret);
extern int spl06_writeb(struct spl06_dev *dev, int offset, uint8_t val);

#endif /* SPL06_TRANSFER_H_ */

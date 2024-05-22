/**
 * @file
 *
 * @date 06.02.2020
 * @author Alexander Kalmuk
 */

#ifndef L3G4200D_TRANSFER_H_
#define L3G4200D_TRANSFER_H_

struct l3g4200d_dev;

extern int l3g4200d_hw_init(struct l3g4200d_dev *dev);
extern int l3g4200d_readb(struct l3g4200d_dev *dev, int offset, uint8_t *ret);
extern int l3g4200d_writeb(struct l3g4200d_dev *dev, int offset, uint8_t val);

#endif /* L3G4200D_TRANSFER_H_ */

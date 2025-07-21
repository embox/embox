/**
 * @file
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Daria Pechenova
 * @version
 * @date 18.07.2025
 */

#ifndef ISM43362_H_
#define ISM43362_H_

extern int ism43362_init();
extern int ism43362_exchange(char *txb, int txl, char *rxb, int rxl);

#endif /* ISM43362_H_ */
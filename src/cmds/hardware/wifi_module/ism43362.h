/**
 * @file ism43362.c
 * @brief ISM43362-M3G-L44-SPI Inventek eS-WiFi module library
 * @author Andrew Bursian
 * @version
 * @date 15.06.2023
 */

extern int ism43362_init();
extern int ism43362_exchange(char *txb, int txl, char *rxb, int rxl);

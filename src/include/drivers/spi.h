/**
 * @file
 *
 * @date 21.02.13
 * @author Pavel Cherstvov
 */

#ifndef DRIVERS_SPI_H_
#define DRIVERS_SPI_H_

/* Set master mode  */
int spi_poll_switch_master_mode(void);

/* Send data in Polling Mode */
int spi_poll_send(const char *outdata, const __u8 *indata, __u32 nBytes);
/* Send data in Interrupt Mode */
/* int spi_irq_send(void); */

#endif /* DRIVERS_SPI_H_ */

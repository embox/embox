/**
 * @file spi.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#ifndef DRIVERS_SPI_H_
#define DRIVERS_SPI_H_

#include <stdbool.h>

#include <drivers/char_dev.h>
#include <util/array.h>
#include <util/macro.h>

struct spi_ops;

/* Host modes */
enum spi_mode_t {
	/* It could be just SPI_MODE_MASTER, but
	 * it conflicts with STM32 macros */
	SPI_MODE_T_MASTER,
	SPI_MODE_T_SLAVE,
};

enum spi_pol_phase_t {
	SPI_MODE_0 = 0x00,	/* CPOL = 0, CPHA = 0 */
	SPI_MODE_1 = 0x01,  /* CPOL = 0, CPHA = 1 */
	SPI_MODE_2 = 0x02,  /* CPOL = 1, CPHA = 0 */
	SPI_MODE_3 = 0x03,  /* CPOL = 1, CPHA = 1 */
};

struct spi_device;

typedef void (*irq_event_t)(struct spi_device *data);

struct spi_device {
	int    flags;
	struct dev_module *dev;
	struct spi_ops *spi_ops;
	bool is_master;
	void  *priv;
	irq_event_t send_complete;
	irq_event_t received_data;
	uint8_t *in;
	uint8_t *out;
	int count;
};

struct spi_ops {
	int (*init)(struct spi_device *dev);
	int (*select)(struct spi_device *dev, int cs);
	int (*set_mode)(struct spi_device *dev, bool is_master);
	int (*transfer)(struct spi_device *dev, uint8_t *in, uint8_t *out, int cnt);
};

extern struct spi_device *spi_dev_by_id(int id);
extern int spi_dev_id(struct spi_device *dev);

/* In polling mode: 
 * - either of *in or *out buffers can be set, cnt is zero to positive
 *
 * In interrupt mode:  
 * - call with cnt < 0, set to -N bytes to write out. Interrupts will fire
 *   until N bytes are written out to make cnt == 0
 * - within interrupt handlers, function can be called as expected in polling mode
 *   where cnt is positive.
 * 
 *   NB For BCM283X implementation: 
 *   - count must not be larger than 16 bytes for sending within interrupt call 
 *   - takes 12 bytes received to trigger interrupt
 */
extern int spi_transfer(struct spi_device *dev, uint8_t *in, uint8_t *out, int cnt);

/* Set extra options in struct *dev before calling this function 
 */
extern int spi_select(struct spi_device *dev, int cs);
extern int spi_set_master_mode(struct spi_device *dev);
extern int spi_set_slave_mode(struct spi_device *dev);

extern const struct idesc_ops spi_iops;

#define SPI_REGISTRY_SZ 4

/* Note: if you get linker error like "redefinition of 'spi_device0'"
 * then you should reconfig system so SPI bus indecies do not overlap */
#define SPI_DEV_DEF(dev_name, spi_dev_ops, dev_priv, idx) \
	struct spi_device MACRO_CONCAT(spi_device, idx) = { \
		.spi_ops = spi_dev_ops, \
		.priv    = dev_priv, \
	}; \
	CHAR_DEV_DEF(dev_name, NULL, NULL, &spi_iops, &MACRO_CONCAT(spi_device, idx)) \

/* IOCTL-related stuff */
#define SPI_IOCTL_CS       0x1
#define SPI_IOCTL_TRANSFER 0x2
#define SPI_IOCTL_CS_MODE  0x3

struct spi_transfer_arg {
	uint8_t *in, *out;
	ssize_t count;
};

/* CS modes */
#define SPI_CS_ACTIVE   (1 << 0)
#define SPI_CS_INACTIVE (1 << 1)
#define SPI_CS_MODE(x)	( ( (x) & 0x03) << 2)		/* x is one of enum spi_pol_phase_t */
#define SPI_CS_IRQD  	(1 << 4)					
#define SPI_CS_IRQR  	(1 << 5)					
#define SPI_CS_DIVSOR(x) ( ( (x) & 0xFFFF) << 16 ) 	/* Upper 16 bits used to set clock divisor */

#endif /* DRIVERS_SPI_H_ */

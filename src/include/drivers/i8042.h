/**
 * @file
 * @brief
 *
 * @date 14.02.13
 * @author Anton Bondarev
 */

#ifndef DRIVERS_I8042_H_
#define DRIVERS_I8042_H_

#include <asm/io.h>

#define  I8042_CMD_PORT        0x64
#define  I8042_STS_PORT        0x64
#define  I8042_DATA_PORT       0x60

#define  I8042_CMD_READ_MODE   0x20
#define  I8042_CMD_WRITE_MODE  0x60
#define  I8042_CMD_PORT_DIS    0xAD
#define  I8042_CMD_PORT_EN     0xAE

#define  I8042_MODE_XLATE      0x40
#define  I8042_MODE_DISABLE    0x10
#define  I8042_MODE_INTERRUPT  0x01


/* Status */
#define I8042_STS_PERR      0x80    /* Parity error */
#define I8042_STS_TMO       0x40    /* Timeout */
#define I8042_STS_AUXOBF    0x20    /* Mouse OBF */
#define I8042_STS_INH       0x10    /* 0: inhibit  1: no-inhibit */
#define I8042_STS_SYS       0x04    /* 0: power up  1:Init comp */
#define I8042_STS_IBF       0x02    /* Input (to kbd) buffer full */
#define I8042_STS_OBF       0x01    /* Output (from kbd) buffer full */

#define keyboard_read_stat() \
	inb(I8042_STS_PORT)

#define keyboard_wait_read(status) \
	do {} while (0 == ((status = inb(I8042_STS_PORT)) & I8042_STS_OBF))
#define keyboard_wait_write(status) \
	do {} while (0 != ((status = inb(I8042_STS_PORT)) & I8042_STS_IBF))
#define kmc_wait_ibe()	while (inb(I8042_STS_PORT) & I8042_STS_IBF)

static inline int keyboard_havechar(void) {
	unsigned char c = inb(I8042_STS_PORT);
	return (c == 0xFF) ? 0 : c & I8042_STS_OBF;
}

#endif /* DRIVERS_I8042_H_ */

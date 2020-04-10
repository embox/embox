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

/* I/O ports */
#define  I8042_CMD_PORT           0x64
#define  I8042_STS_PORT           0x64
#define  I8042_DATA_PORT          0x60

/* Controller Commands */
#define  I8042_CMD_READ_MODE      0x20
#define  I8042_CMD_WRITE_MODE     0x60
#define  I8042_CMD_MOUSE_DISABLE  0xA7
#define  I8042_CMD_MOUSE_ENABLE   0xA8
#define  I8042_CMD_PORT_DIS       0xAD
#define  I8042_CMD_PORT_EN        0xAE
#define  I8042_CMD_WRITE_MOUSE    0xD4

/* Controller Status */
#define I8042_STS_PERR            0x80
#define I8042_STS_TMO             0x40
#define I8042_STS_AUXOBF          0x20
#define I8042_STS_INH             0x10
#define I8042_STS_SYS             0x04
#define I8042_STS_IBF             0x02
#define I8042_STS_OBF             0x01

/* Controller Mode Register Bits */
#define I8042_MODE_INTERRUPT      0x01
#define I8042_MODE_MOUSE_INT      0x02
#define I8042_MODE_SYS            0x04
#define I8042_MODE_NO_KEYLOCK     0x08
#define I8042_MODE_DISABLE        0x10
#define I8042_MODE_DISABLE_MOUSE  0x20
#define I8042_MODE_XLATE          0x40
#define I8042_MODE_RFU            0x80

/* Mouse Commands */
#define I8042_AUX_SET_SCALE11     0xE6
#define I8042_AUX_SET_SCALE21     0xE7
#define I8042_AUX_SET_RES         0xE8
#define I8042_AUX_GET_SCALE       0xE9
#define I8042_AUX_SET_STREAM      0xEA
#define I8042_AUX_POLL            0xEB
#define I8042_AUX_RESET_WRAP      0xEC
#define I8042_AUX_SET_WRAP        0xEE
#define I8042_AUX_SET_REMOTE      0xF0
#define I8042_AUX_GET_TYPE        0xF2
#define I8042_AUX_SET_SAMPLE      0xF3
#define I8042_AUX_ENABLE_DEV      0xF4
#define I8042_AUX_DISABLE_DEV     0xF5
#define I8042_AUX_SET_DEFAULT     0xF6
#define I8042_AUX_RESET           0xFF
#define I8042_AUX_ACK             0xFA

/* Keyboard Commands */
#define I8042_KBD_SET_LEDS        0xED
#define I8042_KBD_ECHO            0xEE
#define I8042_KBD_SCANCODE        0xF0
#define I8042_KBD_GET_ID          0xF2
#define I8042_KBD_SET_RATE        0xF3
#define I8042_KBD_ENABLE          0xF4
#define I8042_KBD_RESET_DISABLE   0xF5
#define I8042_KBD_RESET_ENABLE    0xF6
#define I8042_KBD_RESET           0xFF

#define I8042_KBD_IRQ   1
#define I8042_MOUSE_IRQ 12

#define keyboard_read_stat() \
	inb(I8042_STS_PORT)

#define keyboard_wait_read(status) \
	do {} while (0 == ((status = inb(I8042_STS_PORT)) & I8042_STS_OBF))
#define keyboard_wait_write(status) \
	do {} while (0 != ((status = inb(I8042_STS_PORT)) & I8042_STS_IBF))

#define i8042_wait_write() while (inb(I8042_STS_PORT) & I8042_STS_IBF)
#define i8042_wait_read() while (~inb(I8042_STS_PORT) & I8042_STS_OBF)

extern int i8042_read_mode(void);
extern void i8042_write_mode(uint8_t val);
extern void i8042_write_aux(uint8_t val);

static inline int keyboard_havechar(void) {
	unsigned char c = inb(I8042_STS_PORT);
	return (c == 0xFF) ? 0 : c & I8042_STS_OBF;
}

#endif /* DRIVERS_I8042_H_ */

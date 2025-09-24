/*
 * @file
 *
 * @date Sep 19, 2025
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <drivers/gpio.h>

#include <config/board_config.h>

static void mdelay(int value) {
	volatile int delay = value * 10;

	while (delay --);

}

#define DEMO_FILL     1


#define KS0108_LD_CMD_ON				0x3F
#define KS0108_LD_CMD_OFF				0x3E
#define KS0108_LD_CMD_SET_ADDR          0x40
#define KS0108_LD_CMD_SET_PAGE          0xB8
#define KS0108_LD_CMD_START             0xC0


#define PORT_DB0          (CONF_LCD_PIN_DB0_PORT)
#define PIN_DB0           (1 << CONF_LCD_PIN_DB0_NR)
#define PORT_DB1          (CONF_LCD_PIN_DB1_PORT)
#define PIN_DB1           (1 << CONF_LCD_PIN_DB1_NR)
#define PORT_DB2          (CONF_LCD_PIN_DB2_PORT)
#define PIN_DB2           (1 << CONF_LCD_PIN_DB2_NR)
#define PORT_DB3          (CONF_LCD_PIN_DB3_PORT)
#define PIN_DB3           (1 << CONF_LCD_PIN_DB3_NR)
#define PORT_DB4          (CONF_LCD_PIN_DB4_PORT)
#define PIN_DB4           (1 << CONF_LCD_PIN_DB4_NR)
#define PORT_DB5          (CONF_LCD_PIN_DB5_PORT)
#define PIN_DB5           (1 << CONF_LCD_PIN_DB5_NR)
#define PORT_DB6          (CONF_LCD_PIN_DB6_PORT)
#define PIN_DB6           (1 << CONF_LCD_PIN_DB6_NR)
#define PORT_DB7          (CONF_LCD_PIN_DB7_PORT)
#define PIN_DB7           (1 << CONF_LCD_PIN_DB7_NR)


#define PORT_CS1          (CONF_LCD_PIN_CS1_PORT)
#define PIN_CS1           (1 << CONF_LCD_PIN_CS1_NR)
#define PORT_CS2          (CONF_LCD_PIN_CS2_PORT)
#define PIN_CS2           (1 << CONF_LCD_PIN_CS2_NR)

#define PORT_EN          (CONF_LCD_PIN_EN_PORT)
#define PIN_EN           (1 << CONF_LCD_PIN_EN_NR)
#define PORT_RW          (CONF_LCD_PIN_RW_PORT)
#define PIN_RW           (1 << CONF_LCD_PIN_RW_NR)

#define PORT_RS          (CONF_LCD_PIN_RS_PORT)
#define PIN_RS           (1 << CONF_LCD_PIN_RS_NR)
#define PORT_RST         (CONF_LCD_PIN_RST_PORT)
#define PIN_RST          (1 << CONF_LCD_PIN_RST_NR)


void ks0108_lcd_select_chip(int mask_chip) {
	if (mask_chip & 1) {
		gpio_set(PORT_CS1, PIN_CS1, 1);
	} else {
		gpio_set(PORT_CS1, PIN_CS1, 0);
	}
	if (mask_chip & 2) {
		gpio_set(PORT_CS2, PIN_CS2, 1);
	} else {
		gpio_set(PORT_CS2, PIN_CS2, 0);		
	}
}

#define CHECK_BIT(byte, num)        ((byte & (1 << num)) ? 1 : 0)

void ks0108_lcd_send_byte(uint8_t byte) {


	gpio_set(PORT_DB0, PIN_DB0, CHECK_BIT(byte, 0));
	gpio_set(PORT_DB1, PIN_DB1, CHECK_BIT(byte, 1));
	gpio_set(PORT_DB2, PIN_DB2, CHECK_BIT(byte, 2));
	gpio_set(PORT_DB3, PIN_DB3, CHECK_BIT(byte, 3));
	gpio_set(PORT_DB4, PIN_DB4, CHECK_BIT(byte, 4));
	gpio_set(PORT_DB5, PIN_DB5, CHECK_BIT(byte, 5));
	gpio_set(PORT_DB6, PIN_DB6, CHECK_BIT(byte, 6));
	gpio_set(PORT_DB7, PIN_DB7, CHECK_BIT(byte, 7));

	mdelay(1);
	gpio_set(PORT_EN, PIN_EN, 1);

	mdelay(1);
	gpio_set(PORT_EN, PIN_EN, 0);
	mdelay(10);
}

void ks0108_lcd_send_command(uint8_t cmd, int chips) {
/* FIXME must be waiting loop */
	mdelay(10);

	ks0108_lcd_select_chip(chips);
	
	gpio_set(PORT_RS, PIN_RS, 0);    /* RS = 0 */
	gpio_set(PORT_RW, PIN_RW, 0);    /* RW = 0 */

	ks0108_lcd_send_byte(cmd);
}

void ks0108_lcd_send_data(uint8_t cmd, int chips) {
/* FIXME must be waiting loop */
	mdelay(10);

	ks0108_lcd_select_chip(chips);
	
	gpio_set(PORT_RS, PIN_RS, 1);    /* RS = 1 */
	gpio_set(PORT_RW, PIN_RW, 0);    /* RW = 0 */

	ks0108_lcd_send_byte(cmd);
}

void ks0108_lcd_goto(int x, int y) {
	int chip = 0;

	if (x < 64) {
		chip = 1;
	} else {
		x -= 64;
		chip = 2;
	}
	ks0108_lcd_send_command(KS0108_LD_CMD_SET_PAGE | (y >> 3), chip);
	ks0108_lcd_send_command(KS0108_LD_CMD_SET_ADDR | (x), chip);

}

void ks0108_lcd_init(void) {
	gpio_setup_mode(PORT_DB0, PIN_DB0, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_DB1, PIN_DB1, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_DB2, PIN_DB2, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_DB3, PIN_DB3, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_DB4, PIN_DB4, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_DB5, PIN_DB5, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_DB6, PIN_DB6, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_DB7, PIN_DB7, GPIO_MODE_OUT);

	gpio_setup_mode(PORT_CS1, PIN_CS1, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_CS2, PIN_CS2, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_EN, PIN_EN, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_RW, PIN_RW, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_RS, PIN_RS, GPIO_MODE_OUT);
	gpio_setup_mode(PORT_RST, PIN_RST, GPIO_MODE_OUT);

	gpio_set(PORT_RS, PIN_RS, 0);
	gpio_set(PORT_RW, PIN_RW, 0);
	gpio_set(PORT_EN, PIN_EN, 0);
	
	gpio_set(PORT_RST, PIN_RST, 0);
	mdelay(5);
	gpio_set(PORT_RST, PIN_RST, 1);
	mdelay(50);

	ks0108_lcd_send_command(KS0108_LD_CMD_ON, 3);
	ks0108_lcd_send_command(KS0108_LD_CMD_START, 3);

	ks0108_lcd_goto(0, 0);

	for (int p = 0; p < 8; p ++) {
		ks0108_lcd_goto(0, p * 8);
		for (int a = 0; a < 64; a ++) {
			ks0108_lcd_send_data(0, 1);
		}
		ks0108_lcd_goto(64, p * 8);
		for (int a = 0; a < 64; a ++) {
			ks0108_lcd_send_data(0, 2);
		}
	}

#if DEMO_FILL

	for (int p = 0;  p < 8; p ++) {
		ks0108_lcd_goto(0, p * 8);
		for (int a = 0; a < 64; a ++) {
			if (((p * 8) + a) < 64) {
				ks0108_lcd_send_data(0xFF, 1);
			} else {
				ks0108_lcd_send_data(0x00, 1);
			}
		}
	}
#endif /* DEMO_FILL */

}
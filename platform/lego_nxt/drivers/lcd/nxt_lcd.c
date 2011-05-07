/**
 * @file
 * @brief Text and graphics output to the LCD display.
 *
 * @date 09.10.10
 * @author Nikolay Korotky
 */

#include <embox/unit.h>
#include <linux/init.h>
#include <hal/reg.h>
#include <string.h>
#include <drivers/at91sam7s256.h>
#include <drivers/lcd.h>

EMBOX_UNIT_INIT(unit_lcd_init);

#define CS_PIN  (1<<10)
#define CD_PIN  (1<<12)

__u8 mode = 0xff;

__u8 display_buffer[NXT_LCD_DEPTH + 1][NXT_LCD_WIDTH];

static void spi_set_mode(__u8 m) {
	__u32 status;

	/* nothing to do if we are already in the correct mode */
	if (m == mode) {
		return;
	}

	/* Wait until all bytes have been sent */
	do {
		status = REG_LOAD(AT91C_SPI_SR);
	} while (!(status & 0x200));
	/* Set command or data mode */
	if (m) {
		REG_STORE(AT91C_PIOA_SODR, CD_PIN);
	} else {
		REG_STORE(AT91C_PIOA_CODR, CD_PIN);
	}
	/* remember the current mode */
	mode = m;
}

static void nxt_spi_write(__u32 CD, const __u8 *data, __u32 nBytes) {
	__u32 status;
	__u32 cd_mask = (CD ? 0x100 : 0);

	spi_set_mode(CD);
	while (nBytes) {
		REG_STORE(AT91C_SPI_TDR, (*data | cd_mask));
		data++;
		nBytes--;
		/* Wait until byte sent */
		do {
			status = REG_LOAD(AT91C_SPI_SR);
		} while (!(status & 0x200));
	}
}

static void nxt_lcd_command(__u8 cmd) {
	__u8 tmp = cmd;
	nxt_spi_write(0, &tmp, 1);
}

static void nxt_lcd_set_col(__u32 coladdr) {
	nxt_lcd_command(0x00 | (coladdr & 0xF));
	nxt_lcd_command(0x10 | ((coladdr >> 4) & 0xF));
}

// XXX defined but not used
#if 0
static void nxt_lcd_set_temp_comp(__u32 tc) {
	nxt_lcd_command(0x24 | (tc & 3));
}

static void nxt_lcd_set_panel_loading(__u32 hi) {
	nxt_lcd_command(0x28 | ((hi) ? 1 : 0));
}

static void nxt_lcd_set_pump_control(__u32 pc) {
	nxt_lcd_command(0x2c | (pc & 3));
}

static void nxt_lcd_set_scroll_line(__u32 sl) {
	nxt_lcd_command(0x40 | (sl & 0x3f));
}
#endif

static void nxt_lcd_set_page_address(__u32 pa) {
	nxt_lcd_command(0xB0 | (pa & 0xf));
}

// XXX defined but not used
#if 0
static void nxt_lcd_set_frame_rate(__u32 fr) {
	nxt_lcd_command(0xA0 | (fr & 1));
}

static void nxt_lcd_inverse_display(__u32 on) {
	nxt_lcd_command(0xA6 | ((on) ? 1 : 0));
}

static void nxt_lcd_set_cursor_update(__u32 on) {
	nxt_lcd_command(0xEE | ((on) ? 1 : 0));
}
#endif

static void nxt_lcd_reset(void) {
	nxt_lcd_command(0xE2);
}

static void nxt_lcd_set_multiplex_rate(__u32 mr) {
	nxt_lcd_command(0x20 | (mr & 3));
}

static void nxt_lcd_set_bias_ratio(__u32 ratio) {
	nxt_lcd_command(0xE8 | (ratio & 3));
}

static void nxt_lcd_set_pot(__u32 pot) {
	nxt_lcd_command(0x81);
	nxt_lcd_command(pot & 0xff);
}

static void nxt_lcd_set_ram_address_control(__u32 ac) {
	nxt_lcd_command(0x88 | (ac & 7));
}

static void nxt_lcd_set_map_control(__u32 map_control) {
	nxt_lcd_command(0xC0 | ((map_control & 3) << 1));
}

static void nxt_lcd_enable(__u32 on) {
	nxt_lcd_command(0xAE | ((on) ? 1 : 0));
}

/* just it's great for debuggin */
void nxt_lcd_set_all_pixels_on(__u32 on) {
	nxt_lcd_command(0xA4 | ((on) ? 1 : 0));
}

static void nxt_lcd_power_up(void) {
	//sleep(20);
	int i = 0;
	while (i<10000) {i++;}
	nxt_lcd_reset();
	//sleep(20);
	i = 0;
	while (i<10000) {i++;}
	nxt_lcd_set_multiplex_rate(3);
	nxt_lcd_set_bias_ratio(3);
	nxt_lcd_set_pot(0x60);

	nxt_lcd_set_ram_address_control(1);
	nxt_lcd_set_map_control(0x02);

	spi_set_mode(1);
	REG_STORE(AT91C_SPI_TNPR, (__u32)display_buffer);
	REG_STORE(AT91C_SPI_TNCR, 132);
	nxt_lcd_enable(1);
	//nxt_lcd_set_all_pixels_on(1);
}

void nxt_lcd_force_update(void) {
	int i;
	__u8 *disp = (__u8*) display_buffer;
	REG_STORE(AT91C_SPI_IER, AT91C_SPI_ENDTX);

	for (i = 0; i < NXT_LCD_DEPTH; i++) {
		nxt_lcd_set_col(0);
		nxt_lcd_set_page_address(i);
		nxt_spi_write(1, disp, NXT_LCD_WIDTH);
		disp += NXT_LCD_WIDTH;
	}
}

int __init lcd_init(void) {
	REG_STORE(AT91C_PMC_PCER, (1L << AT91C_ID_SPI)); /* Enable MCK clock */
	REG_STORE(AT91C_PIOA_PER, AT91C_PIO_PA12); /*EnableA0onPA12*/
	REG_STORE(AT91C_PIOA_OER, AT91C_PIO_PA12);
	REG_STORE(AT91C_PIOA_CODR, AT91C_PIO_PA12);
	REG_STORE(AT91C_PIOA_PDR, AT91C_PA14_SPCK); /*EnableSPCKonPA14*/

	REG_STORE(AT91C_PIOA_ASR, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_ODR, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_OWER, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_MDDR, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_PPUDR, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_IFDR, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_CODR, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_IDR, AT91C_PA14_SPCK);
	REG_STORE(AT91C_PIOA_PDR, AT91C_PA13_MOSI); /*EnablemosionPA13*/

	REG_STORE(AT91C_PIOA_ASR, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_ODR, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_OWER, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_MDDR, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_PPUDR, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_IFDR, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_CODR, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_IDR, AT91C_PA13_MOSI);
	REG_STORE(AT91C_PIOA_PDR, AT91C_PA10_NPCS2); /*Enablenpcs0onPA10*/

	REG_STORE(AT91C_PIOA_BSR, AT91C_PA10_NPCS2);
	REG_STORE(AT91C_PIOA_ODR, AT91C_PA10_NPCS2);
	REG_STORE(AT91C_PIOA_OWER, AT91C_PA10_NPCS2);
	REG_STORE(AT91C_PIOA_MDDR, AT91C_PA10_NPCS2);
	REG_STORE(AT91C_PIOA_PPUDR, AT91C_PA10_NPCS2);
	REG_STORE(AT91C_PIOA_IFDR, AT91C_PA10_NPCS2);
	REG_STORE(AT91C_PIOA_CODR, AT91C_PA10_NPCS2);
	REG_STORE(AT91C_PIOA_IDR, AT91C_PA10_NPCS2);

	REG_STORE(AT91C_SPI_CR, AT91C_SPI_SWRST);/*Softreset*/
	REG_STORE(AT91C_SPI_CR, AT91C_SPI_SPIEN);/*Enablespi*/
	REG_STORE(AT91C_SPI_MR, AT91C_SPI_MSTR|AT91C_SPI_MODFDIS | (0xB<<16));
	AT91C_SPI_CSR[2] = ((CLOCK_FREQUENCY/SPI_BITRATE)<<8) | AT91C_SPI_CPOL;

	nxt_lcd_power_up();
	memset(display_buffer, 0, NXT_LCD_WIDTH*NXT_LCD_DEPTH);
	return 0;
}

static int unit_lcd_init(void) {
	return lcd_init();
}

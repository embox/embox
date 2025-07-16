/**
 * @file
 * @brief Simple driver for Hitachi HD44780. There is some limitation:
 *	* Control pins must be on same gpio port
 *	* Data pins must be attached sequentially to same port
 *
 * @author  Anton Kozlov
 * @date    10.07.2012
 */

#include <string.h>
#include <drivers/gpio.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(lcd_test);

/* low-level defines */

/* below defines control pins */
#define CTRL_PORT OPTION_GET(NUMBER,control_port)
#define RS (1 << OPTION_GET(NUMBER, reg_select_pin))
#define RW (1 << OPTION_GET(NUMBER, rw_pin))
#define E (1 << OPTION_GET(NUMBER, clock_pin))

/* Below defines data port */
#define DATA_PORT OPTION_GET(NUMBER,data_port)
#define DATA_PINS_OFFSET OPTION_GET(NUMBER,data_pins_offset)
#define BUSY_FLAG (1 << (7 + DATA_PINS_OFFSET))

/* high-level defines */
#define LINE_N OPTION_GET(NUMBER,line_n)
#define WIDTH_N OPTION_GET(NUMBER,line_width)

static char buf[LINE_N * WIDTH_N];
static int pos = 0;
static int line_first = 0;

/* low-level routines */

static inline void nsdelay(int ns) {
	volatile int lns = ns / 10 + 1;
	while (lns-- > 0);
}

static int lcd_read(int reg) {
	int val;
	gpio_setup_mode(DATA_PORT, 0xff << DATA_PINS_OFFSET, GPIO_MODE_IN);
	gpio_set(CTRL_PORT, RS, reg);
	gpio_set(CTRL_PORT, RW, 1);

	gpio_set(CTRL_PORT, E, 1);
	nsdelay(140);
	val = gpio_get(DATA_PORT, 0xff << DATA_PINS_OFFSET);
	gpio_set(CTRL_PORT, E, 0);
	nsdelay(10);

	return val;
}

static void lcd_write(int reg, int val) {
	gpio_setup_mode(DATA_PORT, 0xff << DATA_PINS_OFFSET, GPIO_MODE_OUT);
	gpio_set(DATA_PORT, 0xff << DATA_PINS_OFFSET, 0);
	gpio_set(CTRL_PORT, RS, reg);
	gpio_set(CTRL_PORT, RW, 0);
	gpio_set(DATA_PORT, val << DATA_PINS_OFFSET, 1);

	gpio_set(CTRL_PORT, E, 1);
	nsdelay(140);
	gpio_set(CTRL_PORT, E, 0);
	nsdelay(10);
}

static void lcd_wait_busy(void) {
	while (lcd_read(0) & BUSY_FLAG);
}

static void lcd_write_wait(int reg, char ch) {
	lcd_write(reg, ch);
	lcd_wait_busy();
}

static inline void go_to(int line, int char_pos) {
	lcd_write_wait(0, 0x80 + line * 0x40 + char_pos);
}

static inline void clear(void) {
	lcd_write_wait(0, 0x01);
}

/* high-level routines */
static inline void print_line(int n) {
	for (int j = 0; j < WIDTH_N; j++) {
		lcd_write_wait(1, buf[n * WIDTH_N + j]);
	}
}

static void refresh(void) {
	int cnt = 0;

	for (int i = line_first+1; i < LINE_N; i++) {
		go_to(cnt++, 0);
		print_line(i);

	}
	for (int i = 0; i <= line_first; i++) {
		go_to(cnt++, 0);
		print_line(i);
	}
}

static void newline(void) {

	line_first = (line_first + 1) % LINE_N;
	memset(buf + WIDTH_N * line_first, ' ', WIDTH_N);
	refresh();
	pos = 0;
	go_to(LINE_N - 1, 0);
}

void lcd_putc(char ch) {

	if (ch == '\n') {
		newline();
		return;
	}

	if (pos >= WIDTH_N) {
		newline();
	}

	buf[line_first * WIDTH_N + pos] = ch;
	pos ++;

	lcd_write_wait(1, ch);
}


static int lcd_test(void) {
	char demo[] = "      Embox     \n   Rev unknown  ";

	gpio_set(CTRL_PORT, E | RS | RW, 0);
	gpio_setup_mode(CTRL_PORT, RS | E | RW, GPIO_MODE_OUT);

	lcd_write(0, 0x38);
	nsdelay(39000);
	lcd_write(0, 0x38);
	nsdelay(39000);
	lcd_write_wait(0, 0x0e);
	lcd_write_wait(0, 0x01);

	memset(buf, ' ', LINE_N * WIDTH_N);
	go_to(LINE_N - 1, 0);

#if 1 /* example */
	for (int i = 0; i < sizeof(demo) - 1; i++) {
		lcd_putc(demo[i]);
	}
#endif
	return 0;
}

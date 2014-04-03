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

#include <drivers/gpio.h>

EMBOX_UNIT_INIT(lcd_test);

/* low-level defines */

/* below defines control pins */
#define CTRL_PORT OPTION_GET(STRING,control_port)
#define RS (1 << OPTION_GET(NUMBER, reg_select_pin))
#define RW (1 << OPTION_GET(NUMBER, rw_pin))
#define E (1 << OPTION_GET(NUMBER, clock_pin))

/* Below defines data port */
#define DATA_PORT OPTION_GET(STRING,data_port)
#define DATA_PINS_OFFSET OPTION_GET(NUMBER,data_pins_offset)
#define BUSY_FLAG (1 << (7 + DATA_PINS_OFFSET))

/* high-level defines */
#define LINE_N OPTION_GET(NUMBER,line_n)
#define WIDTH_N OPTION_GET(NUMBER,line_width)

static char buf[LINE_N * WIDTH_N];
static int pos = 0;
static int line_first = 0;

/* low-level routines */

static inline void ctrl(int pin, int state) {
	if (state) {
		gpio_set_level(CTRL_PORT, pin, 0x01);
		return;
	}

	gpio_set_level(CTRL_PORT, pin, 0);
}

static inline int clock(void) {
	int level = 0;
	ctrl(E, 1);
	gpio_settings(DATA_PORT, BUSY_FLAG, GPIO_MODE_INPUT);
	level = gpio_get_level(DATA_PORT, BUSY_FLAG);
	gpio_settings(DATA_PORT, BUSY_FLAG, GPIO_MODE_OUTPUT);
	ctrl(E, 0);
	return level;
}

static inline void set_data(int reg, char ch) {
	ctrl(RS, reg);
	ctrl(RW, 0);
	gpio_set_level(DATA_PORT, 0xff & ~ch, 0);
	gpio_set_level(DATA_PORT, ch, 0x01);

}

static inline void wait_busy(void) {
	ctrl(RS, 0);
	ctrl(RW, 1);
	while (clock());
}

static inline void set_n_clock(int reg, char ch) {
	wait_busy();
	set_data(reg, ch);
	clock();
}

static inline void go_to(int line, int char_pos) {
	set_n_clock(0, 0x80 + line * 0x40 + char_pos);
}

static inline void clear(void) {
	set_n_clock(0, 0x01);
}

/* high-level routines */
static inline void print_line(int n) {
	for (int j = 0; j < WIDTH_N; j++) {
		set_n_clock(1, buf[n * WIDTH_N + j]);
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


static void lcd_putc(char ch) {

	if (ch == '\n') {
		newline();
		return;
	}

	if (pos >= WIDTH_N) {
		newline();
	}

	buf[line_first * WIDTH_N + pos] = ch;
	pos ++;

	set_n_clock(1, ch);
}


static int lcd_test(void) {
	char init[] = {0x38, 0x0e, 0x06, 0x01};
	char demo[] = "      Embox     \n   Rev unknown  ";

	ctrl(E | RS | RW, 0);
	gpio_settings(CTRL_PORT, RS | E | RW, GPIO_MODE_OUTPUT);
	gpio_settings(DATA_PORT, 0xff << DATA_PINS_OFFSET, GPIO_MODE_OUTPUT);

	wait_busy();

	for (int i = 0; i < sizeof(init); i++) {
		set_n_clock(0, init[i]);
	}

	memset(buf, ' ', LINE_N * WIDTH_N);
	go_to(LINE_N - 1, 0);

#if 1 /* example */
	for (int i = 0; i < sizeof(demo) - 1; i++) {
		lcd_putc(demo[i]);
	}
#endif
	return 0;
}

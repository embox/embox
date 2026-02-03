/*
 * @file
 *
 * @date 31.01.2023
 * @author Andrew Bursian
 */

#include <util/log.h>

#include <errno.h>
#include <assert.h>

//#include <kernel/irq.h>
#include <kernel/time/sys_timer.h>
#include <drivers/input/input_dev.h>

#include <drivers/gpio.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(keypad_init);

#include <framework/mod/options.h>

#define KEYPAD_ROWS OPTION_GET(NUMBER,rows)
#define KEYPAD_COLS OPTION_GET(NUMBER,cols)

#define KP_POLL_PERIOD   OPTION_GET(NUMBER, kp_poll_period)
#define KP_HOLD_TICKS    OPTION_GET(NUMBER, kp_hold_ticks)

#define COL0_PORT   OPTION_GET(NUMBER, kp_col0_port)
#define COL0_PIN    OPTION_GET(NUMBER, kp_col0_pino)
#define COL1_PORT   OPTION_GET(NUMBER, kp_col1_port)
#define COL1_PIN    OPTION_GET(NUMBER, kp_col1_pino)
#define COL2_PORT   OPTION_GET(NUMBER, kp_col2_port)
#define COL2_PIN    OPTION_GET(NUMBER, kp_col2_pino)
#define COL3_PORT   OPTION_GET(NUMBER, kp_col3_port)
#define COL3_PIN    OPTION_GET(NUMBER, kp_col3_pino)
#define ROW0_PORT   OPTION_GET(NUMBER, kp_row0_port)
#define ROW0_PIN    OPTION_GET(NUMBER, kp_row0_pino)
#define ROW1_PORT   OPTION_GET(NUMBER, kp_row1_port)
#define ROW1_PIN    OPTION_GET(NUMBER, kp_row1_pino)
#define ROW2_PORT   OPTION_GET(NUMBER, kp_row2_port)
#define ROW2_PIN    OPTION_GET(NUMBER, kp_row2_pino)
#define ROW3_PORT   OPTION_GET(NUMBER, kp_row3_port)
#define ROW3_PIN    OPTION_GET(NUMBER, kp_row3_pino)

struct bpid {				// board pin id
	unsigned port;			// port (| chip_no<<4)
	unsigned pino;			// pin no 0..15
};
static const struct bpid cols[4]={
	{COL0_PORT, COL0_PIN},
	{COL1_PORT, COL1_PIN},
	{COL2_PORT, COL2_PIN},
	{COL3_PORT, COL3_PIN}
};
static const struct bpid rows[4]={
	{ROW0_PORT, ROW0_PIN},
	{ROW1_PORT, ROW1_PIN},
	{ROW2_PORT, ROW2_PIN},
	{ROW3_PORT, ROW3_PIN}
};
//static const struct bpid rows[4]={{GPIO_PORT_C, 12}, {GPIO_PORT_D, 2}, {GPIO_PORT_G, 2}, {GPIO_PORT_G, 3}};
//static const struct bpid cols[4]={{GPIO_PORT_C, 8}, {GPIO_PORT_C, 9}, {GPIO_PORT_C, 10}, {GPIO_PORT_C, 11}};
static const int keys[]={
	2, 3, 4, (3*8+6)|KBD_SHIFT_PRESSED,
	5, 6, 7, (6*8+0)|KBD_SHIFT_PRESSED,
	8, 9, 10, (5*8+6)|KBD_SHIFT_PRESSED,
	9|KBD_SHIFT_PRESSED, 11, 4|KBD_SHIFT_PRESSED, (4*8+0)|KBD_SHIFT_PRESSED	
};
//static const char keys[]="123A456B789C*0#D";

struct keypad_s {
	struct input_dev indev;
	int  down;			// internal flag-counter, means key still down
	int  row, col;			// internal row & column of key, last pressed
	unsigned long timer_counter;	// timer_counter*KEYBOARD_POLL_PERIOD is the time elapsed from open or flush
	int POLL_PERIOD;		// in ms
	int HOLD_TICKS;			// number of poll periods to ensure that key is up
	sys_timer_t * timer;		// timer pointer for sys_timer_close
};

static struct keypad_s keypad; // static initialisation see below 

static void keypad_timer_handler(sys_timer_t* timer, void *param) {
	struct keypad_s *kp=param;
	struct input_dev *dev=&((struct keypad_s *)param)->indev;
	struct input_event ev;

	kp->timer_counter++;
	
	if (!(kp->down)) {
		for (int c=0; c<4; c++) {
			gpio_set(cols[c].port, 1<<cols[c].pino, GPIO_PIN_LOW);
			for (int r=0; r<4; r++) {
				if (!gpio_get(rows[r].port, 1<<rows[r].pino)) {
					ev.type=USER_BUTTON_PRESSED | KBD_KEY_PRESSED;
					ev.value=keys[(r<<2)+c];
					log_debug("keypad key pressed");
					input_dev_report_event(dev, &ev);
					kp->down=kp->HOLD_TICKS;
					kp->row=r; kp->col=c;
				}
			}
			gpio_set(cols[c].port, 1<<cols[c].pino, GPIO_PIN_HIGH);
		}
	} else {	// no matter about down spikes, holding poll until key up for HOLD_TICKS
		gpio_set(cols[kp->col].port, 1<<cols[kp->col].pino, GPIO_PIN_LOW);
		if (!gpio_get(rows[kp->row].port, 1<<rows[kp->row].pino)) kp->down=kp->HOLD_TICKS;
		else kp->down--;
		if (!(kp->down)) {
			ev.type=USER_BUTTON_UNPRESSED;
			ev.value=keys[(kp->row<<2)+kp->col];
			log_debug("keypad key released");
			input_dev_report_event(dev, &ev);
		} 
		gpio_set(cols[kp->col].port, 1<<cols[kp->col].pino, GPIO_PIN_HIGH);
	}
}

void keypad_flush(struct keypad_s *kp) {
	kp->down=0;
	kp->row=0;
	kp->col=0;
	kp->timer_counter=0;
}

static int keypad_start(struct input_dev *dev) {
	// Flags & variables flush
	struct keypad_s *kp=(struct keypad_s *)&keypad;				// ????
	keypad_flush(kp);

	// Set timer handler
	if (sys_timer_set(&(kp->timer), SYS_TIMER_PERIODIC, kp->POLL_PERIOD, keypad_timer_handler, kp)) {
		log_error("Failed to install timer");
		return -1;
	}
	return 0;
}

static int keypad_stop(struct input_dev *dev) {
	struct keypad_s *kp=(struct keypad_s *)&keypad;				// ????
	// Reset timer handler
	sys_timer_close(kp->timer);
	return 0;
}

static const struct input_dev_ops keypad_input_ops = {
	.start=keypad_start,
	.stop=keypad_stop
};

static struct keypad_s keypad = {
	.indev = {
		.ops = &keypad_input_ops,
		.name = "keypad",
		//.type = INPUT_DEV_BUTTON
		.type = INPUT_DEV_KBD
	}
};

static int keypad_init(void) {
	int ret = 0;

	// Check matrix size
	if (KEYPAD_ROWS !=4 || KEYPAD_COLS != 4) {
		log_error("only 4x4 matrix can be driven");
		goto err_out;
	}

	// Driver registration
	ret = input_dev_register(&keypad.indev);
	if (ret != 0) {
		log_error("input_dev_register failed");
		goto err_out;
	}
	keypad.indev.data = (void *) &keypad;

	// Internal data structure preparation
	struct keypad_s *kp=(struct keypad_s *)&keypad;				// ????
	kp->POLL_PERIOD=KP_POLL_PERIOD;
	kp->HOLD_TICKS=KP_HOLD_TICKS;

	// I/O pins intitialization
	for (int i=0; i<4; i++) {
		// Output pins
		gpio_setup_mode(cols[i].port, 1<<cols[i].pino, GPIO_MODE_OUT);
		gpio_set(cols[i].port, 1<<cols[i].pino, GPIO_PIN_HIGH);
		// Input pins
		gpio_setup_mode(rows[i].port, 1<<rows[i].pino, GPIO_MODE_IN|GPIO_MODE_IN_PULL_UP);
	}

err_out:
	return ret;
}


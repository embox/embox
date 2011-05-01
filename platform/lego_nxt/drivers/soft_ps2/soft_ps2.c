/**
 * @file
 * @brief Soft PS/2 keyboard/mouse electrical protocol implementation
 *
 * @date 12.12.10
 * @author Anton Kozlov
 */
#include <types.h>
#include <drivers/pins.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <drivers/at91_timer_counter.h>
#include <hal/reg.h>
#include <drivers/at91sam7s256.h>

#define READ_CNT 11
#define WRITE_CNT 12

#define DELAY 50000
#if 1
#define DATA (1 << 23)
#define CLK (1 << 18)
#else
#define CLK (1 << 23)
#define DATA (1 << 18)
#endif

volatile enum {
	IDLE,
	READ,
	WRITE,
	NOT_INITED,
	BYPASS
} ps2_state;

uint16_t read_val = 0x00;
static uint16_t read_mask = 1;
static uint16_t write_val;
int cnt;

EMBOX_UNIT_INIT(soft_ps2_init);

static void pin_set_high(pin_mask_t mask) {
	pin_config_input(mask);
}

static void pin_set_low(pin_mask_t mask) {
	pin_clear_output(mask);
	pin_config_output(mask);
}

void reset_ps2_state(void) {
	ps2_state = IDLE;
}

volatile static int inited = 0;

static int parity(uint8_t val) {
	int ret = 0;
	while (val) {
		ret += val & 1;
		val >>= 1;
	}
	return ret & 1;
}

void hard_delay(int val) {
	volatile int tmp = val;
	while (tmp --) {
	}
}

int ps2_write(uint8_t data) {
	while (ps2_state != IDLE) {
	}
	TRACE("write goes\n");
	ps2_state = BYPASS;
	cnt = WRITE_CNT;
	write_val = (data & 0x7f) | (parity(data) << 1);
	pin_set_low(CLK);
	hard_delay(DELAY);
	pin_set_low(DATA);
	hard_delay(DELAY);
	pin_set_high(CLK);
	ps2_state = WRITE;
	return 0;

}

static int handler_counter = 0;
static int clk_snap = 0;

#define PS2_BUFF_SIZE 128
uint16_t ps2_buff[PS2_BUFF_SIZE];
int ps2_buff_pos = 0;

static inline void process(uint16_t data) {
	ps2_buff[ps2_buff_pos++] = 0x7ff & data;
}

int get_clk_snap(void) {
	return clk_snap;
}

int get_handler_counter(void) {
	return handler_counter;
}

static inline pin_mask_t fast_get_input(void) {
	return (int) REG_LOAD(AT91C_PIOA_PDSR);
}

static void ps2_handler(pin_mask_t state, pin_mask_t mask) {
	int data_state = fast_get_input();
	int clk_state = data_state & CLK;
	data_state &= DATA;
	handler_counter++;
	switch (ps2_state) {
	case IDLE:
	case READ:
		clk_snap <<= 1;
		if (clk_state) {
			clk_snap |= 1;
		}
		if (!clk_state) {
			if (data_state) {
				read_val |= read_mask;
			}
			read_mask <<= 1;
			if (--cnt == 0) {
				cnt = READ_CNT;
				read_mask = 1;
				process(read_val);
				read_val = 0;
			}
		}
		break;
	case WRITE:
		if (state & CLK) {
			if (write_val & 1) {
				pin_set_high(DATA);
			} else {
				pin_set_low(DATA);
			}
			write_val >>= 1;
			cnt --;
		} else {
			if (cnt == 0) {
				pin_set_high(DATA);
				cnt = READ_CNT;
				ps2_state = IDLE;
			}
		}
		break;
	case NOT_INITED:
		if (clk_state && data_state) {
			ps2_state = IDLE;
		}
		break;
	case BYPASS:
	default:
		break;
	}
}

static int soft_ps2_init(void) {
	REG_STORE(AT91C_PIOA_PER, CLK | DATA);
	REG_STORE(AT91C_PIOA_PPUDR, CLK);
	REG_STORE(AT91C_PIOA_PPUDR, DATA);
	REG_STORE(AT91C_PIOA_ODR, CLK | DATA);
	REG_STORE(AT91C_PIOA_MDER, CLK | DATA);

	REG_ORIN(AT91C_AIC_SMR + AT91C_ID_PIOA, 7);

	ps2_state = NOT_INITED;
	cnt = READ_CNT;
	pin_set_high(CLK | DATA);
	hard_delay(3000);
	if (pin_get_input(CLK) && pin_get_input(DATA)) {
		ps2_state = IDLE;
	}
	pin_set_input_monitor(CLK, &ps2_handler);
	return 0;
}

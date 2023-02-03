/**
 * @file
 *
 * @date Apr 28, 2021
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <kernel/irq.h>

#include <drivers/input/input_dev.h>
#include <drivers/ps_keyboard.h>
#include <drivers/common/memory.h>

#include <drivers/pl050.h>

#include <embox/unit.h>

#define BASE_ADDR		OPTION_GET(NUMBER,base_addr)
#define IRQ_NUM			OPTION_GET(NUMBER,irq_num)
#define TRANSLATE_TO_SET1	OPTION_GET(BOOLEAN,translate_to_set1)

EMBOX_UNIT_INIT(pl050_keyboard_init);


struct pl050_keyboard_indev {
	struct input_dev input_dev;
	struct pl050 *pl050_dev;
};

static int pl050_keyboard_stop(struct input_dev *dev) {

	/* TODO */
	return 0;
}

static const struct input_dev_ops kbd_input_ops = {
		/*.start = keyboard_start,*/
		.stop = pl050_keyboard_stop,
};

static struct pl050_keyboard_indev keyboard_dev = {
	.input_dev = {
		.ops = &kbd_input_ops,
		.name = "ps-keyboard",
		.type = INPUT_DEV_KBD,
	},
};

#if (TRANSLATE_TO_SET1 == 1)
static const uint32_t set2_to_set1_table[]={
/*	0	1	2	3	4	5	6	7	8	9	A	B	C	D	E	F
0*/	0,	0x43,	0,	0x3F,	0x3D,	0x3B,	0x3C,	0x58,	0x64,	0x44,	0x42,	0x40,	0x3E,	0x0F,	0x29,	0x59,	/*
1*/	0x65,	0x38,	0x2A,	0x70,	0x1D,	0x10,	0x02,	0,	0x66,	0x71,	0x2C,	0x1F,	0x1E,	0x11,	0x03,	0x5B,	/*
2*/	0x67,	0x2E,	0x2D,	0x20,	0x12,	0x05,	0x04,	0x5C,	0x68,	0x39,	0x2F,	0x21,	0x14,	0x13,	0x06,	0x5D,	/*
3*/	0x69,	0x31,	0x30,	0x23,	0x22,	0x15,	0x07,	0x5E,	0x6A,	0x72,	0x32,	0x24,	0x16,	0x08,	0x09,	0x5F,	/*
4*/	0x6B,	0x33,	0x25,	0x17,	0x18,	0x0B,	0x0A,	0x60,	0x6C,	0x34,	0x35,	0x26,	0x27,	0x19,	0x0C,	0x61,	/*
5*/	0x6D,	0x73,	0x28,	0x74,	0x1A,	0x0D,	0x62,	0x6E,	0x3A,	0x36,	0x1C,	0x1B,	0x75,	0x2B,	0x63,	0x76,	/*
6*/	0,	0x56,	0x77,	0x78,	0x79,	0x7A,	0x0E,	0x7B,	0x7C,	0x4F,	0x7D,	0x4B,	0x47,	0x7E,	0x7F,	0x6F,	/*
7*/	0x52,	0x53,	0x50,	0x4C,	0x4D,	0x48,	0x01,	0x45,	0x57,	0x4E,	0x51,	0x4A,	0x37,	0x49,	0x46,	0,	/*
8*/	0,	0,	0,	0x41,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	/*
9*/	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	/*
A*/	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	/*
B*/	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	/*
C*/	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	/*
D*/	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	/*
E*/	0xE0,	0xE1
};
static int break_flag=0; // indicates that next key is up, depressed

static uint32_t translate_set2_to_set1(uint32_t c) {
	if (c==0xF0) {
		break_flag=1;
		return 0xF0;
	}
	if (c>sizeof(set2_to_set1_table)/sizeof(uint32_t)) return 0;
	c=set2_to_set1_table[c];
	if (break_flag) {
		break_flag=0;
		c|=0x80;
	}
	return c;
}
#endif	// TRANSLATE_TO_SET1

static irq_return_t pl050_keyboard_irq_hnd(unsigned int irq_nr, void *data) {
	struct pl050_keyboard_indev *pl050_keyboard_indev;
	struct pl050 *pl050;
	uint32_t rx_data;
	struct input_event event;
	struct input_dev *indev;

	pl050_keyboard_indev = (struct pl050_keyboard_indev *)data;
	pl050 = pl050_keyboard_indev->pl050_dev;
	indev = &pl050_keyboard_indev->input_dev;

	rx_data = pl050->data;

	if (rx_data == KEYBOARD_SCAN_CODE_EXT) {
		goto out;
	}

#if (TRANSLATE_TO_SET1 == 1)
	rx_data=translate_set2_to_set1(rx_data);

	if (rx_data == 0xF0) {
		goto out;
	}
#endif	// TRANSLATE_TO_SET1

	keyboard_scan_code_to_event(rx_data, &event);
	input_dev_report_event(indev, &event);

out:
	return IRQ_HANDLED;
}

static int pl050_keyboard_init(void) {
	int res;

	res = irq_attach(IRQ_NUM, pl050_keyboard_irq_hnd, 0,
					 &keyboard_dev, "ps keyboard");
	if (res < 0) {
		return res;
	}
	keyboard_dev.pl050_dev = (void*)(uintptr_t)BASE_ADDR;

	pl050_init(keyboard_dev.pl050_dev);

	return input_dev_register(&keyboard_dev.input_dev);
}

PERIPH_MEMORY_DEFINE(pl050_keyboard, BASE_ADDR, 0x1000);


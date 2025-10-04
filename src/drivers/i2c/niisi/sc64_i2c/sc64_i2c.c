/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 29.05.25
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <drivers/i2c/i2c.h>
#include <hal/reg.h>

#include "sc64_i2c.h"

struct sc64_i2c_regs {
	uint8_t addr;
	uint8_t data;
	uint8_t ctrl;
	union {
		uint8_t stat; /* Read only */
		uint8_t ccr;  /* Write only */
	};
	uint8_t xaddr;
	uint8_t reserved[2];
	uint8_t reset;
} __attribute__((packed));

#define REG_CNTR_AAK  (1 << 2)
#define REG_CNTR_IFLG (1 << 3)
#define REG_CNTR_STP  (1 << 4)
#define REG_CNTR_STA  (1 << 5)
#define REG_CNTR_ENAB (1 << 6)
#define REG_CNTR_IEN  (1 << 7)

#define REG_STAT_M_START          0x08
#define REG_STAT_M_REPEAT_START   0x10
#define REG_STAT_M_WR_ADDR_ACK    0x18
#define REG_STAT_M_WR_ADDR_NO_ACK 0x20
#define REG_STAT_M_WR_DATA_ACK    0x28
#define REG_STAT_M_WR_DATA_NO_ACK 0x30
#define REG_STAT_M_RD_ADDR_ACK    0x40
#define REG_STAT_M_RD_ADDR_NO_ACK 0x48
#define REG_STAT_M_RD_DATA_ACK    0x50
#define REG_STAT_M_RD_DATA_NO_ACK 0x58

static int sc64_i2c_wait_interrupt(struct sc64_i2c_regs *regs) {
	int i;

	usleep(1);

	for (i = 0; i < 10000; i++) {
		if (REG8_LOAD(&regs->ctrl) & REG_CNTR_IFLG) {
			return 0;
		}
	}

	return -1;
}

static int sc64_i2c_send_msg(struct sc64_i2c_regs *regs, struct i2c_msg *msg) {
	size_t pos;
	int i;

	pos = 0;

	for (i = 0; i < msg->len + 10; i++) {
		if (0 != sc64_i2c_wait_interrupt(regs)) {
			return -ENXIO;
		}

		if (pos == msg->len) {
			return 0;
		}

		switch (REG8_LOAD(&regs->stat)) {
		case REG_STAT_M_START:
		case REG_STAT_M_REPEAT_START:
			REG8_STORE(&regs->data, (msg->addr & 0x7f) << 1);
			REG8_STORE(&regs->ctrl,
			    REG_CNTR_IEN | REG_CNTR_ENAB | REG_CNTR_AAK);
			break;

		case REG_STAT_M_WR_ADDR_ACK:
		case REG_STAT_M_WR_DATA_ACK:
			REG8_STORE(&regs->data, msg->buf[pos++]);
			REG8_STORE(&regs->ctrl,
			    REG_CNTR_IEN | REG_CNTR_ENAB | REG_CNTR_AAK);
			break;

		case REG_STAT_M_WR_ADDR_NO_ACK:
		case REG_STAT_M_WR_DATA_NO_ACK:
		default:
			break;
		}
	}

	return -EIO;
}

static int sc64_i2c_recv_msg(struct sc64_i2c_regs *regs, struct i2c_msg *msg) {
	size_t pos;
	int i;

	pos = 0;

	for (i = 0; i < msg->len + 10; i++) {
		if (0 != sc64_i2c_wait_interrupt(regs)) {
			return -ENXIO;
		}

		switch (REG8_LOAD(&regs->stat)) {
		case REG_STAT_M_START:
		case REG_STAT_M_REPEAT_START:
			REG8_STORE(&regs->data, ((msg->addr & 0x7f) << 1) | 0x1);
			REG8_STORE(&regs->ctrl,
			    REG_CNTR_IEN | REG_CNTR_ENAB | REG_CNTR_AAK);
			break;

		case REG_STAT_M_RD_DATA_ACK:
			msg->buf[pos++] = REG8_LOAD(&regs->data);
			if (pos == msg->len) {
				return 0;
			}
		case REG_STAT_M_RD_ADDR_ACK:
			REG8_STORE(&regs->ctrl,
			    REG_CNTR_IEN | REG_CNTR_ENAB | REG_CNTR_AAK);
			break;

		case REG_STAT_M_RD_ADDR_NO_ACK:
		case REG_STAT_M_RD_DATA_NO_ACK:
		default:
			break;
		}
	}

	return -EIO;
}

static int sc64_i2c_master_xfer(const struct i2c_bus *bus, struct i2c_msg *msgs,
    size_t num_msgs) {
	struct sc64_i2c_regs *regs;
	int send_stop;
	int err;
	int i;

	regs = (struct sc64_i2c_regs *)bus->i2c_priv;
	send_stop = false;
	err = 0;

	for (i = 0; i < num_msgs; i++) {
		if (msgs[i].flags & I2C_M_TEN) {
			err = -ENOSUPP;
			break;
		}

		if (msgs[i].len == 0) {
			continue;
		}

		send_stop = true;

		REG8_STORE(&regs->ctrl,
		    REG_CNTR_IEN | REG_CNTR_ENAB | REG_CNTR_STA | REG_CNTR_AAK);

		if (msgs[i].flags & I2C_M_RD) {
			err = sc64_i2c_recv_msg(regs, &msgs[i]);
		}
		else {
			err = sc64_i2c_send_msg(regs, &msgs[i]);
		}
		if (err) {
			break;
		}
	}

	if (send_stop) {
		REG8_STORE(&regs->ctrl, REG_CNTR_ENAB | REG_CNTR_STP);
	}

	return err;
}

static int sc64_i2c_init(const struct i2c_bus *bus) {
	struct sc64_i2c_regs *regs;

	regs = (struct sc64_i2c_regs *)bus->i2c_priv;
	if (!regs) {
		return -EINVAL;
	}

	REG8_STORE(&regs->reset, 0);
	REG8_STORE(&regs->ctrl, REG_CNTR_ENAB | REG_CNTR_STP);

	return 0;
}

const struct i2c_ops sc64_i2c_ops = {
    .i2c_master_xfer = sc64_i2c_master_xfer,
    .i2c_init = sc64_i2c_init,
};

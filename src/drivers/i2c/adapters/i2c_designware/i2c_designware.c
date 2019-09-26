/**
 * @file
 *
 * @date Sep 25, 2019
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <hal/reg.h>
#include <swab.h>
#include <kernel/irq.h>

#include <drivers/i2c/i2c.h>

#include "i2c_designware.h"

static void delay(volatile int d) {
	d = d * 100;
	while (d-- >= 0);
}

static uint32_t i2c_dw_read(struct i2c_dw_dev *dev, int offset) {
	uint32_t value;

	if (dev->accessor_flags & ACCESS_16BIT) {
		value = REG32_LOAD(dev->base_addr + offset) |
			(REG32_LOAD(dev->base_addr + offset + 2) << 16);
	} else {
		value = REG32_LOAD(dev->base_addr + offset);
	}

	if (dev->accessor_flags & ACCESS_SWAP) {
		return swab32(value);
	} else {
		return value;
	}
}

static void i2c_dw_write(struct i2c_dw_dev *dev, uint32_t b, int offset) {
	if (dev->accessor_flags & ACCESS_SWAP) {
		b = swab32(b);
	}

	if (dev->accessor_flags & ACCESS_16BIT) {
		REG16_STORE(dev->base_addr + offset, ((uint16_t)b));
		REG16_STORE(dev->base_addr + offset + 2, (uint16_t)(b >> 16));
	} else {
		REG32_STORE(dev->base_addr + offset, b);
	}
}

static uint32_t inline
i2c_dw_scl_hcnt(uint32_t clk, uint32_t ts, uint32_t tf, int cond, int offset) {
	/*
	 * DesignWare I2C core doesn't seem to have solid strategy to meet
	 * the tHD;STA timing spec.  Configuring _HCNT based on tHIGH spec
	 * will result in violation of the tHD;STA spec.
	 */
	if (cond) {
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + (1+4+3) >= IC_CLK * tHIGH
		 *
		 * This is based on the DW manuals, and represents an ideal
		 * configuration.  The resulting I2C bus speed will be
		 * faster than any of the others.
		 *
		 * If your hardware is free from tHD;STA issue, try this one.
		 */
		return (clk * ts + 500000) / 1000000 - 8 + offset;
	} else {
		/*
		 * Conditional expression:
		 *
		 *   IC_[FS]S_SCL_HCNT + 3 >= IC_CLK * (tHD;STA + tf)
		 *
		 * This is just experimental rule; the tHD;STA period turned
		 * out to be proportional to (_HCNT + 3).  With this setting,
		 * we could meet both tHIGH and tHD;STA timing specs.
		 *
		 * If unsure, you'd better to take this alternative.
		 *
		 * The reason why we need to take into account "tf" here,
		 * is the same as described in i2c_dw_scl_lcnt().
		 */
		return (clk * (ts + tf) + 500000) / 1000000 - 3 + offset;
	}
}

static uint32_t inline
i2c_dw_scl_lcnt(uint32_t ic_clk, uint32_t tl, uint32_t tf, int offset) {
	/*
	 * Conditional expression:
	 *
	 *   IC_[FS]S_SCL_LCNT + 1 >= IC_CLK * (tLOW + tf)
	 *
	 * DW I2C core starts counting the SCL CNTs for the LOW period
	 * of the SCL clock (tLOW) as soon as it pulls the SCL line.
	 * In order to meet the tLOW timing spec, we need to take into
	 * account the fall time of SCL signal (tf).  Default tf value
	 * should be 0.3 us, for safety.
	 */
	return ((ic_clk * (tl + tf) + 500000) / 1000000) - 1 + offset;
}

static void i2c_dw_enable(struct i2c_dw_dev *dev, bool enable) {
	i2c_dw_write(dev, enable, I2C_DW_IC_ENABLE);
}

static void i2c_dw_enable_and_wait(struct i2c_dw_dev *dev, bool enable) {
	int timeout = 100;

	do {
		i2c_dw_enable(dev, enable);
		if ((i2c_dw_read(dev, I2C_DW_IC_ENABLE_STATUS) & 1) == enable) {
			return;
		}

		/*
		 * Wait 10 times the signaling period of the highest I2C
		 * transfer supported by the driver (for 400KHz this is
		 * 25us) as described in the DesignWare I2C databook.
		 */
		delay(100);
	} while (timeout--);

	log_debug("timeout in %sabling adapter", enable ? "en" : "dis");
}

/*
 * Waiting for bus not busy
 */
static int i2c_dw_wait_bus_not_busy(struct i2c_dw_dev *dev) {
	int timeout = 1000;

	while (i2c_dw_read(dev, I2C_DW_IC_STATUS) & I2C_DW_IC_STATUS_ACTIVITY) {
		if (timeout <= 0) {
			log_error("timeout waiting for bus ready");
			return -ETIMEDOUT;
		}
		timeout--;
		delay(1000);
	}

	return 0;
}

static void i2c_dw_disable_int(struct i2c_dw_dev *dev) {
	i2c_dw_write(dev, 0, I2C_DW_IC_INTR_MASK);
}

static void i2c_dw_xfer_init(struct i2c_dw_dev *dev) {
	struct i2c_msg *msgs = dev->msgs;
	uint32_t ic_con, ic_tar = 0;

	/* Disable the adapter */
	i2c_dw_enable_and_wait(dev, false);

	/* if the slave address is ten bit address, enable 10BITADDR */
	ic_con = i2c_dw_read(dev, I2C_DW_IC_CON);
	if (msgs[dev->msg_write_idx].flags & I2C_M_TEN) {
		ic_con |= I2C_DW_IC_CON_10BITADDR_MASTER;
		/*
		 * If I2C_DYNAMIC_TAR_UPDATE is set, the 10-bit addressing
		 * mode has to be enabled via bit 12 of IC_TAR register.
		 * We set it always as I2C_DYNAMIC_TAR_UPDATE can't be
		 * detected from registers.
		 */
		ic_tar = I2C_DW_IC_TAR_10BITADDR_MASTER;
	} else {
		ic_con &= ~I2C_DW_IC_CON_10BITADDR_MASTER;
	}
	i2c_dw_write(dev, ic_con, I2C_DW_IC_CON);

	/*
	 * Set the slave (target) address and enable 10-bit addressing mode
	 * if applicable.
	 */
	i2c_dw_write(dev, msgs[dev->msg_write_idx].addr | ic_tar, I2C_DW_IC_TAR);

	/* enforce disabled interrupts (due to HW issues) */
	i2c_dw_disable_int(dev);

	/* Enable the adapter */
	i2c_dw_enable(dev, true);

	/* Clear and enable interrupts */
	i2c_dw_read(dev, I2C_DW_IC_CLR_INTR);
	i2c_dw_write(dev, I2C_DW_IC_INTR_DEFAULT_MASK, I2C_DW_IC_INTR_MASK);
}

/*
 * Initiate (and continue) low level master read/write transaction.
 * This function is only called from i2c_dw_isr, and pumping i2c_msg
 * messages into the tx buffer.  Even if the size of i2c_msg data is
 * longer than the size of the tx buffer, it handles everything.
 */
static void i2c_dw_xfer_msg(struct i2c_dw_dev *dev) {
	struct i2c_msg *msgs = dev->msgs;
	uint32_t intr_mask;
	int tx_limit, rx_limit;
	uint32_t addr = msgs[dev->msg_write_idx].addr;
	uint32_t buf_len = dev->tx_buf_len;
	uint8_t *buf = dev->tx_buf;
	bool need_restart = false;

	intr_mask = I2C_DW_IC_INTR_DEFAULT_MASK;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		/*
		 * if target address has changed, we need to
		 * reprogram the target address in the i2c
		 * adapter when we are done with this transfer
		 */
		if (msgs[dev->msg_write_idx].addr != addr) {
			log_error("invalid target address");
			dev->msg_err = -EINVAL;
			break;
		}

		if (msgs[dev->msg_write_idx].len == 0) {
			log_error("invalid message length %d", msgs[dev->msg_write_idx].len);
			dev->msg_err = -EINVAL;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg */
			buf = msgs[dev->msg_write_idx].buf;
			buf_len = msgs[dev->msg_write_idx].len;

			/* If both IC_EMPTYFIFO_HOLD_MASTER_EN and
			 * IC_RESTART_EN are set, we must manually
			 * set restart bit between messages.
			 */
			if ((dev->master_cfg & I2C_DW_IC_CON_RESTART_EN) &&
					(dev->msg_write_idx > 0)) {
				need_restart = true;
			}
		}

		tx_limit = dev->tx_fifo_depth - i2c_dw_read(dev, I2C_DW_IC_TXFLR);
		rx_limit = dev->rx_fifo_depth - i2c_dw_read(dev, I2C_DW_IC_RXFLR);

		while (buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
			uint32_t cmd = 0;

			/*
			 * If IC_EMPTYFIFO_HOLD_MASTER_EN is set we must
			 * manually set the stop bit. However, it cannot be
			 * detected from the registers so we set it always
			 * when writing/reading the last byte.
			 */
			if (dev->msg_write_idx == dev->msgs_num - 1 && buf_len == 1) {
				cmd |= 1 << 9;
			}

			if (need_restart) {
				cmd |= 1 << 10;
				need_restart = false;
			}

			if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {

				/* avoid rx buffer overrun */
				if (dev->rx_outstanding >= dev->rx_fifo_depth) {
					break;
				}

				i2c_dw_write(dev, cmd | 0x100, I2C_DW_IC_DATA_CMD);
				rx_limit--;
				dev->rx_outstanding++;

			} else {
				i2c_dw_write(dev, cmd | *buf++, I2C_DW_IC_DATA_CMD);
			}
			tx_limit--;
			buf_len--;
		}

		dev->tx_buf = buf;
		dev->tx_buf_len = buf_len;

		if (buf_len > 0) {
			/* more bytes to be written */
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		} else
			dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/*
	 * If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more.
	 */
	if (dev->msg_write_idx == dev->msgs_num) {
		intr_mask &= ~I2C_DW_IC_INTR_TX_EMPTY;
	}

	if (dev->msg_err) {
		intr_mask = 0;
	}

	i2c_dw_write(dev, intr_mask, I2C_DW_IC_INTR_MASK);
}

static void i2c_dw_read_msg(struct i2c_dw_dev *dev) {
	struct i2c_msg *msgs = dev->msgs;
	int rx_valid;

	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		uint32_t len;
		uint8_t *buf;

		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD)) {
			continue;
		}

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dev->msg_read_idx].len;
			buf = msgs[dev->msg_read_idx].buf;
		} else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		rx_valid = i2c_dw_read(dev, I2C_DW_IC_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			*buf++ = i2c_dw_read(dev, I2C_DW_IC_DATA_CMD);
			dev->rx_outstanding--;
		}

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		} else {
			dev->status &= ~STATUS_READ_IN_PROGRESS;
		}
	}
}

static int i2c_dw_xfer(struct i2c_adapter *ad, struct i2c_msg *msgs, int num) {
	struct i2c_dw_dev *dev;
	int ret;

	dev = ad->i2c_algo_data;

	dev->msgs = msgs;
	dev->msgs_num = num;
	dev->cmd_err = 0;
	dev->msg_write_idx = 0;
	dev->msg_read_idx = 0;
	dev->msg_err = 0;
	dev->status = STATUS_IDLE;
	dev->abort_source = 0;
	dev->rx_outstanding = 0;

	ret = i2c_dw_wait_bus_not_busy(dev);
	if (ret) {
		return -ETIMEDOUT;
	}

	/* start the transfers */
	i2c_dw_xfer_init(dev);

	/* wait for tx to complete */
	delay(1000);

	while (dev->status != STATUS_IDLE);
	/*
	 * We must disable the adapter before returning and signaling the end
	 * of the current transfer. Otherwise the hardware might continue
	 * generating interrupts which in turn causes a race condition with
	 * the following transfer.  Needs some more investigation if the
	 * additional interrupts are a hardware bug or this driver doesn't
	 * handle them correctly yet.
	 */
	i2c_dw_enable(dev, false);

	if (dev->msg_err) {
		return dev->msg_err;
	}

	/* no error */
	if (!dev->cmd_err && !dev->status) {
		return num;
	}

	/* We have an error */
	if (dev->cmd_err == I2C_DW_IC_ERR_TX_ABRT) {
		return -ENODEV;
	}

	return ret;
}

const struct i2c_algorithm i2c_dw_algo = {
	.i2c_master_xfer = i2c_dw_xfer,
};

static uint32_t i2c_dw_read_clear_intrbits(struct i2c_dw_dev *dev) {
	uint32_t stat;

	/*
	 * The IC_INTR_STAT register just indicates "enabled" interrupts.
	 * Ths unmasked raw version of interrupt status bits are available
	 * in the IC_RAW_INTR_STAT register.
	 *
	 * That is,
	 *   stat = i2c_dw_read(IC_INTR_STAT);
	 * equals to,
	 *   stat = i2c_dw_read(IC_RAW_INTR_STAT) & i2c_dw_read(IC_INTR_MASK);
	 *
	 * The raw version might be useful for debugging purposes.
	 */
	stat = i2c_dw_read(dev, I2C_DW_IC_INTR_STAT);

	/*
	 * Do not use the IC_CLR_INTR register to clear interrupts, or
	 * you'll miss some interrupts, triggered during the period from
	 * i2c_dw_read(IC_INTR_STAT) to i2c_dw_read(IC_CLR_INTR).
	 *
	 * Instead, use the separately-prepared IC_CLR_* registers.
	 */
	if (stat & I2C_DW_IC_INTR_RX_UNDER) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_RX_UNDER);
	}
	if (stat & I2C_DW_IC_INTR_RX_OVER) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_RX_OVER);
	}
	if (stat & I2C_DW_IC_INTR_TX_OVER) {
		i2c_dw_read(dev,I2C_DW_IC_CLR_TX_OVER);
	}
	if (stat & I2C_DW_IC_INTR_RD_REQ) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_RD_REQ);
	}
	if (stat & I2C_DW_IC_INTR_TX_ABRT) {
		/*
		 * The IC_TX_ABRT_SOURCE register is cleared whenever
		 * the IC_CLR_TX_ABRT is read.  Preserve it beforehand.
		 */
		dev->abort_source = i2c_dw_read(dev, I2C_DW_IC_TX_ABRT_SOURCE);
		i2c_dw_read(dev, I2C_DW_IC_CLR_TX_ABRT);
	}
	if (stat & I2C_DW_IC_INTR_RX_DONE) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_RX_DONE);
	}
	if (stat & I2C_DW_IC_INTR_ACTIVITY) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_ACTIVITY);
	}
	if (stat & I2C_DW_IC_INTR_STOP_DET) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_STOP_DET);
	}
	if (stat & I2C_DW_IC_INTR_START_DET) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_START_DET);
	}
	if (stat & I2C_DW_IC_INTR_GEN_CALL) {
		i2c_dw_read(dev, I2C_DW_IC_CLR_GEN_CALL);
	}

	return stat;
}

/*
 * Interrupt service routine. This gets called whenever an I2C interrupt
 * occurs.
 */
static irq_return_t i2c_dw_isr(unsigned int this_irq, void *dev_id) {
	struct i2c_dw_dev *dev = dev_id;
	uint32_t stat, enabled;

	enabled = i2c_dw_read(dev, I2C_DW_IC_ENABLE);
	stat = i2c_dw_read(dev, I2C_DW_IC_RAW_INTR_STAT);
	log_debug("enabled=%#x stat=%#x", enabled, stat);
	if (!enabled || !(stat & ~I2C_DW_IC_INTR_ACTIVITY)) {
		return IRQ_NONE;
	}

	stat = i2c_dw_read_clear_intrbits(dev);

	if (stat & I2C_DW_IC_INTR_TX_ABRT) {
		log_debug("I2C_DW_IC_INTR_TX_ABRT");
		dev->cmd_err |= I2C_DW_IC_ERR_TX_ABRT;
		dev->status = STATUS_IDLE;

		/*
		 * Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed.  Make sure to skip them.
		 */
		i2c_dw_write(dev, 0, I2C_DW_IC_INTR_MASK);
		goto tx_aborted;
	}

	if (stat & I2C_DW_IC_INTR_RX_FULL) {
		log_debug("I2C_DW_IC_INTR_RX_FULL");
		i2c_dw_read_msg(dev);
	}

	if (stat & I2C_DW_IC_INTR_TX_EMPTY) {
		log_debug("I2C_DW_IC_INTR_TX_EMPTY");
		i2c_dw_xfer_msg(dev);
	}

	/*
	 * No need to modify or disable the interrupt mask here.
	 * i2c_dw_xfer_msg() will take care of it according to
	 * the current transmit status.
	 */

tx_aborted:
	if ((stat & (I2C_DW_IC_INTR_TX_ABRT | I2C_DW_IC_INTR_STOP_DET)) || dev->msg_err ) {
		log_debug("tx_aborted");
	}
	else if (unlikely(dev->accessor_flags & ACCESS_INTR_MASK)) {
		/* workaround to trigger pending interrupt */
		stat = i2c_dw_read(dev, I2C_DW_IC_INTR_MASK);
		i2c_dw_disable_int(dev);
		i2c_dw_write(dev, stat, I2C_DW_IC_INTR_MASK);
	}

	return IRQ_HANDLED;
}

static int i2c_dw_hw_init(struct i2c_dw_dev *dev) {
	uint32_t hcnt, lcnt;
	uint32_t reg, comp_param1;
	uint32_t sda_falling_time, scl_falling_time;

	dev->master_cfg = I2C_DW_IC_CON_MASTER |
			I2C_DW_IC_CON_SLAVE_DISABLE | I2C_DW_IC_CON_RESTART_EN |
			I2C_DW_IC_CON_SPEED_FAST;
	dev->sda_falling_time = 5000;
	dev->scl_falling_time = 5000;
	dev->clk_freq = 100000000;

	reg = i2c_dw_read(dev, I2C_DW_IC_COMP_TYPE);
	if (reg == ___constant_swab32(I2C_DW_IC_COMP_TYPE_VALUE)) {
		/* Configure register endianess access */
		dev->accessor_flags |= ACCESS_SWAP;
	} else if (reg == (I2C_DW_IC_COMP_TYPE_VALUE & 0x0000ffff)) {
		/* Configure register access mode 16bit */
		dev->accessor_flags |= ACCESS_16BIT;
	} else if (reg != I2C_DW_IC_COMP_TYPE_VALUE) {
		log_error("Unknown Synopsys component type: ""0x%08x\n", reg);
		return -ENODEV;
	}

	comp_param1 = i2c_dw_read(dev, I2C_DW_IC_COMP_PARAM_1);

	/* Disable the adapter */
	i2c_dw_enable_and_wait(dev, false);

	/* set standard and fast speed deviders for high/low periods */
	sda_falling_time = dev->sda_falling_time ?: 300; /* ns */
	scl_falling_time = dev->scl_falling_time ?: 300; /* ns */

	log_debug("sda_falling_time %d, scl_falling_time %d", sda_falling_time, scl_falling_time);

	/* Set SCL timing parameters for standard-mode */
	hcnt = 0x190; /* default */
	lcnt = 0x1D6; /* default */
#if 0
	hcnt = i2c_dw_scl_hcnt(dev->clk_freq,
				4000, /* tHD;STA = tHIGH = 4.0 us */
				sda_falling_time,
				0,    /* 0: DW default, 1: Ideal */
				0);   /* No offset */
	lcnt = i2c_dw_scl_lcnt(dev->clk_freq,
				4700, /* tLOW = 4.7 us */
				scl_falling_time,
				0);   /* No offset */
#endif
	i2c_dw_write(dev, hcnt, I2C_DW_IC_SS_SCL_HCNT);
	i2c_dw_write(dev, lcnt, I2C_DW_IC_SS_SCL_LCNT);
	log_debug("Standard-mode HCNT:LCNT = %d:%d", hcnt, lcnt);

	/* Set SCL timing parameters for fast-mode or fast-mode plus */
	hcnt = 0x3c; /* default */
	lcnt = 0x82; /* default */
#if 0
	hcnt = i2c_dw_scl_hcnt(dev->clk_freq,
				600,   /* tHD;STA = tHIGH = 0.6 us */
				sda_falling_time,
				0,     /* 0: DW default, 1: Ideal */
				0);    /* No offset */
	lcnt = i2c_dw_scl_lcnt(dev->clk_freq,
				1300,  /* tLOW = 1.3 us */
				scl_falling_time,
				0);    /* No offset */
#endif
	i2c_dw_write(dev, hcnt, I2C_DW_IC_FS_SCL_HCNT);
	i2c_dw_write(dev, lcnt, I2C_DW_IC_FS_SCL_LCNT);
	log_debug("Fast-mode HCNT:LCNT = %d:%d", hcnt, lcnt);

	if ((dev->master_cfg & I2C_DW_IC_CON_SPEED_MASK) ==
		I2C_DW_IC_CON_SPEED_HIGH) {
		if ((comp_param1 & I2C_DW_IC_COMP_PARAM_1_SPEED_MODE_MASK)
			!= I2C_DW_IC_COMP_PARAM_1_SPEED_MODE_HIGH) {
			log_error("High Speed not supported!");
			dev->master_cfg &= ~I2C_DW_IC_CON_SPEED_MASK;
			dev->master_cfg |= I2C_DW_IC_CON_SPEED_FAST;
		} else if (dev->hs_hcnt && dev->hs_lcnt) {
			hcnt = dev->hs_hcnt;
			lcnt = dev->hs_lcnt;
			i2c_dw_write(dev, hcnt, I2C_DW_IC_HS_SCL_HCNT);
			i2c_dw_write(dev, lcnt, I2C_DW_IC_HS_SCL_LCNT);
			log_debug("HighSpeed-mode HCNT:LCNT = %d:%d", hcnt, lcnt);
		}
	}

	/* Configure SDA Hold Time if required */
	reg = i2c_dw_read(dev, I2C_DW_IC_COMP_VERSION);
	if (reg >= I2C_DW_IC_SDA_HOLD_MIN_VERS) {
		if (!dev->sda_hold_time) {
			/* Keep previous hold time setting if no one set it */
			dev->sda_hold_time = i2c_dw_read(dev, I2C_DW_IC_SDA_HOLD);
		}
		/*
		 * Workaround for avoiding TX arbitration lost in case I2C
		 * slave pulls SDA down "too quickly" after falling edge of
		 * SCL by enabling non-zero SDA RX hold. Specification says it
		 * extends incoming SDA low to high transition while SCL is
		 * high but it appears to help also above issue.
		 */
		if (!(dev->sda_hold_time & I2C_DW_IC_SDA_HOLD_RX_MASK)) {
			dev->sda_hold_time |= 1 << I2C_DW_IC_SDA_HOLD_RX_SHIFT;
		}
		i2c_dw_write(dev, dev->sda_hold_time, I2C_DW_IC_SDA_HOLD);
	} else {
		log_info("Hardware too old to adjust SDA hold time.");
	}

	dev->tx_fifo_depth = ((comp_param1 >> 16) & 0xff) + 1;
	dev->rx_fifo_depth = ((comp_param1 >> 8)  & 0xff) + 1;

	/* Configure Tx/Rx FIFO threshold levels */
	i2c_dw_write(dev, dev->tx_fifo_depth / 2, I2C_DW_IC_TX_TL);
	i2c_dw_write(dev, 0, I2C_DW_IC_RX_TL);

	/* configure the i2c master */
	i2c_dw_write(dev, dev->master_cfg , I2C_DW_IC_CON);

	return 0;
}

int i2c_dw_master_init(struct i2c_dw_dev *dev) {
	int res = 0;

	i2c_dw_hw_init(dev);

	res = irq_attach(dev->irq_num, i2c_dw_isr, 0, dev, "i2c_dw");

	return res;
}

/**
 * @file
 *
 * @date Sep 25, 2019
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <swab.h>
#include <unistd.h>

#include <drivers/i2c/i2c.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <util/log.h>

#include "i2c_designware.h"

static uint32_t i2c_dw_read(struct i2c_dw_dev *dev, int offset) {
	uint32_t value;

	if (dev->accessor_flags & ACCESS_16BIT) {
		value = REG32_LOAD(dev->base_addr + offset)
		        | (REG32_LOAD(dev->base_addr + offset + 2) << 16);
	}
	else {
		value = REG32_LOAD(dev->base_addr + offset);
	}

	if (dev->accessor_flags & ACCESS_SWAP) {
		return swab32(value);
	}
	else {
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
	}
	else {
		REG32_STORE(dev->base_addr + offset, b);
	}
}

static void i2c_dw_enable(struct i2c_dw_dev *dev, bool enable) {
	i2c_dw_write(dev, enable ? 1 : 0, I2C_DW_IC_ENABLE);
}

static void i2c_dw_enable_and_wait(struct i2c_dw_dev *dev, bool enable) {
	int timeout = 100;

	i2c_dw_enable(dev, enable);

	do {
		if ((i2c_dw_read(dev, I2C_DW_IC_ENABLE_STATUS) & 1) == enable) {
			return;
		}

		/* Wait 10 times the signaling period of the highest I2C
		 * transfer supported by the driver (for 400KHz this is
		 * 25us) as described in the DesignWare I2C databook.     */
		usleep(25);
	} while (timeout--);

	log_debug("timeout in %sabling adapter", enable ? "en" : "dis");
}

static int i2c_dw_wait_bus_not_busy(struct i2c_dw_dev *dev) {
	int timeout = 1000;

	while (i2c_dw_read(dev, I2C_DW_IC_STATUS) & I2C_DW_IC_STATUS_ACTIVITY) {
		if (timeout <= 0) {
			log_error("timeout waiting for bus ready");
			return -ETIMEDOUT;
		}
		timeout--;
		usleep(USEC_PER_MSEC);
	}

	return 0;
}

static void i2c_dw_disable_int(struct i2c_dw_dev *dev) {
	i2c_dw_write(dev, 0, I2C_DW_IC_INTR_MASK);
}

static void i2c_dw_xfer_init(struct i2c_dw_dev *dev) {
	struct i2c_msg *msg = &dev->msgs[dev->msg_write_idx];
	uint32_t ic_con, ic_tar = 0;

	/* Disable the adapter */
	i2c_dw_enable_and_wait(dev, false);

	/* if the slave address is ten bit address, enable 10BITADDR */
	ic_con = i2c_dw_read(dev, I2C_DW_IC_CON);
	if (msg->flags & I2C_M_TEN) {
		ic_con |= I2C_DW_IC_CON_10BITADDR_MASTER;
		/* If I2C_DYNAMIC_TAR_UPDATE is set, the 10-bit addressing
		 * mode has to be enabled via bit 12 of IC_TAR register.
		 * We set it always as I2C_DYNAMIC_TAR_UPDATE can't be
		 * detected from registers. */
		ic_tar = I2C_DW_IC_TAR_10BITADDR_MASTER;
	}
	else {
		ic_con &= ~I2C_DW_IC_CON_10BITADDR_MASTER;
	}
	i2c_dw_write(dev, ic_con, I2C_DW_IC_CON);

	/* Set the slave (target) address and enable 10-bit addressing mode
	 * if applicable. */
	i2c_dw_write(dev, msg->addr | ic_tar, I2C_DW_IC_TAR);

	/* enforce disabled interrupts (due to HW issues) */
	i2c_dw_disable_int(dev);

	i2c_dw_enable(dev, true);

	/* Clear and enable interrupts */
	i2c_dw_read(dev, I2C_DW_IC_CLR_INTR);
	i2c_dw_write(dev, I2C_DW_IC_INTR_DEFAULT_MASK, I2C_DW_IC_INTR_MASK);
}

/* Initiate (and continue) low level master read/write transaction.
 * This function is only called from i2c_dw_irq_handler, and pumping
 * i2c_msg messages into the tx buffer.  Even if the size of i2c_msg
 * data is longer than the size of the tx buffer, it handles everything. */
static void i2c_dw_xfer_msg(struct i2c_dw_dev *dev) {
	uint32_t addr = dev->msgs[dev->msg_write_idx].addr;
	uint8_t *buf = dev->tx_buf;
	int len = dev->tx_buf_len;
	uint32_t intr_mask = I2C_DW_IC_INTR_DEFAULT_MASK;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		struct i2c_msg *msg = &dev->msgs[dev->msg_write_idx];
		int tx_limit, rx_limit;
		bool need_restart = false;

		/* if target address has changed, we need to
		 * reprogram the target address in the i2c
		 * adapter when we are done with this transfer */
		if (msg->addr != addr) {
			log_error("invalid target address");
			dev->msg_err = -EINVAL;
			break;
		}

		if (msg->len == 0) {
			log_error("invalid message length %d", msg->len);
			dev->msg_err = -EINVAL;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			/* New i2c_msg */
			buf = msg->buf;
			len = msg->len;

			/* If both IC_EMPTYFIFO_HOLD_MASTER_EN and
			 * IC_RESTART_EN are set, we must manually
			 * set restart bit between messages. */
			if ((dev->master_cfg & I2C_DW_IC_CON_RESTART_EN)
			    && (dev->msg_write_idx > 0)) {
				need_restart = true;
			}
		}

		tx_limit = dev->tx_fifo_depth - i2c_dw_read(dev, I2C_DW_IC_TXFLR);
		rx_limit = dev->rx_fifo_depth - i2c_dw_read(dev, I2C_DW_IC_RXFLR);

		while (len > 0 && tx_limit > 0 && rx_limit > 0) {
			uint32_t cmd = 0;

			/* If IC_EMPTYFIFO_HOLD_MASTER_EN is set we must
			 * manually set the stop bit. However, it cannot be
			 * detected from the registers so we set it always
			 * when writing/reading the last byte. */
			if (dev->msg_write_idx == dev->msgs_num - 1 && len == 1) {
				cmd |= 1 << 9;
			}

			if (need_restart) {
				cmd |= 1 << 10;
				need_restart = false;
			}

			if (msg->flags & I2C_M_RD) {
				/* avoid rx buffer overrun */
				if (dev->rx_outstanding >= dev->rx_fifo_depth) {
					break;
				}

				i2c_dw_write(dev, cmd | 0x100, I2C_DW_IC_DATA_CMD);
				rx_limit--;
				dev->rx_outstanding++;
			}
			else {
				i2c_dw_write(dev, cmd | *buf++, I2C_DW_IC_DATA_CMD);
			}

			tx_limit--;
			len--;
		}

		if (len > 0) {
			/* More bytes to be written */
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			dev->tx_buf_len = len;
			dev->tx_buf = buf;
			break;
		}

		dev->status &= ~STATUS_WRITE_IN_PROGRESS;
	}

	/* If i2c_msg index search is completed, we don't need TX_EMPTY
	 * interrupt any more. */
	if (dev->msg_write_idx == dev->msgs_num) {
		intr_mask &= ~I2C_DW_IC_INTR_TX_EMPTY;
	}

	if (dev->msg_err) {
		intr_mask = 0;
	}

	i2c_dw_write(dev, intr_mask, I2C_DW_IC_INTR_MASK);
}

static void i2c_dw_read_msg(struct i2c_dw_dev *dev) {
	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		uint32_t len;
		uint8_t *buf;
		int rx_valid;
		struct i2c_msg *msg = &dev->msgs[dev->msg_read_idx];

		if (!(msg->flags & I2C_M_RD)) {
			continue;
		}

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msg->len;
			buf = msg->buf;
		}
		else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		rx_valid = i2c_dw_read(dev, I2C_DW_IC_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			*buf++ = i2c_dw_read(dev, I2C_DW_IC_DATA_CMD);
			dev->rx_outstanding--;
		}

		if (len > 0) {
			/* Not enough data to fill message, so save
			 * buf and len for the next interrupt */
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		}

		dev->status &= ~STATUS_READ_IN_PROGRESS;
	}
}

static int i2c_dw_xfer(const struct i2c_bus *bus, struct i2c_msg *msgs,
    size_t num_msgs) {
	struct i2c_dw_dev *dev;
	int ret;

	dev = bus->i2c_priv;

	dev->msgs = msgs;
	dev->msgs_num = num_msgs;
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

	i2c_dw_xfer_init(dev);

	usleep(USEC_PER_MSEC);
	while (dev->status != STATUS_IDLE)
		;

	/* We must disable the adapter before returning and signaling the end
	 * of the current transfer. Otherwise the hardware might continue
	 * generating interrupts which in turn causes a race condition with
	 * the following transfer.  Needs some more investigation if the
	 * additional interrupts are a hardware bug or this driver doesn't
	 * handle them correctly yet. */
	i2c_dw_enable(dev, false);

	if (dev->msg_err) {
		return dev->msg_err;
	}

	if (!dev->cmd_err && dev->status == STATUS_IDLE) {
		return num_msgs;
	}

	if (dev->cmd_err == I2C_DW_IC_ERR_TX_ABRT) {
		return -ENODEV;
	}

	return ret;
}

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
		i2c_dw_read(dev, I2C_DW_IC_CLR_TX_OVER);
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

static irq_return_t i2c_dw_irq_handler(unsigned int this_irq, void *dev_id) {
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

		/* Anytime TX_ABRT is set, the contents of the tx/rx
		 * buffers are flushed.  Make sure to skip them. */
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

	/* No need to modify or disable the interrupt mask here.
	 * i2c_dw_xfer_msg() will take care of it according to
	 * the current transmit status. */

tx_aborted:
	if ((stat & (I2C_DW_IC_INTR_TX_ABRT | I2C_DW_IC_INTR_STOP_DET))
	    || dev->msg_err) {
		log_debug("tx_aborted");
	}
	else {
		/* Workaround to trigger pending interrupt */
		stat = i2c_dw_read(dev, I2C_DW_IC_INTR_MASK);
		i2c_dw_disable_int(dev);
		i2c_dw_write(dev, stat, I2C_DW_IC_INTR_MASK);
	}

	return IRQ_HANDLED;
}

static int i2c_dw_hw_init(struct i2c_dw_dev *dev) {
	uint32_t reg;

	reg = i2c_dw_read(dev, I2C_DW_IC_COMP_TYPE);

	/* This register should be equal to I2C_DW_IC_COMP_TYPE_VALUE,
	 * so we can check endianness and 16/32-bit access */
	if (reg == swab32(I2C_DW_IC_COMP_TYPE_VALUE)) {
		dev->accessor_flags |= ACCESS_SWAP;
	}
	else if (reg == (I2C_DW_IC_COMP_TYPE_VALUE & 0x0000ffff)) {
		dev->accessor_flags |= ACCESS_16BIT;
	}
	else if (reg != I2C_DW_IC_COMP_TYPE_VALUE) {
		log_error("Unknown Synopsys component type: "
		          "0x%08x\n",
		    reg);
		return -ENODEV;
	}

	reg = i2c_dw_read(dev, I2C_DW_IC_COMP_PARAM_1);
	dev->tx_fifo_depth = ((reg >> 16) & 0xff) + 1;
	dev->rx_fifo_depth = ((reg >> 8) & 0xff) + 1;

	i2c_dw_enable_and_wait(dev, false);

	/* Set SCL timing parameters for standard-mode */
	i2c_dw_write(dev, 0x190, I2C_DW_IC_SS_SCL_HCNT);
	i2c_dw_write(dev, 0x1D6, I2C_DW_IC_SS_SCL_LCNT);

	/* Set SCL timing parameters for fast-mode or fast-mode plus */
	i2c_dw_write(dev, 0x3C, I2C_DW_IC_FS_SCL_HCNT);
	i2c_dw_write(dev, 0x82, I2C_DW_IC_FS_SCL_LCNT);

	/* Configure Tx/Rx FIFO threshold levels */
	i2c_dw_write(dev, dev->tx_fifo_depth / 2, I2C_DW_IC_TX_TL);
	i2c_dw_write(dev, 0, I2C_DW_IC_RX_TL);

	dev->master_cfg = I2C_DW_IC_CON_MASTER | I2C_DW_IC_CON_SLAVE_DISABLE
	                  | I2C_DW_IC_CON_RESTART_EN | I2C_DW_IC_CON_SPEED_FAST;
	i2c_dw_write(dev, dev->master_cfg, I2C_DW_IC_CON);

	return 0;
}

static int i2c_dw_init(const struct i2c_bus *bus) {
	struct i2c_dw_dev *dev;

	dev = bus->i2c_priv;

	i2c_dw_hw_init(dev);

	return irq_attach(dev->irq_num, i2c_dw_irq_handler, 0, dev, "i2c_dw");
}

const struct i2c_ops i2c_dw_ops = {
    .i2c_master_xfer = i2c_dw_xfer,
    .i2c_init = i2c_dw_init,
};

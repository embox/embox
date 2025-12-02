/**
 * @file
 * @brief I2C driver for NIIET K1921VG015
 *
 * @date Oct 23, 2025
 * @author Anton Bondarev
 * @author kimstik
 */

#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/i2c/i2c.h>

#define I2C_SYSTEM_FREQ    50000000
#define I2C_FS_FREQ        100000

struct niiet_i2c_regs {
	volatile uint32_t SDA;
	volatile uint32_t ST;
	volatile uint32_t CST;
	volatile uint32_t CTL0;
	volatile uint32_t ADDR;
	volatile uint32_t CTL1;
	volatile uint32_t TOPR;
	volatile uint32_t CTL2;
	volatile uint32_t CTL3;
	volatile uint32_t CTL4;
};

/* CTL0 register bits */
#define CTL0_START_BIT    (1 << 0)
#define CTL0_STOP_BIT     (1 << 1)
#define CTL0_INTEN_BIT    (1 << 2)
#define CTL0_ACK_BIT      (1 << 4)
#define CTL0_CLRST_BIT    (1 << 7)

/* CTL1 register bits */
#define CTL1_ENABLE_BIT   (1 << 0)
#define CTL1_SCLFRQ(v)    (((v) & 0x7F) << 1)

/* CTL3 register */
#define CTL3_SCLFRQ(v)    ((v) & 0xFF)

/* ST register */
#define ST_INT_BIT        (1 << 7)
#define ST_MODE(reg)      ((reg) & 0x3F)

/* CST register bits */
#define CST_BB_BIT        (1 << 0)
#define CST_TOERR_BIT     (1 << 3)

/* Timeout for polling loops - prevents infinite hangs */
#define I2C_TIMEOUT_LOOPS      100000

/* ST.MODE status codes (FS master mode only) */
#define I2C_ST_MODE_STDONE     0x01  /* Start condition generated */
#define I2C_ST_MODE_RSDONE     0x02  /* Repeated start generated */
#define I2C_ST_MODE_MTADPA     0x04  /* TX: slave addr ACK */
#define I2C_ST_MODE_MTADNA     0x05  /* TX: slave addr NAK */
#define I2C_ST_MODE_MTDAPA     0x06  /* TX: data byte ACK */
#define I2C_ST_MODE_MTDANA     0x07  /* TX: data byte NAK */
#define I2C_ST_MODE_MRADPA     0x08  /* RX: slave addr ACK */
#define I2C_ST_MODE_MRADNA     0x09  /* RX: slave addr NAK */
#define I2C_ST_MODE_MRDAPA     0x0A  /* RX: data byte ACK */
#define I2C_ST_MODE_MRDANA     0x0B  /* RX: data byte NAK */
#define I2C_ST_MODE_BERROR     0x1F  /* Bus error */
#define I2C_ST_MODE_HMTMCOK    0x21  /* HS mode entered (master code sent) */

static void vg015_i2c_dump_regs(struct niiet_i2c_regs *regs) {
	log_error("I2C: ST=0x%02x CST=0x%02x CTL0=0x%02x CTL1=0x%02x",
	    regs->ST, regs->CST, regs->CTL0, regs->CTL1);
}

/* Wait for INT flag with timeout and hardware error check.
 * Returns 0 on success, -ETIMEDOUT on timeout, -EIO on hardware error */
static int vg015_i2c_wait_int(struct niiet_i2c_regs *regs) {
	int timeout = I2C_TIMEOUT_LOOPS;
	uint32_t mode;

	while(!(regs->ST & ST_INT_BIT)) {
		/* Check for bus error (invalid start/stop condition) */
		mode = ST_MODE(regs->ST);
		if (mode == I2C_ST_MODE_BERROR) {
			log_error("I2C: bus error (BERROR)");
			vg015_i2c_dump_regs(regs);
			regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return -EIO;
		}
		/* Check for hardware timeout (slave clock stretch too long) */
		if (regs->CST & CST_TOERR_BIT) {
			log_error("I2C: hardware timeout (TOERR)");
			vg015_i2c_dump_regs(regs);
			/* Clear TOERR by writing CLRST */
			regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return -ETIMEDOUT;
		}
		if (--timeout == 0) {
			log_error("I2C: software timeout waiting for INT");
			vg015_i2c_dump_regs(regs);
			/* Controller may be stuck - will be reset by caller */
			return -ETIMEDOUT;
		}
	}
	return 0;
}

/* Wait for STOP condition to complete.
 * Returns 0 on success, -ETIMEDOUT on timeout */
static int vg015_i2c_wait_stop(struct niiet_i2c_regs *regs) {
	int timeout = I2C_TIMEOUT_LOOPS;

	/* Wait for STOP bit to clear (hardware finished sending STOP) */
	while(regs->CTL0 & CTL0_STOP_BIT) {
		if (regs->CST & CST_TOERR_BIT) {
			log_error("I2C: hardware timeout during STOP");
			regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return -ETIMEDOUT;
		}
		if (--timeout == 0) {
			log_error("I2C: timeout waiting for STOP completion");
			return -ETIMEDOUT;
		}
	}
	return 0;
}

static int vg015_i2c_tx(const struct i2c_bus *bus, uint16_t addr,
									uint8_t *buff, size_t sz) {
	struct niiet_i2c_regs *regs;
	uint32_t mode;
	size_t i;
	int ret;

	regs = (void *)bus->i2cb_label;

	/* Wait for start condition completion */
	ret = vg015_i2c_wait_int(regs);
	if (ret != 0) {
		return ret;
	}
	mode = ST_MODE(regs->ST);
	if (mode != I2C_ST_MODE_STDONE && mode != I2C_ST_MODE_RSDONE) {
		log_error("TX: expected STDONE/RSDONE, got 0x%x", mode);
		vg015_i2c_dump_regs(regs);
		return -EIO;
	}

	/* Send slave address with write bit (R/W = 0) */
	regs->SDA = (addr << 1);
	regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;

	/* Wait for address ACK */
	ret = vg015_i2c_wait_int(regs);
	if (ret != 0) {
		return ret;
	}
	mode = ST_MODE(regs->ST);
	if (mode == I2C_ST_MODE_MTADNA) {
		/* NAK is normal for device scanning (i2cdetect), not an error */
		log_debug("TX: slave 0x%02x not responding (NAK)", addr);
		regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		return -ENXIO;
	}
	if (mode == I2C_ST_MODE_HMTMCOK) {
		/* Addresses 0x04-0x07 when shifted become master codes (0x08-0x0E)
		 * which trigger HS mode. These are reserved I2C addresses.
		 * Send STOP to exit HS mode and return no-device. */
		log_debug("TX: addr 0x%02x triggered HS mode (reserved)", addr);
		regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		return -ENXIO;
	}
	if (mode != I2C_ST_MODE_MTADPA) {
		log_error("TX: expected MTADPA, got 0x%x", mode);
		vg015_i2c_dump_regs(regs);
		regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		return -EIO;
	}

	/* Quick write (sz=0): address sent, no data - used by i2cdetect.
	 * Must clear INT before returning, STOP is sent by master_xfer. */
	if (sz == 0) {
		regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		return 0;
	}

	/* Transmit data bytes */
	for (i = 0; i < sz; i++) {
		regs->SDA = buff[i];
		regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;

		ret = vg015_i2c_wait_int(regs);
		if (ret != 0) {
			regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return ret;
		}
		mode = ST_MODE(regs->ST);
		if (mode == I2C_ST_MODE_MTDANA) {
			log_error("TX: data NAK at byte %zu", i);
			vg015_i2c_dump_regs(regs);
			regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return -EIO;
		}
		if (mode != I2C_ST_MODE_MTDAPA) {
			log_error("TX: expected MTDAPA, got 0x%x", mode);
			vg015_i2c_dump_regs(regs);
			regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return -EIO;
		}
	}

	return sz;
}

static int vg015_i2c_rx(const struct i2c_bus *bus, uint16_t addr,
									uint8_t *buff, size_t sz) {
	struct niiet_i2c_regs *regs;
	uint32_t mode;
	size_t i;
	int ret;

	regs = (void *)bus->i2cb_label;

	/* Wait for start condition completion */
	ret = vg015_i2c_wait_int(regs);
	if (ret != 0) {
		return ret;
	}
	mode = ST_MODE(regs->ST);
	if (mode != I2C_ST_MODE_STDONE && mode != I2C_ST_MODE_RSDONE) {
		log_error("RX: expected STDONE/RSDONE, got 0x%x", mode);
		vg015_i2c_dump_regs(regs);
		return -EIO;
	}

	/* Send slave address with read bit (R/W = 1) */
	regs->SDA = (addr << 1) | 1;
	regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;

	/* Wait for address ACK */
	ret = vg015_i2c_wait_int(regs);
	if (ret != 0) {
		return ret;
	}
	mode = ST_MODE(regs->ST);
	if (mode == I2C_ST_MODE_MRADNA) {
		/* NAK is normal for device scanning (i2cdetect), not an error */
		log_debug("RX: slave 0x%02x not responding (NAK)", addr);
		regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		return -ENXIO;
	}
	if (mode != I2C_ST_MODE_MRADPA) {
		log_error("RX: expected MRADPA, got 0x%x", mode);
		vg015_i2c_dump_regs(regs);
		regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		return -EIO;
	}

	/* Receive data bytes */
	for (i = 0; i < sz; i++) {
		/* ACK bit: 0 = send ACK, 1 = send NACK
		 * Send ACK for all bytes except the last one */
		if (i < sz - 1) {
			regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		} else {
			/* NACK for last byte to signal end of transfer */
			regs->CTL0 = CTL0_ACK_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
		}

		ret = vg015_i2c_wait_int(regs);
		if (ret != 0) {
			regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return ret;
		}
		mode = ST_MODE(regs->ST);
		if (mode != I2C_ST_MODE_MRDAPA && mode != I2C_ST_MODE_MRDANA) {
			log_error("RX: expected MRDAPA/MRDANA, got 0x%x", mode);
			vg015_i2c_dump_regs(regs);
			regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
			return -EIO;
		}

		buff[i] = regs->SDA;
	}

	return sz;
}

static inline int niiet_i2c_is_busy(struct niiet_i2c_regs *regs) {
	return regs->CST & CST_BB_BIT;
}

/* Reset I2C controller to recover from stuck state */
static void vg015_i2c_reset(struct niiet_i2c_regs *regs) {
	uint32_t ctl1_save;

	log_warning("I2C: resetting controller");

	/* Save CTL1 (frequency settings) */
	ctl1_save = regs->CTL1;

	/* Disable I2C controller */
	regs->CTL1 &= ~CTL1_ENABLE_BIT;

	/* Small delay for disable to take effect */
	for (volatile int i = 0; i < 100; i++);

	/* Re-enable controller with saved settings */
	regs->CTL1 = ctl1_save | CTL1_ENABLE_BIT;

	/* Clear status and enable INT */
	regs->CTL0 = CTL0_CLRST_BIT | CTL0_INTEN_BIT;

	/* Generate STOP to reset bus state */
	regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
	vg015_i2c_wait_stop(regs);
}

static int vg015_i2c_master_xfer(const struct i2c_bus *bus,
						struct i2c_msg *msgs, size_t num_msgs) {
	struct niiet_i2c_regs *regs;
	int res = 0;
	size_t i;
	int timeout;

	regs = (void *)bus->i2cb_label;

	timeout = 0;
	while(niiet_i2c_is_busy(regs)) {
		if (timeout++ > 1000) {
			/* Bus stuck - try to recover */
			vg015_i2c_reset(regs);
			if (niiet_i2c_is_busy(regs)) {
				return -EBUSY;
			}
			break;
		}
	}

	for (i = 0; i < num_msgs; i++) {
		/* Generate start or repeated start condition.
		 * Use |= to preserve INTEN bit set during init */
		regs->CTL0 |= CTL0_START_BIT;

		if (msgs[i].flags & I2C_M_RD) {
			res = vg015_i2c_rx(bus, msgs[i].addr, msgs[i].buf, msgs[i].len);
		} else {
			res = vg015_i2c_tx(bus, msgs[i].addr, msgs[i].buf, msgs[i].len);
		}

		if (res < 0) {
			break;
		}
	}

	/* Generate stop condition and wait for completion */
	regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
	if (vg015_i2c_wait_stop(regs) != 0) {
		/* STOP failed - reset controller */
		vg015_i2c_reset(regs);
	}

	/* Reset controller after serious errors to prevent stuck state */
	if (res == -ETIMEDOUT || res == -EIO) {
		vg015_i2c_reset(regs);
	}

	return res;
}

extern int vg015_i2c_hw_init0(const struct i2c_bus *bus);

static int vg015_i2c_init(const struct i2c_bus *bus) {
	struct niiet_i2c_regs *regs;
	uint32_t freq_calc;

	regs = (void *)bus->i2cb_label;

	vg015_i2c_hw_init0(bus);

	freq_calc = I2C_SYSTEM_FREQ / (4 * I2C_FS_FREQ);
	regs->CTL1 = CTL1_SCLFRQ(freq_calc & 0x7F);
	regs->CTL3 = CTL3_SCLFRQ(freq_calc >> 7);

	/* Set HS mode dividers so STOP works if HS mode entered accidentally
	 * (reserved addresses 0x04-0x07 trigger HS mode) */
	regs->CTL2 = 0x80;  /* HSDIV[3:0] = 8 */
	regs->CTL4 = 0x00;  /* HSDIV[11:4] = 0 */

	regs->CTL1 |= CTL1_ENABLE_BIT;

	/* Enable INT flag generation (required for polling ST.INT) */
	regs->CTL0 = CTL0_INTEN_BIT;

	/* Generate dummy STOP to set BB=1 (bus busy) for clean state */
	regs->CTL0 = CTL0_STOP_BIT | CTL0_CLRST_BIT | CTL0_INTEN_BIT;
	vg015_i2c_wait_stop(regs);

	/* Check bus state - warn if stuck (possible missing pull-ups) */
	if (niiet_i2c_is_busy(regs)) {
		log_warning("I2C: bus busy after init - check pull-up resistors");
	}

	return 0;
}

const struct i2c_ops vg015_i2c_ops = {
    .i2c_master_xfer = vg015_i2c_master_xfer,
    .i2c_init = vg015_i2c_init,
};

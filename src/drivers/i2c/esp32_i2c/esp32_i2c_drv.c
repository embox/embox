/**
 * @file
 * @brief ESP32-C5 I2C0 bus instance.
 *
 * @date Oct 31, 2026
 * @author Dmitrii Piliuk
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/i2c/i2c.h>
#include <drivers/i2c/i2c_priv.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/time/ktime.h>

#include "esp32_i2c.h"

#define ESP32_I2C_SOURCE_HZ       OPTION_GET(NUMBER, source_clk_hz)
#define ESP32_I2C_BUS_HZ          100000U
#define ESP32_I2C_XFER_TIMEOUT_NS 100000000LL

static int esp32_i2c_init(const struct i2c_bus *bus) {
	uintptr_t base;
	uint32_t half_cycle, wait_high, timeout;

	if (!bus || !bus->i2cb_label) {
		return -EINVAL;
	}
	base = bus->i2cb_label;
	half_cycle = ESP32_I2C_SOURCE_HZ / ESP32_I2C_BUS_HZ / 2;
	wait_high = half_cycle / 2 - 2;
	/* Hardware timeout is encoded as an exponent, as in ESP-IDF LL. */
	timeout = 32U - __builtin_clz(5U * half_cycle) + 2U;

	REG32_STORE(base + ESP32_I2C_INT_ENA, 0);
	/* Master, MSB first, open-drain SDA/SCL. Arbitration is disabled
	 * for this initial single-master implementation.
	 */
	REG32_STORE(base + ESP32_I2C_CTR, ESP32_I2C_CTR_MASTER | ESP32_I2C_CTR_CLK_EN);
	REG32_ORIN(base + ESP32_I2C_CTR, ESP32_I2C_CTR_FSM_RST);
	REG32_STORE(base + ESP32_I2C_FIFO_CONF,
	    ESP32_I2C_FIFO_PRT_EN | ESP32_I2C_FIFO_RX_RST | ESP32_I2C_FIFO_TX_RST);
	REG32_STORE(base + ESP32_I2C_FIFO_CONF, ESP32_I2C_FIFO_PRT_EN);

	/* Filter glitches shorter than seven source clock cycles. */
	REG32_STORE(base + ESP32_I2C_FILTER_CFG,
	    7U | (7U << 4) | ESP32_I2C_FILTER_SCL_EN | ESP32_I2C_FILTER_SDA_EN);

	/* Match ESP-IDF LL timing adjustments: HIGH and WAIT_HIGH are
	 * written directly; LOW, SDA and START/STOP fields subtract one.
	 */
	REG32_STORE(base + ESP32_I2C_SCL_LOW, half_cycle - 1);
	REG32_STORE(base + ESP32_I2C_SCL_HIGH,
	    (half_cycle - wait_high) | (wait_high << 9));
	REG32_STORE(base + ESP32_I2C_SDA_HOLD, half_cycle / 4 - 1);
	REG32_STORE(base + ESP32_I2C_SDA_SAMPLE, half_cycle / 2 - 1);
	REG32_STORE(base + ESP32_I2C_START_HOLD, half_cycle - 1);
	REG32_STORE(base + ESP32_I2C_RESTART_SETUP, half_cycle - 1);
	REG32_STORE(base + ESP32_I2C_STOP_HOLD, half_cycle - 1);
	REG32_STORE(base + ESP32_I2C_STOP_SETUP, half_cycle - 1);
	REG32_STORE(base + ESP32_I2C_TIMEOUT, timeout | ESP32_I2C_TIMEOUT_EN);
	REG32_ORIN(base + ESP32_I2C_CTR, ESP32_I2C_CTR_CONF_UPDATE);
	REG32_STORE(base + ESP32_I2C_INT_CLR, ESP32_I2C_INT_MASK);

	return 0;
}

static void esp32_i2c_reset_fifos(uintptr_t base) {
	REG32_ORIN(base + ESP32_I2C_FIFO_CONF,
	    ESP32_I2C_FIFO_TX_RST | ESP32_I2C_FIFO_RX_RST);
	REG32_CLEAR(base + ESP32_I2C_FIFO_CONF,
	    ESP32_I2C_FIFO_TX_RST | ESP32_I2C_FIFO_RX_RST);
}

static int esp32_i2c_run(uintptr_t base, bool last) {
	const uint32_t errors = ESP32_I2C_NACK_INT | ESP32_I2C_ARBITRATION_LOST_INT
	                        | ESP32_I2C_TIME_OUT_INT | ESP32_I2C_SCL_ST_TO_INT
	                        | ESP32_I2C_SCL_MAIN_ST_TO_INT;
	const uint32_t done = last ? ESP32_I2C_TRANS_COMPLETE_INT
	                           : ESP32_I2C_END_DETECT_INT;
	time64_t start;
	uint32_t int_raw;
	int ret;

	REG32_ORIN(base + ESP32_I2C_CTR, ESP32_I2C_CTR_CONF_UPDATE);
	start = ktime_get_ns();
	REG32_ORIN(base + ESP32_I2C_CTR, ESP32_I2C_CTR_TRANS_START);

	for (;;) {
		int_raw = REG32_LOAD(base + ESP32_I2C_INT_RAW);
		/* Errors take precedence over completion in the same snapshot. */
		if (int_raw & errors) {
			if (int_raw & ESP32_I2C_ARBITRATION_LOST_INT) {
				ret = -EAGAIN;
			}
			else if (int_raw & ESP32_I2C_NACK_INT) {
				ret = -ENXIO;
			}
			else {
				ret = -ETIMEDOUT;
			}
			break;
		}
		if (int_raw & done) {
			REG32_STORE(base + ESP32_I2C_INT_CLR, ESP32_I2C_INT_MASK);
			return 0;
		}
		if (ktime_get_ns() - start >= ESP32_I2C_XFER_TIMEOUT_NS) {
			ret = -ETIMEDOUT;
			break;
		}
	}

	/* Abort the command engine and discard unsent data before retrying.
	 * A slave holding SDA low may additionally require bus recovery.
	 */
	REG32_ORIN(base + ESP32_I2C_CTR, ESP32_I2C_CTR_FSM_RST);
	esp32_i2c_reset_fifos(base);
	REG32_ORIN(base + ESP32_I2C_CTR, ESP32_I2C_CTR_CONF_UPDATE);
	REG32_STORE(base + ESP32_I2C_INT_CLR, ESP32_I2C_INT_MASK);
	return ret;
}

static int esp32_i2c_tx(uintptr_t base, uint16_t addr, const uint8_t *buff,
    size_t sz, bool last) {
	size_t i;
	int ret;

	if (addr > 0x7f || (sz && !buff)) {
		return -EINVAL;
	}
	/* Reserve one FIFO byte for the slave address. */
	if (sz >= ESP32_I2C_FIFO_LEN) {
		return -EMSGSIZE;
	}

	esp32_i2c_reset_fifos(base);
	REG32_STORE(base + ESP32_I2C_INT_CLR, ESP32_I2C_INT_MASK);
	REG32_STORE(base + ESP32_I2C_COMD0, ESP32_I2C_CMD_RSTART);
	REG32_STORE(base + ESP32_I2C_COMD1, ESP32_I2C_CMD_WRITE
	                                        | ESP32_I2C_CMD_BYTE_NUM(sz + 1)
	                                        | ESP32_I2C_CMD_ACK_CHECK_EN);
	REG32_STORE(base + ESP32_I2C_COMD2,
	    last ? ESP32_I2C_CMD_STOP : ESP32_I2C_CMD_END);

	REG32_STORE(base + ESP32_I2C_DATA, addr << 1);
	for (i = 0; i < sz; i++) {
		REG32_STORE(base + ESP32_I2C_DATA, buff[i]);
	}

	ret = esp32_i2c_run(base, last);
	return ret < 0 ? ret : (int)sz;
}

static int esp32_i2c_rx(uintptr_t base, uint16_t addr, uint8_t *buff, size_t sz,
    bool last) {
	unsigned int cmd = ESP32_I2C_COMD2;
	size_t i;
	int ret;

	if (addr > 0x7f || (sz && !buff)) {
		return -EINVAL;
	}
	if (sz > ESP32_I2C_FIFO_LEN) {
		return -EMSGSIZE;
	}

	esp32_i2c_reset_fifos(base);
	REG32_STORE(base + ESP32_I2C_INT_CLR, ESP32_I2C_INT_MASK);
	REG32_STORE(base + ESP32_I2C_COMD0, ESP32_I2C_CMD_RSTART);
	REG32_STORE(base + ESP32_I2C_COMD1, ESP32_I2C_CMD_WRITE
	                                        | ESP32_I2C_CMD_BYTE_NUM(1)
	                                        | ESP32_I2C_CMD_ACK_CHECK_EN);
	REG32_STORE(base + ESP32_I2C_DATA, (addr << 1) | 1U);

	/* ACK all but the last byte; NACK terminates this read message. */
	if (sz > 1) {
		REG32_STORE(base + cmd,
		    ESP32_I2C_CMD_READ | ESP32_I2C_CMD_BYTE_NUM(sz - 1));
		cmd += sizeof(uint32_t);
	}
	if (sz) {
		REG32_STORE(base + cmd, ESP32_I2C_CMD_READ | ESP32_I2C_CMD_BYTE_NUM(1)
		                            | ESP32_I2C_CMD_ACK_VAL);
		cmd += sizeof(uint32_t);
	}
	REG32_STORE(base + cmd, last ? ESP32_I2C_CMD_STOP : ESP32_I2C_CMD_END);

	ret = esp32_i2c_run(base, last);
	if (ret < 0) {
		return ret;
	}
	for (i = 0; i < sz; i++) {
		buff[i] = REG32_LOAD(base + ESP32_I2C_DATA);
	}
	return sz;
}

static int esp32_i2c_master_xfer(const struct i2c_bus *bus,
    struct i2c_msg *msgs, size_t num_msgs) {
	int res = 0;
	size_t i;
	uintptr_t base;

	if (!bus || !bus->i2cb_label || (num_msgs && !msgs)) {
		return -EINVAL;
	}

	/* Validate the whole transaction before an END can hold the bus. */
	for (i = 0; i < num_msgs; i++) {
		if (msgs[i].addr > 0x7f || (msgs[i].len && !msgs[i].buf)) {
			return -EINVAL;
		}
		if (msgs[i].len
		    > ESP32_I2C_FIFO_LEN - ((msgs[i].flags & I2C_M_RD) ? 0 : 1)) {
			return -EMSGSIZE;
		}
	}

	base = bus->i2cb_label;

	for (i = 0; i < num_msgs; i++) {
		if (msgs[i].flags & I2C_M_RD) {
			res = esp32_i2c_rx(base, msgs[i].addr, msgs[i].buf, msgs[i].len,
			    i == num_msgs - 1);
		}
		else {
			res = esp32_i2c_tx(base, msgs[i].addr, msgs[i].buf, msgs[i].len,
			    i == num_msgs - 1);
		}
		if (res < 0) {
			return res;
		}
	}

	return res;
}

const struct i2c_bus_ops esp32_i2c_ops = {
    .i2c_master_xfer = esp32_i2c_master_xfer,
    .i2c_init = esp32_i2c_init,
};

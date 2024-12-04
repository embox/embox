/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <hal/reg.h>

#include <drivers/i2c/i2c.h>

#include "imx_i2c.h"

static inline int imx_i2c_wait_status(struct imx_i2c *adapter, uint8_t mask, int for_set) {
	uint8_t temp;
	volatile int i;

	for (i = 0; i < 10000; i++) {
		temp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2SR);
		/* check for arbitration lost */
		if (temp & IMX_I2C_I2SR_IAL) {
			temp &= ~IMX_I2C_I2SR_IAL;
			REG8_STORE(adapter->base_addr + IMX_I2C_I2SR, temp);
			log_error("arbitration lost");
			return -EAGAIN;
		}
		if (for_set && (temp & mask) ) {
			return 0;
		}
		if (!for_set && !(temp & mask) ) {
			return 0;
		}
	}
	return -ETIMEDOUT;
}

static int imx_i2c_tx_byte(struct imx_i2c *adapter, uint8_t byte) {
	int res;

	REG8_STORE(adapter->base_addr + IMX_I2C_I2SR, 0);
	REG8_STORE(adapter->base_addr + IMX_I2C_I2DR, byte);
	res = imx_i2c_wait_status(adapter, IMX_I2C_I2SR_IIF, 1);
	if (res) {
		return res;
	}

	if (REG8_LOAD(adapter->base_addr + IMX_I2C_I2SR) &  IMX_I2C_I2SR_RXAK) {
		return -ENODEV;
	}

	return 0;
}

static int imx_i2c_tx(struct imx_i2c *adapter, uint16_t addr, uint8_t *buff, size_t sz) {
	int res;
	int i;

	res = imx_i2c_tx_byte(adapter, (addr << 1) & 0xFF);
	if (res < 0) {
		return res;
	}

	log_debug("ACK received 0x%x", addr);
	res = sz;
	for (i = 0; i < sz; i++) {
		log_debug("write byte: %d=0x%X", i, buff[i]);
		res = imx_i2c_tx_byte(adapter, buff[i]);
		if (res < 0) {
			return res;
		}
		log_debug("byte sent addr (0x%x) val (0x%x)", addr, buff[i]);
	}

	return res;
}

static int imx_i2c_stop(struct imx_i2c *adapter) {
	uint8_t tmp;

	tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
	tmp &= ~(IMX_I2C_I2CR_MTX | IMX_I2C_I2CR_TXAK);
	tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
	/* delay(1); */
	tmp &= ~(IMX_I2C_I2CR_MSTA) ;
	REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, tmp);
	usleep(100);

	REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, 0);
	usleep(100);

	return 0;
}

static int imx_i2c_start(struct imx_i2c *adapter) {
	uint8_t tmp;

	REG8_STORE(adapter->base_addr + IMX_I2C_I2SR, 0 );
	REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, IMX_I2C_I2CR_IEN );

	usleep(200);

	/* Start I2C transaction */
	tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
	tmp |= IMX_I2C_I2CR_MSTA ;
	REG8_STORE(adapter->base_addr+ IMX_I2C_I2CR, tmp);

	tmp = imx_i2c_wait_status(adapter, IMX_I2C_I2SR_IBB, 1);
	if (tmp) {
		return tmp;
	}

	tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
	tmp |= IMX_I2C_I2CR_MTX | IMX_I2C_I2CR_TXAK;
	REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, tmp);

	return 0;
}


static int
imx_i2c_rx(struct imx_i2c *adapter, uint16_t addr, uint8_t *buff, size_t sz) {
	int res;
	int cnt;
	uint8_t tmp;

	res = imx_i2c_tx_byte(adapter, ((addr << 1) | 0x1) & 0xFF);
	if (res < 0) {
		return res;
	}

	log_debug("ACK received 0x%x", addr);
	tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
	tmp &= ~IMX_I2C_I2CR_MTX;
	if (sz > 1) {
		tmp &= ~IMX_I2C_I2CR_TXAK;
	}
	REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, tmp);

	REG8_STORE(adapter->base_addr + IMX_I2C_I2SR, 0);
	/* dummy read */
	tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2DR);

	for (cnt = sz; cnt > 0; cnt--) {
		res = imx_i2c_wait_status(adapter, IMX_I2C_I2SR_IIF, 1);
		if (res) {
			log_error("i2c completion error %d", res);
			return res;
		}

		log_debug("i2c completion ok");

		if (cnt == 1) {
			/*
			 * It must generate STOP before read I2DR to prevent
			 * controller from generating another clock cycle
			 */
			tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
			tmp &= ~(IMX_I2C_I2CR_MTX | IMX_I2C_I2CR_MSTA);
			REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, tmp);
		}
		if (cnt == 2) {
			tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
			tmp |= IMX_I2C_I2CR_TXAK;
			REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, tmp);
		}

		/* wait for free bus */
		tmp = imx_i2c_wait_status(adapter, IMX_I2C_I2SR_IBB, 0);
		REG8_STORE(adapter->base_addr + IMX_I2C_I2SR, 0);
		tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2DR);

		log_debug("read 0x%x ", tmp);
		*buff++ = tmp;
	}

	return sz;
}

static int
imx_i2c_master_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num) {
	struct imx_i2c *adapter;
	int res = -1;
	int i;
	uint8_t tmp;

	adapter = adap->i2c_algo_data;

	res = imx_i2c_start(adapter);
	if (res) {
		log_error("i2c  bus error");
		goto out;
	}

	for (i = 0; i < num; i ++) {
		if (i) {
			tmp = REG8_LOAD(adapter->base_addr + IMX_I2C_I2CR);
			tmp |= IMX_I2C_I2CR_RSTA;
			REG8_STORE(adapter->base_addr + IMX_I2C_I2CR, tmp);
			res = imx_i2c_wait_status(adapter, IMX_I2C_I2SR_IBB, 1);
			if (res) {
				break;
			}
		}

		if (msgs[i].flags & I2C_M_RD) {
			res = imx_i2c_rx(adapter, msgs->addr, msgs->buf, msgs->len);
		} else {
			res = imx_i2c_tx(adapter, msgs->addr, msgs->buf, msgs->len);
		}

	}

out:
	imx_i2c_stop(adapter);

	return res;
}

const struct i2c_algorithm imx_i2c_algo = {
	.i2c_master_xfer = imx_i2c_master_xfer,
};

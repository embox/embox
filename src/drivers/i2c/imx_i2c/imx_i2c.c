/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <drivers/i2c/i2c.h>
#include <hal/reg.h>
#include <util/log.h>

#include "imx_i2c.h"

static inline int imx_i2c_wait_status(const struct imx_i2c_priv *priv,
    uint8_t mask, int for_set) {
	uint8_t temp;
	int i;

	for (i = 0; i < 10000; i++) {
		temp = REG8_LOAD(priv->base_addr + IMX_I2C_I2SR);
		/* check for arbitration lost */
		if (temp & IMX_I2C_I2SR_IAL) {
			temp &= ~IMX_I2C_I2SR_IAL;
			REG8_STORE(priv->base_addr + IMX_I2C_I2SR, temp);
			log_error("arbitration lost");
			return -EAGAIN;
		}

		if (for_set && (temp & mask)) {
			return 0;
		}

		if (!for_set && !(temp & mask)) {
			return 0;
		}
	}

	return -ETIMEDOUT;
}

static int imx_i2c_tx_byte(const struct imx_i2c_priv *priv, uint8_t byte) {
	int res;

	REG8_STORE(priv->base_addr + IMX_I2C_I2SR, 0);
	REG8_STORE(priv->base_addr + IMX_I2C_I2DR, byte);

	res = imx_i2c_wait_status(priv, IMX_I2C_I2SR_IIF, 1);
	if (res) {
		return res;
	}

	if (REG8_LOAD(priv->base_addr + IMX_I2C_I2SR) & IMX_I2C_I2SR_RXAK) {
		return -ENODEV;
	}

	return 0;
}

static int imx_i2c_tx(const struct imx_i2c_priv *priv, uint16_t addr,
    uint8_t *buff, size_t sz) {
	int res;
	int i;

	res = imx_i2c_tx_byte(priv, (addr << 1) & 0xFF);
	if (res < 0) {
		return res;
	}

	log_debug("ACK received 0x%x", addr);

	res = sz;
	for (i = 0; i < sz; i++) {
		log_debug("write byte: %d=0x%X", i, buff[i]);
		res = imx_i2c_tx_byte(priv, buff[i]);
		if (res < 0) {
			return res;
		}
		log_debug("byte sent addr (0x%x) val (0x%x)", addr, buff[i]);
	}

	return res;
}

static int imx_i2c_stop(const struct imx_i2c_priv *priv) {
	uint8_t tmp;

	tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
	tmp &= ~(IMX_I2C_I2CR_MTX | IMX_I2C_I2CR_TXAK);
	tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
	/* delay(1); */
	tmp &= ~(IMX_I2C_I2CR_MSTA);
	REG8_STORE(priv->base_addr + IMX_I2C_I2CR, tmp);
	usleep(100);

	REG8_STORE(priv->base_addr + IMX_I2C_I2CR, 0);
	usleep(100);

	return 0;
}

static int imx_i2c_start(const struct imx_i2c_priv *priv) {
	uint8_t tmp;

	REG8_STORE(priv->base_addr + IMX_I2C_I2SR, 0);
	REG8_STORE(priv->base_addr + IMX_I2C_I2CR, IMX_I2C_I2CR_IEN);

	usleep(200);

	/* Start I2C transaction */
	tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
	tmp |= IMX_I2C_I2CR_MSTA;
	REG8_STORE(priv->base_addr + IMX_I2C_I2CR, tmp);

	tmp = imx_i2c_wait_status(priv, IMX_I2C_I2SR_IBB, 1);
	if (tmp) {
		return tmp;
	}

	tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
	tmp |= IMX_I2C_I2CR_IIEN | IMX_I2C_I2CR_MTX | IMX_I2C_I2CR_TXAK;
	REG8_STORE(priv->base_addr + IMX_I2C_I2CR, tmp);

	return 0;
}

static int imx_i2c_rx(const struct imx_i2c_priv *priv, uint16_t addr,
    uint8_t *buff, size_t sz) {
	int res;
	int cnt;
	uint8_t tmp;

	log_debug("start read %d bytes", sz);

	res = imx_i2c_tx_byte(priv, ((addr << 1) | 0x1) & 0xFF);
	if (res < 0) {
		return res;
	}

	log_debug("ACK received 0x%x", addr);

	tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
	tmp &= ~IMX_I2C_I2CR_MTX;
	if (sz > 1) {
		tmp &= ~IMX_I2C_I2CR_TXAK;
	}
	REG8_STORE(priv->base_addr + IMX_I2C_I2CR, tmp);

	REG8_STORE(priv->base_addr + IMX_I2C_I2SR, 0);
	/* dummy read */
	tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2DR);

	for (cnt = sz; cnt > 0; cnt--) {
		res = imx_i2c_wait_status(priv, IMX_I2C_I2SR_IIF, 1);
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
			tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
			tmp &= ~(IMX_I2C_I2CR_MTX | IMX_I2C_I2CR_MSTA);
			REG8_STORE(priv->base_addr + IMX_I2C_I2CR, tmp);
		}
		if (cnt == 2) {
			tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
			tmp |= IMX_I2C_I2CR_TXAK;
			REG8_STORE(priv->base_addr + IMX_I2C_I2CR, tmp);
		}

		/* wait for free bus */
		tmp = imx_i2c_wait_status(priv, IMX_I2C_I2SR_IBB, 0);
		REG8_STORE(priv->base_addr + IMX_I2C_I2SR, 0);
		tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2DR);

		log_debug("read 0x%x ", tmp);
		*buff++ = tmp;
	}

	log_debug("end read %d bytes", sz);

	return sz;
}

static int imx_i2c_master_xfer(const struct i2c_bus *bus, struct i2c_msg *msgs,
    size_t num_msgs) {
	const struct imx_i2c_priv *priv;
	int res = -1;
	int i;
	uint8_t tmp;

	priv = (const struct imx_i2c_priv *)bus->i2c_priv;

	res = imx_i2c_start(priv);
	if (res) {
		log_error("i2c  bus error");
		goto out;
	}

	for (i = 0; i < num_msgs; i++) {
		if (i) {
			tmp = REG8_LOAD(priv->base_addr + IMX_I2C_I2CR);
			tmp |= IMX_I2C_I2CR_RSTA;
			REG8_STORE(priv->base_addr + IMX_I2C_I2CR, tmp);
			res = imx_i2c_wait_status(priv, IMX_I2C_I2SR_IBB, 1);
			if (res) {
				break;
			}
		}

		if (msgs[i].flags & I2C_M_RD) {
			res = imx_i2c_rx(priv, msgs->addr, msgs[i].buf, msgs[i].len);
		}
		else {
			res = imx_i2c_tx(priv, msgs->addr, msgs[i].buf, msgs[i].len);
		}
	}

out:
	imx_i2c_stop(priv);

	return res;
}

static int imx_i2c_init(const struct i2c_bus *bus) {
	const struct imx_i2c_priv *priv;

	priv = (const struct imx_i2c_priv *)bus->i2c_priv;

	if (priv->init_pins) {
		priv->init_pins();
	}

	return 0;
}

const struct i2c_ops imx_i2c_ops = {
    .i2c_master_xfer = imx_i2c_master_xfer,
    .i2c_init = imx_i2c_init,
};

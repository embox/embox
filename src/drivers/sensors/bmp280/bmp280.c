/**
 * @file
 * @brief BMP280 measurements and integer compensation.
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include <util/log.h>

#include "bmp280.h"
#include "bmp280_transfer.h"

#define BMP280_CALIB     0x88
#define BMP280_ID        0xd0
#define BMP280_RESET     0xe0
#define BMP280_STATUS    0xf3
#define BMP280_CTRL_MEAS 0xf4
#define BMP280_DATA      0xf7
#define BMP280_MEASURING (1U << 3)
#define BMP280_IM_UPDATE (1U << 0)
#define BMP280_CHIP_ID   0x58
#define BMP280_RESET_VAL 0xb6
/* Temperature x1, pressure x1, normal mode. */
#define BMP280_MEAS_X1   ((1U << 5) | (1U << 2) | 3U)

extern struct bmp280_dev bmp280_dev0;

static struct {
	uint16_t t1;
	int16_t t2, t3;
	uint16_t p1;
	int16_t p2, p3, p4, p5, p6, p7, p8, p9;
} calib;
static bool initialized;

static uint16_t bmp280_le16(const uint8_t *p) {
	return p[0] | ((uint16_t)p[1] << 8);
}

static int32_t bmp280_adc20(const uint8_t *p) {
	return ((int32_t)p[0] << 12) | ((int32_t)p[1] << 4) | (p[2] >> 4);
}

int bmp280_init(void) {
	uint8_t id, status, data[24];
	int ret, i;

	initialized = false;
	ret = bmp280_hw_init(&bmp280_dev0);
	if (ret < 0) {
		return ret;
	}
	ret = bmp280_readb(&bmp280_dev0, BMP280_ID, &id);
	if (ret < 0) {
		return ret;
	}
	if (id != BMP280_CHIP_ID) {
		log_error("BMP280: wrong chip id 0x%02x", id);
		return -ENODEV;
	}
	ret = bmp280_writeb(&bmp280_dev0, BMP280_RESET, BMP280_RESET_VAL);
	if (ret < 0) {
		return ret;
	}
	/* Allow reset and NVM calibration copy to start before polling. */
	usleep(2000);
	for (i = 0; i < 100; i++) {
		ret = bmp280_readb(&bmp280_dev0, BMP280_STATUS, &status);
		if (ret < 0) {
			return ret;
		}
		if (!(status & BMP280_IM_UPDATE)) {
			break;
		}
		usleep(1000);
	}
	if (i == 100) {
		return -ETIMEDOUT;
	}
	ret = bmp280_read(&bmp280_dev0, BMP280_CALIB, data, sizeof(data));
	if (ret < 0) {
		return ret;
	}
	calib.t1 = bmp280_le16(data);
	calib.t2 = (int16_t)bmp280_le16(data + 2);
	calib.t3 = (int16_t)bmp280_le16(data + 4);
	calib.p1 = bmp280_le16(data + 6);
	calib.p2 = (int16_t)bmp280_le16(data + 8);
	calib.p3 = (int16_t)bmp280_le16(data + 10);
	calib.p4 = (int16_t)bmp280_le16(data + 12);
	calib.p5 = (int16_t)bmp280_le16(data + 14);
	calib.p6 = (int16_t)bmp280_le16(data + 16);
	calib.p7 = (int16_t)bmp280_le16(data + 18);
	calib.p8 = (int16_t)bmp280_le16(data + 20);
	calib.p9 = (int16_t)bmp280_le16(data + 22);
	if (!calib.p1 || calib.p1 == 0xffff) {
		return -EIO;
	}
	ret = bmp280_writeb(&bmp280_dev0, BMP280_CTRL_MEAS, BMP280_MEAS_X1);
	if (ret < 0) {
		return ret;
	}
	/* First x1 temperature + pressure conversion takes at most 6.425 ms. */
	usleep(10000);
	initialized = true;
	return 0;
}

static int bmp280_wait_sleep(void) {
	uint8_t status, ctrl;
	int i, ret;

	for (i = 0; i < 100; i++) {
		ret = bmp280_readb(&bmp280_dev0, BMP280_STATUS, &status);
		if (ret < 0) {
			return ret;
		}
		ret = bmp280_readb(&bmp280_dev0, BMP280_CTRL_MEAS, &ctrl);
		if (ret < 0) {
			return ret;
		}
		if (!(status & (BMP280_MEASURING | BMP280_IM_UPDATE)) && !(ctrl & 3U)) {
			return 0;
		}
		usleep(1000);
	}
	return -ETIMEDOUT;
}

int bmp280_force_measurement(unsigned int pressure_osrs) {
	uint8_t settings;
	int ret;

	if (pressure_osrs > 5) {
		return -EINVAL;
	}
	/* Keep temperature x1; pressure codes 0..5 mean off, x1..x16. */
	settings = (1U << 5) | (pressure_osrs << 2);

	if (!initialized) {
		return -EIO;
	}
	/* Finish any normal-mode conversion before starting a single shot. */
	ret = bmp280_writeb(&bmp280_dev0, BMP280_CTRL_MEAS, settings);
	if (ret < 0) {
		return ret;
	}
	ret = bmp280_wait_sleep();
	if (ret < 0) {
		return ret;
	}
	ret = bmp280_writeb(&bmp280_dev0, BMP280_CTRL_MEAS, settings | 1U);
	if (ret < 0) {
		return ret;
	}
	/* Avoid interpreting the interval before conversion starts as done. */
	usleep(10000);
	return bmp280_wait_sleep();
}

int bmp280_read_snapshot(struct bmp280_snapshot *snapshot) {
	uint8_t regs[3];
	int ret;
	unsigned int i;

	if (!snapshot) {
		return -EINVAL;
	}
	ret = bmp280_readb(&bmp280_dev0, BMP280_ID, &snapshot->id);
	if (ret < 0) {
		return ret;
	}
	ret = bmp280_read(&bmp280_dev0, BMP280_STATUS, regs, sizeof(regs));
	if (ret < 0) {
		return ret;
	}
	snapshot->status = regs[0];
	snapshot->ctrl_meas = regs[1];
	snapshot->config = regs[2];
	/* Read temperature first, then pressure, in separate transactions. */
	ret = bmp280_read(&bmp280_dev0, BMP280_DATA + 3, snapshot->burst + 3, 3);
	if (ret < 0) {
		return ret;
	}
	ret = bmp280_read(&bmp280_dev0, BMP280_DATA, snapshot->burst, 3);
	if (ret < 0) {
		return ret;
	}
	/* Repeat byte by byte in the same T -> P order.
	 * In normal mode a new conversion can occur between these reads.
	 */
	for (i = 0; i < sizeof(snapshot->single); i++) {
		unsigned int offset = (i + 3) % sizeof(snapshot->single);

		ret = bmp280_readb(&bmp280_dev0, BMP280_DATA + offset,
		    &snapshot->single[offset]);
		if (ret < 0) {
			return ret;
		}
	}
	return 0;
}

int bmp280_read_values(int32_t *temperature, int32_t *pressure) {
	uint8_t data[6];
	int32_t adc_t, adc_p, fine;
	int64_t v1, v2, p, delta;
	int ret;

	if (!temperature || !pressure) {
		return -EINVAL;
	}
	if (!initialized) {
		return -EIO;
	}
	/* One burst keeps pressure and temperature in the same sample. */
	ret = bmp280_read(&bmp280_dev0, BMP280_DATA, data, sizeof(data));
	if (ret < 0) {
		return ret;
	}
	adc_p = bmp280_adc20(data);
	adc_t = bmp280_adc20(data + 3);
	if (adc_t == 0x80000 || adc_p == 0x80000) {
		return -EIO;
	}

	/* Bosch datasheet, section 3.11.3: integer compensation.
	 * Multiplication replaces left shifts of potentially negative values.
	 */
	v1 = (((int64_t)(adc_t >> 3) - (int64_t)calib.t1 * 2) * calib.t2) >> 11;
	delta = (adc_t >> 4) - (int64_t)calib.t1;
	v2 = (((delta * delta) >> 12) * calib.t3) >> 14;
	fine = v1 + v2;

	v1 = (int64_t)fine - 128000;
	v2 = v1 * v1 * calib.p6;
	v2 += v1 * calib.p5 * 131072;
	v2 += (int64_t)calib.p4 * 34359738368LL;
	v1 = ((v1 * v1 * calib.p3) >> 8) + v1 * calib.p2 * 4096;
	v1 = (((1LL << 47) + v1) * calib.p1) >> 33;
	if (!v1) {
		return -EIO;
	}
	p = 1048576 - adc_p;
	p = ((p * 2147483648LL - v2) * 3125) / v1;
	v1 = ((int64_t)calib.p9 * (p >> 13) * (p >> 13)) >> 25;
	v2 = ((int64_t)calib.p8 * p) >> 19;
	p = ((p + v1 + v2) >> 8) + (int64_t)calib.p7 * 16;
	if (p <= 0) {
		log_error("BMP280: invalid pressure: adc_p=%d adc_t=%d fine=%d",
		    (int)adc_p, (int)adc_t, (int)fine);
		log_error("BMP280: P1=%u P2=%d P3=%d P4=%d P5=%d", (unsigned)calib.p1,
		    (int)calib.p2, (int)calib.p3, (int)calib.p4, (int)calib.p5);
		log_error("BMP280: P6=%d P7=%d P8=%d P9=%d", (int)calib.p6,
		    (int)calib.p7, (int)calib.p8, (int)calib.p9);
		return -EIO;
	}
	*temperature = ((int64_t)fine * 5 + 128) >> 8;
	*pressure = p >> 8;
	return 0;
}

int32_t bmp280_get_temperature(void) {
	int32_t temperature, pressure;

	return bmp280_read_values(&temperature, &pressure) < 0 ? INT32_MIN
	                                                       : temperature;
}

int32_t bmp280_get_pressure(void) {
	int32_t temperature, pressure;

	return bmp280_read_values(&temperature, &pressure) < 0 ? INT32_MIN : pressure;
}

int32_t bmp280_get_humidity(void) {
	/* BMP280 has no humidity sensor. */
	return 0;
}

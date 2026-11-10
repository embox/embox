/**
 * @file
 * @brief
 *
 * @date 06.11.2026
 * @author Anton Bondarev
 */

#include <unistd.h>
#include <util/log.h>

#include "bmi160.h"
#include "bmi160_transfer.h"

extern struct bmi160_dev bmi160_dev0;

#define BMI160_REG_CHIPID 0x00
# define     BMI160_CHIPID 0xD1
#define BMI160_REG_ERR_REG 0x02
#define BMI160_REG_FIFO_LENGTH 0x22
#define BMI160_REG_FIFO_DATA 0x24
#define BMI160_REG_ACC_CONF 0x40
#define BMI160_REG_ACC_RANGE 0x41
            /* For convenience, use log2(range) - 1 instead of bits defined in
             * the datasheet. See _configure_accel(). */
# define     BMI160_ACC_RANGE_16G 3
#define BMI160_REG_GYR_CONF 0x42
#define BMI160_REG_GYR_RANGE 0x43
# define     BMI160_GYR_RANGE_2000DPS 0x00
#define BMI160_REG_FIFO_CONFIG_0 0x46
#define BMI160_REG_FIFO_CONFIG_1 0x47
# define     BMI160_FIFO_ACC_EN 0x40
# define     BMI160_FIFO_GYR_EN 0x80
#define BMI160_REG_INT_EN_1 0x51
# define     BMI160_INT_FWM_EN 0x40
#define BMI160_REG_INT_OUT_CTRL 0x53
# define     BMI160_INT1_LVL 0x02
# define     BMI160_INT1_OUTPUT_EN 0x08
#define BMI160_REG_INT_MAP_1 0x56
# define     BMI160_INT_MAP_INT1_FWM 0x40
#define BMI160_REG_CMD 0x7E
# define     BMI160_CMD_ACCEL_NORMAL_POWER_MODE 0x11
# define     BMI160_CMD_GYRO_NORMAL_POWER_MODE 0x15
# define     BMI160_CMD_FIFO_FLUSH 0xB0
# define     BMI160_CMD_SOFTRESET 0xB6

#define BMI160_READ_FLAG                0x80
#define BMI160_HARDWARE_INIT_MAX_TRIES  5

/* Datasheet says that the device powers up in less than 10ms, so waiting for
 * 10 ms before initialization is enough. */
#define BMI160_POWERUP_DELAY_MSEC 10
/* TODO: Investigate this. The delay below is way too high and with that
 * there's still at least 1% of failures on initialization. Lower values
 * increase that percentage. */
#define BMI160_SOFTRESET_DELAY_MSEC 100
/* Define a little bit more than the maximum value in the datasheet's timing
 * table. The datasheet recommends adding 300 us to the time for startup
 * occasions. */
#define BMI160_ACCEL_NORMAL_POWER_MODE_DELAY_MSEC 4
#define BMI160_GYRO_NORMAL_POWER_MODE_DELAY_MSEC 81

int bmi160_init(void) {
	int i;
	uint8_t tmp;
	struct bmi160_dev *dev = &bmi160_dev0;

	if (bmi160_hw_init(dev) < 0) {
		log_error("bmi160 hw init failed!");
		return -1;
	}

    for ( i = 0; i < BMI160_HARDWARE_INIT_MAX_TRIES; i++) {

        ret = bmi160_writeb(dev, BMI160_REG_CMD, BMI160_CMD_SOFTRESET);
        if (!ret) {
            continue;
        }

       msleep(BMI160_SOFTRESET_DELAY_MSEC);

        /* The datasheet recommends doing a read operation on the register 0x7F
         * in order to guarantee the sensor works using the SPI protocol. This
         * shouldn't have side effects for I2C. */
        ret = bmi160_readb(0x7F, &v, 1);
        if (!ret) {
            continue;
        }

        ret = bmi160_readb(BMI160_REG_CHIPID, &v, 1);
        if (!ret) {
            continue;
        }
        if (v != BMI160_CHIPID) {
            ret = false;
            continue;
        }

        ret = bmi160_writeb(BMI160_REG_CMD, BMI160_CMD_ACCEL_NORMAL_POWER_MODE);
        if (!ret) {
            continue;
        }
        msleep(BMI160_ACCEL_NORMAL_POWER_MODE_DELAY_MSEC);

        ret = bmi160_writeb(BMI160_REG_CMD, BMI160_CMD_GYRO_NORMAL_POWER_MODE);
        if (!ret) {
            continue;
        }
        msleep(BMI160_GYRO_NORMAL_POWER_MODE_DELAY_MSEC);

        break;
    }

	return 0;
	
}

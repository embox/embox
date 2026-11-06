/**
 * @file
 * @brief
 *
 * @date    09.04.2026
 * @author  Efim Perevalov
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <drivers/sensors/bmp280.h>

static void print_raw(const char *label, const uint8_t *data) {
	unsigned int pressure, temperature;

	pressure = ((unsigned int)data[0] << 12) | ((unsigned int)data[1] << 4)
	           | (data[2] >> 4);
	temperature = ((unsigned int)data[3] << 12) | ((unsigned int)data[4] << 4)
	              | (data[5] >> 4);
	printf("  %s: T=%02x %02x %02x (%u), P=%02x %02x %02x (%u)\n", label,
	    (unsigned)data[3], (unsigned)data[4], (unsigned)data[5], temperature,
	    (unsigned)data[0], (unsigned)data[1], (unsigned)data[2], pressure);
}

static int diagnose(int forced) {
	struct bmp280_snapshot snapshot;
	unsigned int mode, sample, modes, samples, expected;
	int ret, measurement_ret;

	printf("Read order: temperature -> pressure (separate transactions).\n");
	modes = forced ? 6 : 1;
	samples = forced ? 3 : 5;
	if (!forced) {
		printf("Expected ID=58, CTRL_MEAS=27 (T x1, P x1, normal).\n");
	}
	for (mode = 0; mode < modes; mode++) {
		expected = forced ? (0x20U | (mode << 2)) : 0x27U;
		if (forced) {
			if (!mode) {
				printf("Pressure OFF: expect P=80 00 00 (524288).\n");
			}
			else {
				printf("Pressure x%u, temperature x1.\n", 1U << (mode - 1));
			}
			printf("Expected CTRL_MEAS=%02x, measuring=0 after completion.\n",
			    expected);
		}
		for (sample = 0; sample < samples; sample++) {
			measurement_ret = forced ? bmp280_force_measurement(mode) : 0;
			if (measurement_ret < 0) {
				printf("Forced conversion failed: %d\n", measurement_ret);
			}
			ret = bmp280_read_snapshot(&snapshot);
			if (ret < 0) {
				printf("Register read failed: %d\n", ret);
				return ret;
			}
			printf("Sample %u: ID=%02x CTRL_MEAS=%02x STATUS=%02x "
			       "CONFIG=%02x\n",
			    sample + 1, (unsigned)snapshot.id, (unsigned)snapshot.ctrl_meas,
			    (unsigned)snapshot.status, (unsigned)snapshot.config);
			print_raw("3-byte", snapshot.burst);
			print_raw("single", snapshot.single);
			if (measurement_ret < 0) {
				return measurement_ret;
			}
			if (snapshot.ctrl_meas != expected) {
				printf("WARNING: CTRL_MEAS differs from expected %02x.\n",
				    expected);
			}
			if (sample + 1 < samples) {
				usleep(250000);
			}
		}
	}
	if (!forced) {
		printf("Single reads may span different conversions in normal mode.\n");
	}
	return 0;
}

int main(int argc, char **argv) {
	int32_t temp, press;
	int ret;

	if (argc > 2 || (argc == 2 && strcmp(argv[1], "-d") && strcmp(argv[1], "-f"))) {
		printf("Usage: bmp280 [-d|-f]\n");
		return -1;
	}

	ret = bmp280_init();
	if (ret < 0) {
		printf("Failed to init bmp280: %d\n", ret);
		return ret;
	}
	if (argc == 2) {
		return diagnose(!strcmp(argv[1], "-f"));
	}
	ret = bmp280_read_values(&temp, &press);
	if (ret < 0) {
		printf("Failed to read bmp280: %d\n", ret);
		return ret;
	}

	printf("Temperature: %s%d.%02d C\n", temp < 0 ? "-" : "",
	    (int)(temp < 0 ? -temp : temp) / 100,
	    (int)(temp < 0 ? -temp : temp) % 100);
	printf("Pressure: %d Pa\n", (int)press);
	return 0;
}

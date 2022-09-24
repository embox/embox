#include <assert.h>
#include <math.h>
#include <stdint.h>

int __signbit_float(float x) {
	uint8_t *bytes = (uint8_t *) (&x);
	uint8_t last_byte = bytes[sizeof(x) - 1];

	if (last_byte & (1 << 7)) {
		return 1;
	} else {
		return 0;
	}
}

int __signbit_double(double x) {
	uint8_t *bytes = (uint8_t *) (&x);
	uint8_t last_byte = bytes[sizeof(x) - 1];

	if (last_byte & (1 << 7)) {
		return 1;
	} else {
		return 0;
	}
}

int __signbit_long_double(long double x) {
	uint8_t *bytes = (uint8_t *) (&x);
	uint8_t last_byte = bytes[sizeof(x) - 1];

	if (last_byte & (1 << 7)) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * @file
 * @brief
 *
 * @date 22.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <lib/b64.h>
#include <stddef.h>

union b64_unit {
	struct {
		unsigned char c1,
			c2,
			c3;
	} c;
	struct {
		unsigned char b1:6,
			b2:6,
			b3:6,
			b4:6;
	} b;
};


static const char b2c[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

static const char c2b[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

#define B2C(b, out) \
	out = b2c[b]
#define C2B(c, out) \
	do { \
		char b = c2b[(unsigned char)c]; \
		if (b == -1) { \
			return -EINVAL; \
		} \
		out = (unsigned char)b; \
	} while (0)

size_t b64_coded_len(const char *plain, size_t plain_sz) {
	return ((plain_sz + 2) / 3) * 4;
}

size_t b64_plain_len(const char *coded, size_t coded_sz) {
	return (coded_sz / 4) * 3;
}

int b64_encode(const char *plain, size_t plain_sz,
		char *buff, size_t buff_sz, size_t *out_coded_sz) {
	size_t i;
	char *out;
	union b64_unit b64u;

	if ((plain == NULL) || (buff == NULL)
			|| (out_coded_sz == NULL)) {
		return -EINVAL;
	}

	if (b64_coded_len(plain, plain_sz) > buff_sz) {
		return -ENOMEM;
	}

	out = buff;

	for (i = 0; i + 3 <= plain_sz; i += 3) {
		b64u.c.c1 = *plain++;
		b64u.c.c2 = *plain++;
		b64u.c.c3 = *plain++;

		B2C(b64u.b.b1, *out++);
		B2C(b64u.b.b2, *out++);
		B2C(b64u.b.b3, *out++);
		B2C(b64u.b.b4, *out++);
	}

	if (i + 2 == plain_sz) {
		b64u.c.c1 = *plain++;
		b64u.c.c2 = *plain++;
		b64u.c.c3 = 0;

		B2C(b64u.b.b1, *out++);
		B2C(b64u.b.b2, *out++);
		B2C(b64u.b.b3, *out++);
		*out++ = '=';
	}
	else if (i + 1 == plain_sz) {
		b64u.c.c1 = *plain++;
		b64u.c.c2 = 0;

		B2C(b64u.b.b1, *out++);
		B2C(b64u.b.b2, *out++);
		*out++ = '=';
		*out++ = '=';
	}

	*out_coded_sz = out - buff;

	return 0;
}

int b64_decode(const char *coded, size_t coded_sz,
		char *buff, size_t buff_sz, size_t *out_plain_sz) {
	size_t i;
	char *out;
	union b64_unit b64u;

	if ((coded == NULL) || (buff == NULL) || (coded_sz % 4 != 0)
			|| (out_plain_sz == NULL)) {
		return -EINVAL;
	}

	if (coded_sz == 0) {
		*out_plain_sz = 0;
		return 0;
	}

	if (b64_plain_len(coded, coded_sz) > buff_sz) {
		return -ENOMEM;
	}

	out = buff;

	for (i = 0; i + 4 < coded_sz; i += 4) {
		C2B(*coded++, b64u.b.b1);
		C2B(*coded++, b64u.b.b2);
		C2B(*coded++, b64u.b.b3);
		C2B(*coded++, b64u.b.b4);

		*out++ = b64u.c.c1;
		*out++ = b64u.c.c2;
		*out++ = b64u.c.c3;
	}

	C2B(*coded++, b64u.b.b1);
	C2B(*coded++, b64u.b.b2);
	*out++ = b64u.c.c1;
	if (*coded != '=') {
		C2B(*coded++, b64u.b.b3);
		*out++ = b64u.c.c2;
		if (*coded != '=') {
			C2B(*coded++, b64u.b.b4);
			*out++ = b64u.c.c3;
		}
	}

	*out_plain_sz = out - buff;

	return 0;
}

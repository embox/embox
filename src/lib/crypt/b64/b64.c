/**
 * @file
 * @brief
 *
 * @date 22.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stddef.h>

#include <lib/crypt/b64.h>


struct b64_char {
	unsigned char c1,
		c2,
		c3;
};

struct b64_b {
	unsigned char b1:6,
		b2:6,
		b3:6,
		b4:6;
};

static const char b2char[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

static const char char2b[256] = {
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

#define B_TO_CHAR(b, out) \
	out = b2char[(b)]

#define CHAR_TO_B(ch, out) \
	do { \
		char b = char2b[(unsigned char)(ch)]; \
		if (b == -1) { \
			return -EINVAL; \
		} \
		out = (unsigned char)b; \
	} while (0)

#define MAKE_B1(b64c) \
	(b64c.c1 >> 2)
#define MAKE_B2(b64c) \
	(((b64c.c1 & 0x03) << 4) | (b64c.c2 >> 4))
#define MAKE_B3(b64c) \
	(((b64c.c2 & 0x0F) << 2) | (b64c.c3 >> 6))
#define MAKE_B4(b64c) \
	(b64c.c3 & 0x3F)

#define MAKE_CHAR1(b64b) \
	((b64b.b1 << 2) | (b64b.b2 >> 4))
#define MAKE_CHAR2(b64b) \
	(((b64b.b2 & 0x0F) << 4) | (b64b.b3 >> 2))
#define MAKE_CHAR3(b64b) \
	(((b64b.b3 & 0x03) << 6) | b64b.b4)

size_t b64_coded_len(const char *plain, size_t plain_sz) {
	return ((plain_sz + 2) / 3) * 4;
}

size_t b64_plain_len(const char *coded, size_t coded_sz) {
	size_t size = (coded_sz / 4) * 3;
	if (*(coded + coded_sz - 1) == '=') size -= 1;
	if (*(coded + coded_sz - 2) == '=') size -= 1;
	return size;
}

int b64_encode(const char *plain, size_t plain_sz,
		char *buff, size_t buff_sz, size_t *out_coded_sz) {
	size_t i;
	char *out;
	struct b64_char b64c;

	if ((plain == NULL) || (buff == NULL)
			|| (out_coded_sz == NULL)) {
		return -EINVAL;
	}

	if (b64_coded_len(plain, plain_sz) > buff_sz) {
		return -ENOMEM;
	}

	out = buff;

	for (i = 2; i < plain_sz; i += 3) {
		b64c.c1 = (unsigned char)*plain++;
		b64c.c2 = (unsigned char)*plain++;
		b64c.c3 = (unsigned char)*plain++;

		B_TO_CHAR(MAKE_B1(b64c), *out++);
		B_TO_CHAR(MAKE_B2(b64c), *out++);
		B_TO_CHAR(MAKE_B3(b64c), *out++);
		B_TO_CHAR(MAKE_B4(b64c), *out++);
	}

	if (i == plain_sz) {
		b64c.c1 = (unsigned char)*plain++;
		b64c.c2 = (unsigned char)*plain++;
		b64c.c3 = 0;

		B_TO_CHAR(MAKE_B1(b64c), *out++);
		B_TO_CHAR(MAKE_B2(b64c), *out++);
		B_TO_CHAR(MAKE_B3(b64c), *out++);
		*out++ = '=';
	}
	else if (i - 1 == plain_sz) {
		b64c.c1 = (unsigned char)*plain++;
		b64c.c2 = 0;

		B_TO_CHAR(MAKE_B1(b64c), *out++);
		B_TO_CHAR(MAKE_B2(b64c), *out++);
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
	struct b64_b b64b;

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

	for (i = 7; i < coded_sz; i += 4) {
		CHAR_TO_B(*coded++, b64b.b1);
		CHAR_TO_B(*coded++, b64b.b2);
		CHAR_TO_B(*coded++, b64b.b3);
		CHAR_TO_B(*coded++, b64b.b4);

		*out++ = MAKE_CHAR1(b64b);
		*out++ = MAKE_CHAR2(b64b);
		*out++ = MAKE_CHAR3(b64b);
	}

	CHAR_TO_B(*coded++, b64b.b1);
	CHAR_TO_B(*coded++, b64b.b2);
	*out++ = MAKE_CHAR1(b64b);
	if (*coded != '=') {
		CHAR_TO_B(*coded++, b64b.b3);
		*out++ = MAKE_CHAR2(b64b);
		if (*coded != '=') {
			CHAR_TO_B(*coded++, b64b.b4);
			*out++ = MAKE_CHAR3(b64b);
		}
	}

	*out_plain_sz = out - buff;

	return 0;
}

/**
 * @file
 * @brief
 *
 * @date 18.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/array.h>

const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

in_addr_t inet_addr(const char *cp) {
	struct in_addr in;

	if ((cp == NULL) || (0 == inet_aton(cp, &in))) {
		return (in_addr_t)-1;
	}

	return in.s_addr;
}

char * inet_ntoa(struct in_addr in) {
	int res;
	static char buff[INET_ADDRSTRLEN];
	uint8_t *b;

	b = (uint8_t *)&in;
	res = snprintf(buff, ARRAY_SIZE(buff), "%hhu.%hhu.%hhu.%hhu",
			b[0], b[1], b[2], b[3]);
	assert(res >= 0);

	return &buff[0];
}

int inet_aton(const char *cp, struct in_addr *addr) {
	static const in_addr_t max[4] = { 0xffffffff, 0xffffff, 0xffff, 0xff };
	in_addr_t val;
	uint32_t temp;
	char c;
	union iaddr {
	  uint8_t bytes[4];
	  uint32_t word;
	} res;
	uint8_t *pp = res.bytes;
	int digit;
	int base;
	int dots;

	res.word = 0;
	dots = 0; /* there should be 3 dots in character ip string notation */

	c = *cp;
	for (;;) {
		/*
		 * Collect number up to ``.''.
		 * Values are specified as for C:
		 * 0x=hex, 0=octal, isdigit=decimal.
		 */
		if (!isdigit(c))
			return 0;

		val = 0; base = 10; digit = 0;
		if (c == '0') {
			c = *++cp;
			if (c == 'x' || c == 'X')
				base = 16, c = *++cp;
			else {
				base = 8;
				digit = 1 ;
			}
		}
		for (;;) {
			if (isdigit(c)) {
				if (base == 8 && (c == '8' || c == '9'))
					return 0;
				val = (val * base) + (c - '0');
				c = *++cp;
				digit = 1;
			} else if (base == 16 && isxdigit(c)) {
			    temp = (islower(c) ? 'a' : 'A');
				val = (val << 4) |
					(c + 10 - temp /*(islower(c) ? 'a' : 'A')*/);
				c = *++cp;
				digit = 1;
			} else {
				break;
			}
		}

		if (c == '.') {
			/*
			 * Internet format:
			 *	a.b.c.d
			 *	a.b.c	(with c treated as 16 bits)
			 *	a.b	(with b treated as 24 bits)
			 */
			if (pp > res.bytes + 2 || val > 0xff) {
				return 0;
			}
			*pp++ = val;
			c = *++cp;
			dots++;  									/* calculate how much dots do we have */
		} else {
			break;
		}
	}
	/* Check for trailing characters. */
	if (c != '\0' && !isspace(c)) {
		return 0;
	}
	/* It has a valid format? */
	if (dots == 0) {
		return 0;
	}
	/* Did we get a valid digit? */
	if (!digit) {
		return 0;
	}
	/* Check whether the last part is in its limits depending on
	   the number of parts in total.  */
	if (val > max[pp - res.bytes]) {
		return 0;
	}

	/* everything went fine */
	if (addr != NULL) {
		addr->s_addr = res.word | htonl (val);
	}
	return 1;
}
#if 0
static int inet_ntop4(const struct in_addr *in,
		char *buff, socklen_t size,

const char * inet_ntop(int af, const void *src, char *dst,
		socklen_t size) {
	int ret;
	const char *result;

	switch (af) {
	default:
		ret = -EAFNOSUPPORT;
	case AF_INET:
		ret = inet_ntop4();
	case AF_INET6:
		ret = inet_ntop6();
	}

	if (ret != 0) {
		SET_ERRNO(-ret);
		return NULL;
	}

	assert(result != NULL);
	return result;
}
#endif

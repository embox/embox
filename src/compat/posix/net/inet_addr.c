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
#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/array.h>

const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
const struct in6_addr in6addr_loopback = IN6ADDR_LOOPBACK_INIT;

static int inet_to_str(const struct in_addr *in, char *buff,
		socklen_t buff_sz) {
	int ret;

	assert(in != NULL);
	assert(buff != NULL);

	ret = snprintf(buff, buff_sz, "%hhu.%hhu.%hhu.%hhu",
			in->s_addr8[0], in->s_addr8[1], in->s_addr8[2],
			in->s_addr8[3]);
	if (ret < 0) {
		return -EIO;
	}
	else if (ret >= buff_sz) {
		return -ENOSPC;
	}

	return 0;
}

static int inet6_to_str(const struct in6_addr *in6, char *buff,
		socklen_t buff_sz) {
	int ret;
	size_t i, zs_ind, zs_len, zs_max_ind, zs_max_len;

	assert(in6 != NULL);
	assert(buff != NULL);

	zs_ind = 0; /* XXX required for -O2 */

	zs_max_ind = 0;
	zs_len = zs_max_len = 0;
	for (i = 0; i < ARRAY_SIZE(in6->s6_addr16); ++i) {
		if (in6->s6_addr16[i] == 0) {
			if (zs_len == 0) {
				zs_ind = i;
			}
			++zs_len;
		}
		else if (zs_len != 0) {
			if (zs_len > zs_max_len) {
				zs_max_len = zs_len;
				zs_max_ind = zs_ind;
			}
			zs_len = 0;
		}
	}

	if ((i == ARRAY_SIZE(in6->s6_addr16)) && (zs_len > zs_max_len)) {
		zs_max_len = zs_len;
		zs_max_ind = zs_ind;
	}

	for (i = 0; i < zs_max_ind; ++i) {
		ret = snprintf(buff, buff_sz, "%hx:", ntohs(in6->s6_addr16[i]));
		if (ret < 0) {
			return -EIO;
		}
		else if (ret >= buff_sz) {
			return -ENOSPC;
		}
		buff += ret;
		buff_sz -= ret;
	}

	ret = zs_max_len <= 1 ? snprintf(buff, buff_sz, "%hx", ntohs(in6->s6_addr16[i]))
			: i + zs_max_len == ARRAY_SIZE(in6->s6_addr16) ? i == 0
				? snprintf(buff, buff_sz, "::")
				: snprintf(buff, buff_sz, ":")
			: zs_max_ind == 0 ? snprintf(buff, buff_sz, ":")
			: 0;
	if (ret < 0) {
		return -EIO;
	}
	else if (ret >= buff_sz) {
		return -ENOSPC;
	}
	buff += ret;
	buff_sz -= ret;
	i += zs_max_len <= 1 ? 1 : zs_max_len;

	for (; i < ARRAY_SIZE(in6->s6_addr16); ++i) {
		ret = snprintf(buff, buff_sz, ":%hx", ntohs(in6->s6_addr16[i]));
		if (ret < 0) {
			return -EIO;
		}
		else if (ret >= buff_sz) {
			return -ENOSPC;
		}
		buff += ret;
		buff_sz -= ret;
	}

	return 0;
}

static int str_to_inet(const char *buff, struct in_addr *in) {
	size_t i;
	unsigned long addr, val, max_val;

	assert(buff != NULL);
	assert(in != NULL);

	addr = 0UL;
	max_val = ULONG_MAX;

	for (i = 0; i < ARRAY_SIZE(in->s_addr8); ++i, ++buff,
			max_val >>= CHAR_BIT) {
		SET_ERRNO(0);
		val = strtoul(buff, (char **)&buff, 0);
		if (errno != 0) {
			return 1; /* error: see errno */
		}
		if (val > max_val) {
			return 1; /* error: invalid address format */
		}
		addr |= *buff == '.'
				? val << (CHAR_BIT * (ARRAY_SIZE(in->s_addr8) - i - 1))
				: val;
		if (*buff != '.') {
			break;
		}
		else if (val > UCHAR_MAX) {
			return 1; /* error: invalid address format */
		}
	}

	if (i == ARRAY_SIZE(in->s_addr8)) {
		return 1; /* error: invalid address format */
	}

	in->s_addr = htonl(addr);

	return 0;
}

static int str_to_inet6(const char *buff, struct in6_addr *in6) {
	size_t i, zs_ind;
	unsigned long val;

	assert(buff != NULL);
	assert(in6 != NULL);

	zs_ind = ARRAY_SIZE(in6->s6_addr16);
	memset(in6, 0, sizeof *in6);

	for (i = 0; i < ARRAY_SIZE(in6->s6_addr16); ++i, ++buff) {
		if (*buff == ':') {
			if (zs_ind != ARRAY_SIZE(in6->s6_addr16)) {
				return 1; /* error: invalid address format */
			}
			if ((i == 0) && (*++buff != ':')) {
				return 1; /* error: invalid address format */
			}
			zs_ind = i;
			++buff;
			if (*buff == '\0') {
				break; /* ...:: */
			}
		}
		SET_ERRNO(0);
		val = strtoul(buff, (char **)&buff, 16);
		if (errno != 0) {
			return 1; /* error: see errno */
		}
		if (val > USHRT_MAX) {
			return 1; /* error: invalid address format */
		}
		in6->s6_addr16[i] = htons(val);
		if (*buff != ':') {
			break;
		}
	}

	if ((zs_ind == ARRAY_SIZE(in6->s6_addr16))
			&& (i != ARRAY_SIZE(in6->s6_addr16))) {
		return 1; /* error: invalid address format */
	}

	memmove(&in6->s6_addr16[ARRAY_SIZE(in6->s6_addr16) - (i - zs_ind + 1)],
			&in6->s6_addr16[zs_ind],
			(i - zs_ind + 1) * sizeof in6->s6_addr16[0]);
	memset(&in6->s6_addr16[zs_ind], 0,
			(ARRAY_SIZE(in6->s6_addr16) - i - 1) * sizeof in6->s6_addr16[0]);

	return 0;
}

char * inet_ntoa(struct in_addr in) {
	static char buff[INET_ADDRSTRLEN];
	return 0 == inet_to_str(&in, &buff[0], ARRAY_SIZE(buff))
			? &buff[0] : NULL;
}

const char * inet_ntop(int af, const void *addr, char *buff,
		socklen_t buff_sz) {
	int ret;

	switch (af) {
	default:
		ret = -EAFNOSUPPORT;
		break;
	case AF_INET:
		ret = inet_to_str((const struct in_addr *)addr, buff,
				buff_sz);
		break;
	case AF_INET6:
		ret = inet6_to_str((const struct in6_addr *)addr, buff,
				buff_sz);
		break;
	}

	if (ret != 0) {
		SET_ERRNO(-ret);
		return NULL;
	}

	return buff;
}

in_addr_t inet_addr(const char *cp) {
	struct in_addr in;
	return 0 == str_to_inet(cp, &in) ? in.s_addr : (in_addr_t)-1;
}

int inet_aton(const char *cp, struct in_addr *addr) {
	return 0 == str_to_inet(cp, addr) ? 1 : 0;
}

int inet_pton(int af, const char *buff, void *addr) {
	int ret;

	switch (af) {
	default:
		ret = -EAFNOSUPPORT;
		break;
	case AF_INET:
		ret = str_to_inet(buff, (struct in_addr *)addr);
		break;
	case AF_INET6:
		ret = str_to_inet6(buff, (struct in6_addr *)addr);
		break;
	}

	if (ret < 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return ret == 0 ? 1 : 0;
}

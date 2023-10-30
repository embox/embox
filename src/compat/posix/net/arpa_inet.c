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
#include <inttypes.h>
#include <limits.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <util/array.h>

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
		ret = snprintf(buff, buff_sz, "%" PRIu16 ":", ntohs(in6->s6_addr16[i]));
		if (ret < 0) {
			return -EIO;
		}
		else if (ret >= buff_sz) {
			return -ENOSPC;
		}
		buff += ret;
		buff_sz -= ret;
	}

	ret = zs_max_len <= 1 ? snprintf(buff, buff_sz, "%" PRIu16, ntohs(in6->s6_addr16[i]))
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
		ret = snprintf(buff, buff_sz, ":%" PRIu16, ntohs(in6->s6_addr16[i]));
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

/* convert strinf of dots-and-decimals ip v4 address to binary network order 
 * return 0 - success, 1 - failure*/
static int str_to_inet(const char *str, struct in_addr *in) {
	size_t i;
	unsigned long addr, val;

    const char *buff = str;

    assert(buff != NULL);
    assert(in != NULL);

	addr = 0UL;

    for (i = 0; i < sizeof(struct in_addr); i++) {
        /* if we came across EOL early, out*/
        if(!*buff)
            return 1;
        /* first symbol not a digit - we're out*/
        if(!isdigit(buff[0]))
            return 1;
        /* somewhy padding with 0 in linux isn't accepted, we'll stick to that*/
        if((isdigit(buff[1])) && buff[0] == '0')
            return 1;
        SET_ERRNO(0);
        val = strtoul(buff, (char **)&buff, 10);
        if((errno) || (val>UINT8_MAX))
            return 1; /*we're out if stroul had an error or val > 8 bit max*/
        /* after stroul buff points to first non digit
         * and it should be either a '.' or EOL
         * if not we're out*/
        if((*buff != '.') && (*buff))
            return 1;

        /* build up address in 32 bits */
        addr |= val << (CHAR_BIT * (sizeof(in->s_addr) - i - 1));
        /* hop over '.' but if we're at the end of line
         * stay. If EOL is too early we'll catch with the first condition ^
         * if */
        if (*buff)
            buff++;
    }
    /* if *buff !=0 then the address string contains smth else
     * which is a format error*/
    if(*buff)
        return 1;
    /* catch a trailing '.', that ruins everything =) */
    if(!isdigit(*(--buff)))
        return 1;

	in->s_addr = htonl(addr);
	return 0;
}

/* convert string representation of ip v6 address to network order binary*/
#define COLON_NONE 16 /* value in denote colon hasn't been found yet*/
#define DBCL_NONE 8   /* value to denote double colon hasn't been found yet*/
#define N_16_WORDS 8
static int str_to_inet6(const char *str, struct in6_addr *in6){
    uint8_t i;
    uint8_t i_num = 0; /* curently being parsed numeral index */
    uint8_t double_colon_at = DBCL_NONE; /*position of double colon in 16b words*/
    uint8_t prev_colon = COLON_NONE; /*position of the last colon we had in chars*/
    uint8_t colons = 0; /* number of colons in the string address*/
    uint8_t dots = 0; /* is string address formated with dots */
    uint16_t tmp_buf[8] = {0};
    unsigned long val = 0;

    char *endptr; /*for stroul */

    assert(str != NULL);
    assert(in6 != NULL);

    for(i=0; str[i]!=0; i++){
        switch (str[i]) {
            case ':':
                prev_colon = i; /*we'll need it to correctly parse dots-part*/
                colons++; /*keep counting to check format in dots-part*/
                if(i == 0)
                    break; /*skip further checks at the begining*/
                /* check if we have double-colon and save position */
                if(str[i-1] == ':'){
                    if(double_colon_at == DBCL_NONE){
                        double_colon_at = i_num;
                        break;
                    }
                    /* in case we already have double colon - fmt error*/
                    return 1;
                }
                break;
            case '.': 
                if(colons>6) /* if we already had more than 6 colons - fmt error*/
                    return 1;
                if((double_colon_at == DBCL_NONE) && (colons != 6))
                    return 1; /*If we're at the dot,  and there were no double
                                colons, then we should have come across exactly
                                6 colons before - otherwise fmt error*/
                if(prev_colon != COLON_NONE){
                    /* we have to return to previous numeral and reinterpret it
                     * as the begining of dotted part*/
                    i_num--;
                    i = prev_colon+1;
                    if(!isdigit(str[i])) /*we're axpecting a decimal here */
                        return 1;        /* otherwise fmt error*/
                    /* feed the rest to the v4 str_to_inet function*/
                    if(str_to_inet(&str[i], (struct in_addr *)&tmp_buf[i_num]))
                        return 1; /* if it didn't succed - we're out*/
                    i_num += 2;  /*finished with the last 2 16b words */
                    for(;str[i+1] != 0;) /* come out of the whole cycle-we're done*/
                        i++;
                    dots++;
                    break;
                } 
                return 1; /* there were no colons before dots - fmt error*/
            default:  
                /* we're looking for a hex digit, if it isn't we're out - fmt error*/
                if(!isxdigit(str[i]))
                    return 1;
                SET_ERRNO(0);
                val = strtoul(&str[i], &endptr,16);
                if((errno) || (val>USHRT_MAX))
                    return 1; /*we're out if stroul had an error or val > 16 bit max*/
                tmp_buf[i_num++] = htons(val);
                i += endptr - &str[i] - 1;  /*skip to the next delimiter*/
        } 
    }

    /* fill out output structure */
    if(double_colon_at == DBCL_NONE){ /* if there was no double colon - simple case*/
        if((colons == 7) || ((colons == 6) && dots)){
            memcpy(in6, tmp_buf, sizeof(struct in6_addr));
            return 0;
        } else 
            return 1; /* If there were no double colon, then it should have
                         either 7 colons and not dots-part or 6 colons 
                         and dots-part. Otherwise - fmt error*/
    }
    /* if there was double colon notation */
    int k=0, l=0;
    int zeros = N_16_WORDS - i_num;
    for (; k<double_colon_at; l++, k++){
        in6->s6_addr16[k] = tmp_buf[l];
    }
    for (; k<double_colon_at+zeros; k++){
        in6->s6_addr16[k] = 0;
    }
    for (; k<N_16_WORDS; k++, l++){
        in6->s6_addr16[k] = tmp_buf[l];
    }
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

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host) {
	struct in_addr in;

	if (net < 128) {
		in.s_addr = (net << IN_CLASSA_NSHIFT) | (host & IN_CLASSA_HOST);
	}
	else if (net < 65536) {
		in.s_addr = (net << IN_CLASSB_NSHIFT) | (host & IN_CLASSB_HOST);
	}
	else if (net < 16777216L) {
		in.s_addr = (net << IN_CLASSC_NSHIFT) | (host & IN_CLASSC_HOST);
	}
	else {
		in.s_addr = net | host;
	}

	in.s_addr = htonl(in.s_addr);

	return in;
}

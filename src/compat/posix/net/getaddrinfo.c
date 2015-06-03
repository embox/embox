/**
 * @file
 * @brief
 *
 * @date 29.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <net/util/servent.h>

#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#define ADDRINFO_TUPLE_POOL_SZ OPTION_GET(NUMBER, addrinfo_pool_size)

struct addrinfo_tuple {
	struct addrinfo ai;
	struct sockaddr_storage __ai_ai_addr_storage;
};

POOL_DEF(addrinfo_tuple_pool, struct addrinfo_tuple, ADDRINFO_TUPLE_POOL_SZ);

static int explore_hints(const struct addrinfo *hints,
		struct addrinfo *out_hints) {
	static const struct addrinfo hints_default = {
		.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG,
		.ai_family = AF_UNSPEC,
		.ai_socktype = 0,
		.ai_protocol = 0
	};

	assert(out_hints != NULL);

	memcpy(out_hints, hints != NULL ? hints : &hints_default,
			sizeof *out_hints);

	switch (out_hints->ai_family) {
	default:
		return EAI_FAMILY;
	case AF_UNSPEC:
	case AF_INET:
	case AF_INET6:
		break;
	}

	switch (out_hints->ai_socktype) {
	default:
		return EAI_SOCKTYPE;
	case 0:
		switch (out_hints->ai_protocol) {
		default:
			return EAI_SERVICE;
		case 0:
			break;
		case IPPROTO_TCP:
			out_hints->ai_socktype = SOCK_STREAM;
			break;
		case IPPROTO_UDP:
			out_hints->ai_socktype = SOCK_DGRAM;
			break;
		}
		break;
	case SOCK_STREAM:
		switch (out_hints->ai_protocol) {
		default:
			return EAI_SOCKTYPE;
		case 0:
			out_hints->ai_protocol = IPPROTO_TCP;
			break;
		case IPPROTO_TCP:
			break;
		}
		break;
	case SOCK_DGRAM:
		switch (out_hints->ai_protocol) {
		default:
			return EAI_SOCKTYPE;
		case 0:
			out_hints->ai_protocol = IPPROTO_UDP;
			break;
		case IPPROTO_UDP:
			break;
		}
		break;
	}

	return 0;
}

static struct servent * explore_serv(const char *servname,
		const struct addrinfo *hints) {
	const char *proto;
	unsigned short port;
	struct protoent *pe;
	struct servent *result;

	assert(hints != NULL);

	result = NULL;

	if (hints->ai_protocol != 0) {
		pe = getprotobynumber(hints->ai_protocol);
#if 0
		if (pe == NULL) {
			return NULL; /* error: bad protocol */
		}
#else
		proto = pe != NULL ? pe->p_name : NULL;
#endif
	}
	else {
		proto = NULL;
	}

	if (servname == NULL) {
		return servent_make(NULL, 0, proto);
	}

	if (!(hints->ai_flags & AI_NUMERICSERV)) {
		result = getservbyname(servname, proto);
	}

	if (NULL == result) {
		if (1 == sscanf(servname, "%hu", &port)) {
			result = servent_make(NULL, port, proto);
		}
		/* error: bad port (FIXME EAI_NONAME) */
	}

	return result;
}

static int ai_make(int family, int socktype, int protocol,
		socklen_t addrlen, struct sockaddr *addr,
		struct addrinfo **ai_list) {
	struct addrinfo_tuple *ait;

	assert(addr != NULL);
	assert(ai_list != NULL);

	ait = pool_alloc(&addrinfo_tuple_pool);
	if (ait == NULL) {
		SET_ERRNO(ENOMEM);
		return EAI_SYSTEM;
	}

	ait->ai.ai_flags = 0;
	ait->ai.ai_family = family;
	ait->ai.ai_socktype = socktype;
	ait->ai.ai_protocol = protocol;
	ait->ai.ai_addrlen = addrlen;
	ait->ai.ai_addr = (struct sockaddr *)&ait->__ai_ai_addr_storage;
	memcpy(ait->ai.ai_addr, addr, addrlen);
	ait->ai.ai_canonname = NULL;
	ait->ai.ai_next = NULL;

	*ai_list = &ait->ai;

	return 0;
}

static int explore_node(const char *nodename,
		const struct addrinfo *hints, const struct servent *se,
		struct addrinfo **out_ai) {
	static const int all_family[] = { AF_INET, AF_INET6, -1 },
		all_socktype[] = { SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, -1 },
		all_protocol[] = { IPPROTO_TCP, IPPROTO_UDP, 0 };
	int one_family[2] = { -1, -1 },
		one_socktype[2] = { -1, -1 },
		one_protocol[1] = { -1 };
	const int *family_set, *socktype_set, *protocol_set;
	const int *family, *socktype, *protocol;
	struct addrinfo *ai_head, **ai_last_ptr;
	struct {
		struct sockaddr_in in;
		struct sockaddr_in6 in6;
	} addr;
	struct sockaddr *sa;
	socklen_t salen;
	int ret;
	struct hostent *he;
	char **he_addr;

	assert(hints != NULL);
	assert(se != NULL);
	assert(out_ai != NULL);

	family_set = hints->ai_family == 0 ? all_family
			: (one_family[0] = hints->ai_family, one_family);
	socktype_set = hints->ai_socktype == 0 ? all_socktype
			: (one_socktype[0] = hints->ai_socktype, one_socktype);
	protocol_set = hints->ai_socktype == 0 ? all_protocol
			: (one_protocol[0] = hints->ai_protocol, one_protocol);

	ai_head = NULL;
	ai_last_ptr = &ai_head;

	memset(&addr, 0, sizeof addr);
	addr.in.sin_family = AF_INET;
	addr.in6.sin6_family = AF_INET6;
	addr.in.sin_port = addr.in6.sin6_port = se->s_port;

	if (nodename == NULL) {
		if (hints->ai_flags & AI_PASSIVE) {
			addr.in.sin_addr.s_addr = htonl(INADDR_ANY);
			memcpy(&addr.in6.sin6_addr, &in6addr_any,
					sizeof addr.in6.sin6_addr);
		}
		else {
			addr.in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
			memcpy(&addr.in6.sin6_addr, &in6addr_loopback,
					sizeof addr.in6.sin6_addr);
		}
		for (family = family_set; *family != -1; ++family) {
			for (socktype = socktype_set, protocol = protocol_set;
					*socktype != -1; ++socktype, ++protocol) {
				if (*family == AF_INET) {
					sa = (struct sockaddr *)&addr.in;
					salen = sizeof addr.in;
				}
				else {
					assert(*family == AF_INET6);
					sa = (struct sockaddr *)&addr.in6;
					salen = sizeof addr.in6;
				}
				ret = ai_make(*family, *socktype, *protocol,
						salen, sa, ai_last_ptr);
				if (ret != 0) {
					freeaddrinfo(ai_head);
					return ret;
				}
				ai_last_ptr = &(*ai_last_ptr)->ai_next;
			}
		}
		*out_ai = ai_head;
		return 0;
	}

	if (hints->ai_flags & AI_NUMERICHOST) {
		if (1 == inet_pton(AF_INET, nodename, &addr.in.sin_addr)) {
			sa = (struct sockaddr *)&addr.in;
			salen = sizeof addr.in;
		}
		else if (1 == inet_pton(AF_INET6, nodename,
					&addr.in6.sin6_addr)) {
			sa = (struct sockaddr *)&addr.in6;
			salen = sizeof addr.in6;
		}
		else {
			return EAI_NONAME;
		}
		for (family = family_set; *family != -1; ++family) {
			for (socktype = socktype_set, protocol = protocol_set;
					*socktype != -1; ++socktype, ++protocol) {
				if (*family != sa->sa_family) {
					continue;
				}
				ret = ai_make(*family, *socktype, *protocol,
						salen, sa, ai_last_ptr);
				if (ret != 0) {
					freeaddrinfo(ai_head);
					return ret;
				}
				ai_last_ptr = &(*ai_last_ptr)->ai_next;
			}
		}
		*out_ai = ai_head;
		return 0;
	}

	he = gethostbyname(nodename);
	if (he == NULL) {
		switch (h_errno) {
		default:
		case NO_RECOVERY:
			return EAI_FAIL;
		case HOST_NOT_FOUND:
		case NO_DATA:
			return EAI_NONAME;
		case TRY_AGAIN:
			return EAI_AGAIN;
		}
	}

	for (he_addr = he->h_addr_list; *he_addr != NULL; ++he_addr) {
		for (family = family_set; *family != -1; ++family) {
			for (socktype = socktype_set, protocol = protocol_set;
					*socktype != -1; ++socktype, ++protocol) {
				if (*family != he->h_addrtype) {
					continue;
				}
				if (*family == AF_INET) {
					if (he->h_length != sizeof(addr.in.sin_addr)) {
						continue;
					}
					addr.in.sin_addr = *((struct in_addr *)*he_addr);
					sa = (struct sockaddr *)&addr.in;
					salen = sizeof addr.in;
				}
				else {
					assert(*family == AF_INET6);
					if (he->h_length != sizeof(addr.in6.sin6_addr)) {
						continue;
					}
					addr.in6.sin6_addr = *((struct in6_addr *)*he_addr);
					sa = (struct sockaddr *)&addr.in6;
					salen = sizeof addr.in6;
				}
				ret = ai_make(*family, *socktype, *protocol,
						salen, sa, ai_last_ptr);
				if (ret != 0) {
					freeaddrinfo(ai_head);
					return ret;
				}
				ai_last_ptr = &(*ai_last_ptr)->ai_next;
			}
		}
	}

	*out_ai = ai_head;
	return 0;
}

int getaddrinfo(const char *nodename, const char *servname,
		const struct addrinfo *hints, struct addrinfo **res) {
	int ret;
	struct servent *se;
	struct addrinfo hints_, *res_;

	assert(res != NULL);

	if ((nodename == NULL) && (servname == NULL)) {
		return EAI_NONAME;
	}

	ret = explore_hints(hints, &hints_);
	if (ret != 0) {
		return ret;
	}

	se = explore_serv(servname, &hints_);
	if (se == NULL) {
		return EAI_SERVICE;
	}

	res_ = NULL;
	ret = explore_node(nodename, &hints_, se, &res_);
	if (ret != 0) {
		return ret;
	}

	*res = res_;

	return 0;
}

void freeaddrinfo(struct addrinfo *ai) {
	struct addrinfo *ai_next;
	struct addrinfo_tuple *ait;

	assert(ai != NULL);

	do {
		ait = member_cast_out(ai, struct addrinfo_tuple, ai);
		ai_next = ai->ai_next;
		pool_free(&addrinfo_tuple_pool, ait);
		ai = ai_next;
	} while (ai != NULL);
}

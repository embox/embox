/**
 * @file
 * @brief
 *
 * @date 29.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <string.h>

static const struct addrinfo ai_default = {
	.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG,
	.ai_family = AF_UNSPEC,
	.ai_socktype = 0,
	.ai_protocol = 0
};

static int explore_node(const char *nodename,
		const struct addrinfo *hints,
		struct addrinfo *out_ai) {
	struct hostent *he;

	assert(nodename != NULL);
	assert(hints != NULL);
	assert(out_ai != NULL);

	if (hints->ai_flags & AI_NUMERICHOST) {
		if (inet_pton(AF_INET, nodename, out_ai->ai_addr)) {
			out_ai->ai_family = AF_INET;
			out_ai->ai_addrlen = sizeof(struct sockaddr_in);
		}
		else if (inet_pton(AF_INET6, nodename, out_ai->ai_addr)) {
			out_ai->ai_family = AF_INET6;
			out_ai->ai_addrlen = sizeof(struct sockaddr_in6);
		}
		else {
			return EAI_NONAME;
		}
	}
	else {
		he = gethostbyname(nodename);
		if (he == NULL) {
			return EAI_NONAME;
		}
		/* FIXME */
		out_ai->ai_family = he->h_addrtype;
		memcpy(out_ai->ai_addr, he->h_addr_list[0],
				he->h_length);
		out_ai->ai_addrlen = he->h_length;
	}

	return 0;
}

static int explore_serv(const char *nodename,
		const struct addrinfo *hints,
		struct addrinfo *out_ai) {
	return 0;
}

int getaddrinfo(const char *nodename, const char *servname,
		const struct addrinfo *hints, struct addrinfo **res) {
	int ret;
	static struct addrinfo ai;
	static struct sockaddr_storage ai_ss;

	assert(res != NULL);

	if ((nodename == NULL) && (servname == NULL)) {
		return EAI_NONAME;
	}

	hints = hints != NULL ? hints : &ai_default;

	memset(&ai, 0, sizeof ai);
	ai.ai_addr = (struct sockaddr *)&ai_ss;

	if (nodename != NULL) {
		ret = explore_node(nodename, hints, &ai);
		if (ret != 0) {
			return ret;
		}
	}

	if (servname != NULL) {
		ret = explore_serv(servname, hints, &ai);
		if (ret != 0) {
			return ret;
		}
	}

	*res = &ai;

	return 0;
}

void freeaddrinfo(struct addrinfo *ai) {
}

#if 0
	if (hints != NULL) {
		if (hints->ai_flags & AI_CANONNAME
				|| hints->ai_flags & AI_V4MAPPED
				|| hints->ai_flags & AI_ALL
				|| hints->ai_flags & AI_ADDRCONFIG) {
			return EAI_BADFLAGS;
		}
		switch (hints->ai_family) {
		default:
			return EAI_FAMILY;
		case AF_UNSPEC:
		case AF_INET:
		case AF_INET6:
			break;
		}
		switch (hints->ai_socktype) {
		default:
			return EAI_SOCKTYPE;
		case 0:
		case SOCK_STREAM:
		case SOCK_DGRAM:
			break;
		}
#endif

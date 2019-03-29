#include "mqtt_test.h"

int sock_connect(const char *nodename, const char *servname,
				struct addrinfo *hints) {
    struct addrinfo *result, *res_it;
    int sockfd;
	int r;

	r = getaddrinfo(nodename, servname, hints, &result);
	if (r != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		return (r < 0)? r : -r;
	}

	for (res_it = result; res_it != NULL; res_it = res_it->ai_next) {
		sockfd = socket(res_it->ai_family, res_it->ai_socktype,
						res_it->ai_protocol);
		if (sockfd == -1)
			continue;

		if(hints->ai_flags == AI_PASSIVE){
			if (0 == bind(sockfd, res_it->ai_addr, res_it->ai_addrlen))
				break;		/* Success */
		} else {
			if (0 == connect(sockfd, res_it->ai_addr, res_it->ai_addrlen))
				break;		/* Success */
		}
		
		close(sockfd);
	}

	if (res_it == NULL) {		/* No address succeeded */
		fprintf(stderr, "Could not bind/connect\n");
		return -ENOENT;
	}

	freeaddrinfo(result);

	return sockfd;
}
/**
 * @file
 * @brief Example of using gethostbyaddr function
 *
 * @date 29.03.13
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <framework/example/self.h>
#include <errno.h>

EMBOX_EXAMPLE(exec);

static void print_info(struct hostent *he) {
	char **ptr;

	printf("\nname: %s\n", he->h_name);
	for (ptr = he->h_aliases; *ptr != NULL; ptr++) {
		printf("alternate name: %s\n", *ptr);
	}

	printf("address type: %s\n",
			(he->h_addrtype == AF_INET ? "AF_INET" : "unknown"));

	if (he->h_length != sizeof(struct in_addr)) {
		printf("Invalid length: %d\n", he->h_length);
		return;
	}

	for (ptr = he->h_addr_list; *ptr != NULL; ++ptr) {
		printf("%s address: %s\n",
				(ptr == he->h_addr_list ? "primary" : "alternate"),
				inet_ntoa((*(struct in_addr *)*ptr)));
	}
}

static int exec(int argc, char *argv[]) {
	struct hostent *he;
	struct in_addr in;

	if (argc != 2) {
		printf("Usage: %s <address>\n", argv[0]);
		return 0;
	}

	if (!inet_aton(argv[1], &in)) {
		return -EINVAL;
	}

	he = gethostbyaddr(&in, sizeof in, AF_INET);
	if (he == NULL) {
		printf("%s: unknown host %s\n", argv[0], argv[1]);
		return -1;
	}

	print_info(he);

	return 0;
}

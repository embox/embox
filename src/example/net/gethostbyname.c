/**
 * @file
 * @brief Example of using gethostbyname function
 *
 * @date 21.08.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <net/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <framework/example/self.h>

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

	if (argc != 2) {
		printf("Usage: %s <hostname>\n", argv[0]);
		return 0;
	}

	he = gethostbyname(argv[1]);
	if (he == NULL) {
		printf("%s: unknown host %s\n", argv[0], argv[1]);
		return -1;
	}

	print_info(he);

	return 0;
}

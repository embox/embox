/**
 * @file
 * @brief Example of using gethostbyname and gethostbyaddr functions
 *
 * @date 21.08.12
 * @author Ilia Vaprol
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <framework/example/self.h>
#include <unistd.h>
#include <errno.h>

EMBOX_EXAMPLE(exec);

static void print_info(struct hostent *he) {
	char **ptr;

	/**
	 * Host name
	 */
	printf("\nname: %s\n", he->h_name);

	/**
	 * Host aliases
	 */
	for (ptr = he->h_aliases; *ptr != NULL; ptr++) {
		printf("alternate name: %s\n", *ptr);
	}

	/**
	 * Address type
	 */
	printf("address type: %s\n",
			(he->h_addrtype == AF_INET ? "AF_INET" : "unknown"));

	/**
	 * Address length
	 */
	if (he->h_length != sizeof(struct in_addr)) {
		printf("Invalid length: %d\n", he->h_length);
		return;
	}

	/**
	 * Addresses
	 */
	for (ptr = he->h_addr_list; *ptr != NULL; ++ptr) {
		printf("%s address: %s\n",
				(ptr == he->h_addr_list ? "primary" : "alternate"),
				inet_ntoa((*(struct in_addr *)*ptr)));
	}
}

static void print_usage(void) {
	printf("Usage: hostinfo [<host name> | -a <host address>]\n");
}

static int exec(int argc, char *argv[]) {
	int opt, flag_a;
	struct hostent *he;
	struct in_addr in;

	/**
	 * Initialization
	 */
	flag_a = 0;
	getopt_init();

	/**
	 * Parse command arguments
	 */
	while (-1 != (opt = getopt(argc, argv, "ha"))) {
		switch (opt) {
		case 'a':
			flag_a = 1;
			break;
		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	/**
	 * Get host name or host address
	 */
	if (optind >= argc) {
		printf("hostinfo: error: host not specified\n");
		return -EINVAL;
	}

	/**
	 * The last argument is the name or address?
	 */
	if (flag_a) {
		/**
		 * Get address of remote host which we want to discover
		 */
		if (!inet_aton(argv[optind], &in)) {
			return -EINVAL;
		}

		/**
		 * Try to get host information. As you can see,
		 * 2nd and 3th arguments specify protocol family
		 */
		he = gethostbyaddr(&in, sizeof in, AF_INET);
	}
	else {
		/**
		 * Nope, it's the name.
		 * Discover host by its name
		 */
		he = gethostbyname(argv[optind]);
	}

	/**
	 * Discovery has been successfully completed?
	 */
	if (he == NULL) {
		printf("hostinfo: error: unknown host %s\n", argv[optind]);
		return -1;
	}

	/**
	 * Print host information
	 */
	print_info(he);

	return 0;
}

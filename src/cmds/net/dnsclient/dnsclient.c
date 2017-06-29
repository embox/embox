/**
 * @file
 *
 * @date 10.01.2016
 * @author Anton Bondarev
 */
#include <stdio.h>
#include <unistd.h>

#include <net/lib/dns.h>

static void print_usage(void) {
	printf("Usage: dnsclient [-h] [-is IP address]\n");
}


int main(int argc, char **argv) {
	int opt;
	char *addr;
	char *(*dns_routine)(char *);

	dns_routine = NULL;

	while (-1 != (opt = getopt(argc, argv, "hs:i:"))) {
		switch (opt) {
		case 'i':
			dns_routine = dns_init_nameserver;
			break;
		case 's':
			dns_routine = dns_set_nameserver;
			break;
		case '?':
		case 'h':
		default:
			print_usage();
			return 0;
		}
	}
	if (!dns_routine) {
		printf("Resolv Server IP: %s\n", dns_get_nameserver());
	} else {
		if (argc == 3) {
			addr = dns_routine(argv[argc-1]);
			printf(" Resolv Server IP: %s\n", addr);
		} else {
			printf("Wrong arguments");
			print_usage();
		}
	}
	return 0;
}

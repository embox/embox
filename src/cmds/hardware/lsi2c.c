/**
 * @file
 * @brief show list i2c devices
 *
 * @date 21.04.14
 * @author Kirill Smirnov
 */

#include <stdio.h>
#include <unistd.h>
#include <drivers/i2c.h>

struct lsi2c_cb {
	int nbuses;
	int full;
};

static void print_usage(void) {
	printf("Usage: lsi2c [-f] [-h]\n");
	printf("\t[-f]      - full info\n");
	printf("\t[-h]      - print this help\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

static int i2c_bus_callback(int bus_id, void *cookie)
{
	struct lsi2c_cb *c = (struct lsi2c_cb*)cookie;

	c->nbuses++;
	return 0;
}

int main(int argc, char **argv) {
	int opt;
	int full = 0;
	struct lsi2c_cb cookie;

	if (argc == 1) {
		full = 0;
	} else {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "hf"))) {
			switch (opt) {
			case 'f':
				full = 1;
				break;
			case '?':
			case 'h':
				print_usage();
				return 0;
			default:
				print_error();
				return 0;
			}
		}
	}/*else have some parameters*/

	cookie.nbuses = 0;
	cookie.full = full;
	enumerate_i2c_buses(i2c_bus_callback, &cookie);

	printf("Total: %d i2c buses\n", cookie.nbuses);
	return 0;
}

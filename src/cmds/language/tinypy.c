/**
 * @file
 *
 * @date 14.01.13
 * @author Nikolay Korotkiy
 */

#include <tp.h>
#include <unistd.h>

extern tp_vm *tp_init(int argc, char **argv);
extern tp_obj tp_ez_call(TP, const char *mod, const char *fnc, tp_obj params);
extern void tp_deinit(TP);

static void usage(char **argv) {
	printf("Usage: %s <filename.py>\n", argv[0]);
}

int main(int argc, char **argv) {
	int opt;
	tp_vm *tp;

	if (argc != 2) {
		usage(argv);
		return 0;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "hasnrvmpio"))) {
		switch(opt) {
		default:
			printf("Unknown flag %c\n", opt);
		case 'h':
			usage(argv);
			return 0;
		}
	}

	tp = tp_init(argc, argv);

	tp_ez_call(tp, "py2bc", "tinypy", tp_None);
	tp_deinit(tp);
	return 0;
}

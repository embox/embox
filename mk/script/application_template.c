#include <embox/cmd.h>
#include <unistd.h>

EMBOX_CMD(exec);

extern int MAIN_ROUTING_NAME(int argc, char **argv);

static int exec(int argc, char **argv) {
	getopt_init();
	return MAIN_ROUTING_NAME(argc, argv);
}

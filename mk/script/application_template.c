
#include <embox/cmd.h>

#if CXX_CMD
extern void cxx_app_startup(void);
extern void cxx_app_termination(void);
extern int MAIN_ROUTING_NAME(int argc, char **argv);

static int cmd_main(int argc, char **argv) {
	int ret;

	cxx_app_startup();
	ret = MAIN_ROUTING_NAME(argc, argv);
	cxx_app_termination();

	return ret;
}

__EMBOX_EXTERN_CMD(cmd_main);
#else
__EMBOX_EXTERN_CMD(MAIN_ROUTING_NAME);
#endif

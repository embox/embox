/**
 * @file
 *
 * @date 14.01.13
 * @author Nikolay Korotkiy
 */

#include <embox/cmd.h>
#include <tp.h>

EMBOX_CMD(exec);

extern tp_vm *tp_init(int argc, char **argv);
extern tp_obj tp_ez_call(TP, const char *mod, const char *fnc, tp_obj params);
extern void tp_deinit(TP);

static int exec(int argc, char **argv) {
	tp_vm *tp = tp_init(argc, argv);

	tp_ez_call(tp, "py2bc", "tinypy", tp_None);
	tp_deinit(tp);
	return 0;
}

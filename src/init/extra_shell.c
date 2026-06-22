/**
 * @file
 * @brief Runs a shell on a separate tty
 *
 * @date 22.06.2026
 */
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <kernel/task.h>

#include <cmd/shell.h>

extern const char *setup_tty(const char *dev_name);

static void *extra_shell_run(void *arg) {
	const struct shell *sh;

	if (setup_tty(OPTION_STRING_GET(tty_dev)) == NULL) {
		return NULL;
	}

	sh = shell_lookup(OPTION_STRING_GET(shell_name));
	if (sh == NULL) {
		sh = shell_any();
	}

	if (sh != NULL) {
		shell_run(sh);
	}

	return NULL;
}

static int extra_shell_init(void) {
	return new_task(OPTION_STRING_GET(tty_dev), extra_shell_run, NULL) > 0
	           ? 0
	           : -1;
}

EMBOX_UNIT_INIT(extra_shell_init);

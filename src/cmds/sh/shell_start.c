/**
 * @file
 * @brief Shell starter
 * @details As a shell_api doesn't provide autostart on load, this
 * module starts shell on load
 *
 * @date 13.09.11
 * @author Anton Kozlov
 */

#include <cmd/shell.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(shell_start);

static int shell_start(void) {
	shell_run();
	return 0;
}

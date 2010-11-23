/**
 * @file
 *
 * @brief Implements utilities functions for virtual console working.
 *
 * @date 23.11.2010
 * @author Anton Bondarev
 */
#include <embox/unit.h>
#include <drivers/vconsole.h>

static vconsole_t def_console;
vconsole_t const *sys_console = &def_console;

EMBOX_UNIT_INIT(vconsole_init);

static int vconsole_init(void) {
	/*initialize pool of virtual consoles*/
	/*initialize default console*/
	return 0;
}

vconsole_t *vconsole_open(void) {
	return (vconsole_t *)sys_console;
}

int vconsole_activate(vconsole_t *con) {
	return 0;
}

int vconsole_deactivate(vconsole_t *con) {
	return 0;
}

int vconsole_close(vconsole_t *con) {
	return 0;
}

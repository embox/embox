/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2014
 */

/*#include <embox/unit.h>*/

/*EMBOX_UNIT_INIT(app_init);*/

extern int external_build_lib_call(void);

int main(void) {
	return 314 - external_build_lib_call();
}

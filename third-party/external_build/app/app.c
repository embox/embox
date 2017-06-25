/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2014
 */

/*#include <embox/unit.h>*/

/*EMBOX_UNIT_INIT(app_init);*/

#include <lib.h>

extern int external_build_lib_call(void);
extern int external_build_lib2_call(void);

int main(void) {
	return EXTERNAL_LIB_VAL - external_build_lib_call() +
		EXTERNAL_LIB_VAL - external_build_lib2_call();
}

/**
 * @file
 *
 * @date 21.03.09
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Eldar Abusalimov
 */

#include <types.h>
#include <common.h>

#include <kernel/init.h>
#include <express_tests.h>

// XXX
#define express_tests_execute(ign)

int init(void) {
	extern init_descriptor_t *__init_handlers_start, *__init_handlers_end;
	init_descriptor_t ** p_init_desc = &__init_handlers_start;
	int level;
	const char *init_name;
	const char *default_init_name = "???";

	express_tests_execute(PRE_INIT_LEVEL);

#if 0
	express_tests_execute(0);
#endif
	TRACE("\nStarting Monitor...\n");

	for (level = 0; level <= INIT_MAX_LEVEL; level++) {
		TRACE("\nLevel %d **********************\n", level);
		for (p_init_desc = &__init_handlers_start; p_init_desc
				< &__init_handlers_end; p_init_desc++) {
			init_name = default_init_name;
			if (NULL == (*p_init_desc)) {
				TRACE("Missing init descriptor\n");
				continue;
			}
			if ((*p_init_desc)->level != level) {
				continue;
			}
			if (NULL != (*p_init_desc)->name) {
				init_name = ((*p_init_desc)->name);
			}
			if (NULL == ((*p_init_desc)->init)) {
				TRACE("%s has a broken init handler descriptor.\n",
						init_name);
				continue;
			}
			TRACE("* Initializing %s\n", (*p_init_desc)->name);
			(*p_init_desc)->init();
		}
		TRACE("\n");
		/* after components initializing */
		express_tests_execute(level);
	}
	return 0;
}

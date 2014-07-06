/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.07.2014
 */

#include <stdlib.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(smac_user_dynamic_init_init);

#define SMAC_USER_INIT_DB "/smac_users.init"
#define SMAC_USER_TARGET_DB "/smac_users"

static int smac_user_dynamic_init_init(void) {

	return system("cp " SMAC_USER_INIT_DB " " SMAC_USER_TARGET_DB);
}

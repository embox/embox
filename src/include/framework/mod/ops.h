/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 16.07.24
 */

#ifndef FRAMEWORK_MOD_OPS_H_
#define FRAMEWORK_MOD_OPS_H_

#include <framework/mod/options.h>

#include <config/embox/framework/mod.h>

#define USE_MOD_NAMES \
	OPTION_MODULE_GET(embox__framework__mod, BOOLEAN, use_mod_names)

#define USE_MOD_LOGGER \
	OPTION_MODULE_GET(embox__framework__mod, BOOLEAN, use_mod_logger)

#define USE_MOD_LABELS \
	OPTION_MODULE_GET(embox__framework__mod, BOOLEAN, use_mod_labels)

#define USE_MOD_DEPENDS \
	OPTION_MODULE_GET(embox__framework__mod, BOOLEAN, use_mod_depends)

#define USE_CMD_DETAILS \
	OPTION_MODULE_GET(embox__framework__mod, BOOLEAN, use_cmd_details)

#endif /* FRAMEWORK_MOD_OPS_H_ */

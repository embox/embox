/**
 * @file
 * @brief XXX To be dropped soon. -- Eldar
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_TAG_H_
# error "Do not include this file directly, use <mod/tag.h> instead!"
#endif /* MOD_TAG_H_ */

#include "decls.h"
#include "array.h"

#define __MOD_TAG_PTR(s_tag) (&__MOD_TAG(s_tag))

#define __MOD_TAG_DEF(s_tag, tag_name) \
		__MOD_ARRAY_DEF(s_tag, tagged); \
		const struct mod_tag __MOD_TAG(s_tag) = { \
				.name = tag_name, \
				.mods = (struct mod **) &__MOD_ARRAY(s_tag, tagged), \
			}

struct mod_tag {
	/** (optional) Tag name. */
	const char *name;
	/** Null-terminated array of tagged mods. */
	struct mod **mods;
};


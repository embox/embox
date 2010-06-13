/**
 * @file
 * @brief TODO
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef IMPL_MOD_ARRAY_H_
#define IMPL_MOD_ARRAY_H_

#include <stddef.h>
#include <util/array.h>

#define __MOD_ARRAY(s_mod, s_array) __mod_##s_array##__$$##s_mod

#define __MOD_ARRAY_DEF(s_mod, s_array) \
		ARRAY_DIFFUSE_DEF_TERMINATED_STATIC( \
				const struct mod *, __MOD_ARRAY(s_mod, s_array), NULL)

#define __MOD_ARRAY_ADD(s_mod, s_array, s_mod_entry) \
		extern const struct mod *__MOD_ARRAY(s_mod, s_array)[]; \
		ARRAY_DIFFUSE_ADD(__MOD_ARRAY(s_mod, s_array), &__MOD(s_mod_entry))

#endif /* IMPL_MOD_ARRAY_H_ */

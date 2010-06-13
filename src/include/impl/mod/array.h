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
#include <util/macro.h>

/*
 * All mod-related sections are linked using something like
 * *(SORT(.mod*.rodata)). SORT guarantees that the wildcard is expanded in the
 * lexicographical order (order argument is used to control the placement of
 * sections within a single mod). We use this facility to create
 * null-terminated arrays of module dependencies initialized (populated) in
 * multiple compilation units.
 */

#define __MOD_SECTION(s_mod, section, ord, tag) \
	".mod" MACRO_STRING(__##section##__$$##s_mod##$$__##ord##_##tag) ".rodata"

#define __MOD_SECTION_HEAD(s_mod, section) __MOD_SECTION(s_mod, section,0,head)
#define __MOD_SECTION_BODY(s_mod, section) __MOD_SECTION(s_mod, section,1,body)
#define __MOD_SECTION_TAIL(s_mod, section) __MOD_SECTION(s_mod, section,9,tail)

#define __MOD_ARRAY(s_mod, s_array) __mod_##s_array##__$$##s_mod
#define __MOD_ARRAY_ENTRY(s_mod, s_array, s_entry) \
	__mod_##s_array##__$$##s_mod##$$__$$##s_entry

#define __MOD_ARRAY_DEF(s_mod, s_array) \
	__extension__ static const struct mod *__MOD_ARRAY(s_mod, s_array)[0] \
		__attribute__ ((section(__MOD_SECTION_HEAD(s_mod, s_array)))); \
	static const struct mod *__MOD_ARRAY_ENTRY(s_mod, s_array, __null$$) \
		__attribute__ ((used, section(__MOD_SECTION_TAIL(s_mod, s_array)))) \
		= NULL

#define __MOD_ARRAY_ADD(s_mod, s_array, s_mod_entry) \
	static const struct mod *__MOD_ARRAY_ENTRY(s_mod, s_array, s_mod_entry) \
		__attribute__ ((used, section(__MOD_SECTION_BODY(s_mod, s_array)))) \
		= &__MOD(s_mod_entry)

#endif /* IMPL_MOD_ARRAY_H_ */

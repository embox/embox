/**
 * @file
 * @brief XXX To be dropped soon. -- Eldar
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef MOD_TAG_H_
#define MOD_TAG_H_

#include <impl/mod/tag.h>

/**
 * Pointer to the #mod_tag structure defined with #MOD_TAG_DEF() macro.
 *
 * @param s_tag the mod_tag variable name used at definition time.
 */
#define MOD_TAG_PTR(s_tag) \
		__MOD_TAG_PTR(s_tag)

/**
 * Defines a new tag by allocating the @link #mod_tag corresponding @endlink
 * structure and a section-driven array for the list of the tagged mods.
 *
 * @param s_tag the variable name used later to access the tag structure
 *        with #MOD_TAG_PTR() and to bind the tag with a particular mod using
 *        #MOD_INFO_TAGGED_DEF() macro
 * @param tag_name a string containing the tag name
 */
#define MOD_TAG_DEF(s_tag, tag_name) \
		__MOD_TAG_DEF(s_tag, tag_name)

/**
 * Each mod can have an optional tag. Tag is used to group similar mods, e.g.
 * which are managed by the same framework.
 * Tag has no special meaning for the mods framework.
 *
 * @see #mod_tagged
 */
struct mod_tag;

/**
 * Gets the list of mods which are tagged by the specified tag.
 *
 * @param tag the target tag
 * @param iterator pointer to the #mod_iterator structure instance
 * @return iterator over the list of tagged mods
 * @retval the value passed as the @c iterator argument
 * @retval NULL if one or more arguments is @c NULL
 */
extern struct mod_iterator *mod_tagged(const struct mod_tag *tag,
		struct mod_iterator *iterator);

#endif /* MOD_TAG_H_ */

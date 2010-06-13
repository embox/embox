/**
 * @file
 * @brief TODO
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef IMPL_MOD_INFO_H_
#define IMPL_MOD_INFO_H_

struct mod_ops;
struct mod_tag;

/**
 * Mods framework manages each mod through a special interface so-called
 * @c mod_info.
 */
struct mod_info {
	/** (optional) Module specific data. */
	void *data;
	/** (optional) Available operations. */
	struct mod_ops *ops;
	/** (optional) Mod tag. */
	struct mod_tag *tag;
};

#endif /* IMPL_MOD_INFO_H_ */

/**
 * @file
 * @brief TODO
 *
 * @date 13.06.2010
 * @author Eldar Abusalimov
 */

#ifndef IMPL_MOD_INFO_H_
#define IMPL_MOD_INFO_H_

/**
 * @mainpage
 *
 * This manual documents the Embox C API.
 *
 * There are several other places to look for Embox information, such
 * as the wiki; those can be found at <a href="http://code.google.com/p/embox">
 * the Embox website</a>.
 */

struct mod_ops;

/**
 * Mods framework manages each mod through a special interface so-called
 * @c mod_info.
 */
struct __mod_info {
	/** (optional) Module specific data. */
	void *data;
	/** (optional) Available operations. */
	struct mod_ops *ops;
};

#endif /* IMPL_MOD_INFO_H_ */

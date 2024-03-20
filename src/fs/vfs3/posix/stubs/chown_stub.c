/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <unistd.h>

#include <util/log.h>

int chown(const char *path, uid_t owner_id, gid_t group_id) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

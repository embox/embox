/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <dirent.h>
#include <errno.h>
#include <stddef.h>

#include <util/log.h>

DIR *opendir(const char *path) {
	log_warning(">>> %s", __func__);
	return NULL;
}

int closedir(DIR *dir) {
	log_warning(">>> %s", __func__);
	return -ENOSYS;
}

struct dirent *readdir(DIR *dir) {
	log_warning(">>> %s", __func__);
	return NULL;
}

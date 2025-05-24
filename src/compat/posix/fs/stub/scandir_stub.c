/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 25.05.25
 */

#include <dirent.h>

#include <util/log.h>

int scandir(const char *dir_name, struct dirent ***name_list,
    int (*sel)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **)) {
	log_warning(">>> %s", __func__);
	return -1;
}

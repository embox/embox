/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 25.05.25
 */

#include <dirent.h>
#include <string.h>

int alphasort(const struct dirent **d1, const struct dirent **d2) {
	return strcoll((*d1)->d_name, (*d2)->d_name);
}

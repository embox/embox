/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 25.05.25
 */

#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int scandir(const char *dir_name, struct dirent ***name_list,
    int (*sel)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **)) {
	DIR *dir;
	struct dirent *dent;
	struct dirent **temp_list;
	struct dirent **new_list;
	size_t temp_list_size;
	size_t new_list_size;
	size_t num_dent;

	temp_list = NULL;
	temp_list_size = 0;
	num_dent = 0;

	dir = opendir(dir_name);
	if (dir == NULL) {
		return -1;
	}

	while ((dent = readdir(dir))) {
		if (sel != NULL && !(*sel)(dent)) {
			continue;
		}

		// re-allocate the list
		if (num_dent == temp_list_size) {
			new_list_size = temp_list_size + 32;
			new_list = (struct dirent **)realloc(temp_list,
			    new_list_size * sizeof(struct dirent *));

			if (new_list == NULL) {
				goto out_err;
			}

			temp_list_size = new_list_size;
			temp_list = new_list;
		}

		// add the copy of dirent to the list
		temp_list[num_dent] = (struct dirent *)malloc((dent->d_reclen + 3) & ~3);

		if (!temp_list[num_dent]) {
			goto out_err;
		}

		memcpy(temp_list[num_dent], dent, dent->d_reclen);
		num_dent++;
	}

	closedir(dir);

	if (compar != NULL) {
		qsort(temp_list, num_dent, sizeof(struct dirent *),
		    (int (*)(const void *, const void *))compar);
	}

	*name_list = temp_list;
	return num_dent;

out_err:
	closedir(dir);

	while (num_dent > 0) {
		free(temp_list[--num_dent]);
	}
	free(temp_list);

	return -1;
}

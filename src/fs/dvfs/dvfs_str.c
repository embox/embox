/**
 * @file
 * @brief Some handy functions to work with path
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-29
 */

/**
 * @brief Get pointer to last link in pathname. It's better
 * than strrchr because there could be traling slashes.
 *
 * @param path Given path
 *
 * @return Pointer to last link in pathname (within given path arg)
 */
char *dvfs_last_link(const char *path) {
	char *r, *l;
	int len;
	assert(path);

	len = strlen(path);
	t = path + len - 1;
	/* Miss traling slashes */
	while (*r == '/' && r > path)
		r--;

	/* Find previous slash or begin of path */
	l = r - 1;
	while (*l != '/' && l > path)
		l--;

	return l;
}

/**
 * @file
 *
 * @date Nov 22, 2012
 * @author: Anton Bondarev
 */

#ifndef FS_HLPR_PATH_H_
#define FS_HLPR_PATH_H_

#include <stddef.h>

extern int path_is_double_dot(const char *path);
extern int path_is_single_dot(const char *path);

/**
 * @brief Get stripped from leading '/' path and lenght of first node's name
 * For example,
 * for '/fooo/bar/' returns 'fooo/bar/' and 4 on plen
 * for '/bar/' ('fooo/bar/' + 4) returns 'bar/', 3
 * for '/' ('bar/' + 3) return NULL
 *
 * @param path Path to process
 * @param p_len Pointer to store first node name's length
 *
 * @return Path stripped from leading '/' if any
 * 	NULL if empty path supplied or none left
 */
extern const char *path_next(const char *path, size_t *p_len);

/*
 * remove the top directory name from path
 */
extern void path_cut_mount_dir(char *path, char *mount_dir) ;

/*
 *  move the last name of the whole directory to the top of tail
 */
extern int path_nip_tail(char *head, char *tail);

extern int path_increase_tail(char *head, char *tail);

/**
 * Save first node name in path into buff variable.
 * Return the remaining part of path.
 */
extern char *path_get_next_name(const char *path, char *buff, int buff_len);

/**
 * Tells whether a given @a name is "." or "..".
 * @retval 1 for "."
 * @retval 2 for ".."
 * @retval 0 otherwise
 */
extern int path_is_dotname(const char *name, size_t name_len);

#endif /* FS_HLPR_PATH_H_ */

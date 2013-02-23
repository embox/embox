/**
 * @file
 *
 * @date Nov 22, 2012
 * @author: Anton Bondarev
 */

#ifndef FS_PATH_H_
#define FS_PATH_H_

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

//TODO this is only FAT function
extern char *path_dir_to_canonical(char *dest, char *src, char dir);

extern char *path_canonical_to_dir(char *dest, char *src);


#endif /* FS_PATH_H_ */

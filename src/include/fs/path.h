/**
 * @file
 *
 * @date Nov 22, 2012
 * @author: Anton Bondarev
 */

#ifndef FS_PATH_H_
#define FS_PATH_H_


extern void path_cut_mount_dir(char *path, char *mount_dir) ;

extern int path_nip_tail(char *head, char *tail);

extern int path_increase_tail(char *head, char *tail);

extern char *path_dir_to_canonical(char *dest, char *src, char dir);

extern char *path_canonical_to_dir(char *dest, char *src);


#endif /* FS_PATH_H_ */

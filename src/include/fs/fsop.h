/**
 * @file
 * @brief
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#ifndef FS_FSOP_H_
#define FS_FSOP_H_

extern int format(const char *pathname, const char *fs_type);

extern int mkfs(const char *blk_name, const char *fs_type, char *fs_spec);

#endif /* FS_FSOP_H_ */

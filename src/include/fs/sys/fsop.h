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

extern int mount(char *dev,  char *dir, char *fs_type);

extern int rename(const char *src_name, const char *dst_name);

#endif /* FS_FSOP_H_ */

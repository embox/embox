/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.09.2012
 */

#ifndef FS_DVFS_KFILE_H_
#define FS_DVFS_KFILE_H_

//#include <fs/file_desc.h>
#include <stddef.h>
#include <sys/stat.h>

struct file_desc;
struct stat;

extern int dvfs_close(struct file_desc *desc);
extern int dvfs_write(struct file_desc *desc, char *buf, int count);
extern int dvfs_read(struct file_desc *desc, char *buf, int count);
extern int dvfs_fstat(struct file_desc *desc, struct stat *sb);

#endif /* FS_DVFS_KFILE_H_ */

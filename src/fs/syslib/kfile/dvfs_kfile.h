/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.09.2012
 */

#ifndef FS_DVFS_KFILE_H_
#define FS_DVFS_KFILE_H_

#include <sys/stat.h>

struct file_desc;
struct stat;

extern int kclose(struct file_desc *desc);
extern int kwrite(struct file_desc *desc, char *buf, int count);
extern int kread(struct file_desc *desc, char *buf, int count);
extern int kfstat(struct file_desc *desc, struct stat *sb);
extern int kioctl(struct file_desc *fp, int request, void *data);

#endif /* FS_DVFS_KFILE_H_ */

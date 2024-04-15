/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.09.2012
 */

#ifndef FS_KFILE_H_
#define FS_KFILE_H_

#include <fs/file_desc.h>
#include <stddef.h>
#include <sys/stat.h>

struct file_desc;
struct stat;
struct inode;

extern struct idesc * kopen(struct inode *node, int flag);

extern ssize_t kwrite(struct file_desc *file, const void *buf, size_t size);

extern ssize_t kread(struct file_desc *desc, void *buf, size_t size);

extern void kclose(struct file_desc *desc);

extern int kioctl(struct file_desc *fp, int request, void *data);

extern int kfstat(struct file_desc *fp, struct stat *buff);

extern int ktruncate(struct inode *node, off_t length);

extern int kftruncate(struct file_desc *desc, off_t length);

#endif /* FS_KFILE_H_ */

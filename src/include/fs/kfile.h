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

extern struct file_desc * kopen(struct node *node, int flag);

extern ssize_t kwrite(const void *buf, size_t size, struct file_desc *file);

extern ssize_t kread(void *buf, size_t size, struct file_desc *desc);

extern void kclose(struct file_desc *desc);

extern int kseek(struct file_desc *desc, long int offset, int origin);

extern int kioctl(struct file_desc *fp, int request, void *data);

extern int kfstat(struct file_desc *fp, struct stat *buff);

struct node;
extern int ktruncate(struct node *node, off_t length);

extern int kftruncate(struct file_desc *desc, off_t length);

#endif /* FS_KFILE_H_ */

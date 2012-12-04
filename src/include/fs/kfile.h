/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.09.2012
 */

#ifndef FS_KFILE_H_
#define FS_KFILE_H_

struct file_desc;
struct stat;

extern struct file_desc *kopen(const char *path, int flag);

extern size_t kwrite(const void *buf, size_t size, struct file_desc *file);

extern size_t kread(void *buf, size_t size, struct file_desc *desc);

extern int kclose(struct file_desc *desc);

extern int kseek(struct file_desc *desc, long int offset, int origin);

extern int kioctl(struct file_desc *fp, int request, ...);

extern int kfstat(struct file_desc *fp, struct stat *buff);

#endif /* FS_KFILE_H_ */

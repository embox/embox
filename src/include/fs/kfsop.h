/**
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#ifndef FS_KFSOP_H_
#define FS_KFSOP_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

struct node;
struct stat;
struct path;

extern int kmkdir(const char *pathname, mode_t mode);

extern int kremove(const char *pathname);

extern int kunlink(const char *pathname);

extern int krmdir(const char *pathname);

extern int klstat(const char *path, struct stat *buf);

extern int kformat(const char *pathname, const char *fs_type);

extern int kmount(const char *dev, const char *dir, const char *fs_type);

extern int kumount(const char *dir);

extern int krename(const char *oldpath, const char *newpath);

extern int kfile_fill_stat(struct node *node, struct stat *stat_buff);

extern int kflock(int fd, int operation);

extern int kutime(const char *path, const struct utimbuf *times);

extern int kfile_change_stat(struct node *node, const struct utimbuf *times);

#endif /* FS_KFSOP_H_ */

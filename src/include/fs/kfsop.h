/**
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#ifndef FS_KFSOP_H_
#define FS_KFSOP_H_

#include <fs/node.h>
#include <sys/stat.h>
#include <sys/types.h>

struct node;
struct stat;

extern int kcreat(struct node *node, const char *pathname, mode_t mode);

extern int kmkdir(struct node *node, const char *pathname, mode_t mode);

extern int kremove(const char *pathname);

extern int kunlink(const char *pathname);

extern int krmdir(const char *pathname);

extern int klstat(const char *path, struct stat *buf);

extern int kformat(const char *pathname, const char *fs_type);

extern int kmount(char *dev,  char *dir, char *fs_type);


extern int kfile_fill_stat(struct node *node, struct stat *stat_buff);

#endif /* FS_KFSOP_H_ */

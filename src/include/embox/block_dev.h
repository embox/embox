/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */


#ifndef BLOCK_DEV_H_
#define BLOCK_DEV_H_

#include <fs/file_desc.h>
#include <fs/vfs.h>
#include <kernel/file.h>
#include <util/array.h>

#define IOCTL_GETBLKSIZE        1
#define IOCTL_GETDEVSIZE        2
#define IOCTL_GETGEOMETRY       3
#define IOCTL_REVALIDATE        4

#define NODEV                 (-1)
#define DEV_TYPE_STREAM         1
#define DEV_TYPE_BLOCK          2
#define DEV_TYPE_PACKET         3

typedef struct block_dev_geometry {
	int cyls;
	int heads;
	int spt;
	int sectorsize;
	int sectors;
} block_dev_geometry_t;

typedef struct block_dev {
	node_t *dev_node;
	char name[16];
	struct block_dev_driver *driver;
	void *privdata;
	int refcnt;
	uid_t uid;
	gid_t gid;
	int mode;
	size_t size;

	int reads;
	int writes;
	int input;
	int output;
} block_dev_t;

typedef struct block_dev_driver {
	char *name;
	int type;

	int (*ioctl)(block_dev_t *dev, int cmd, void *args, size_t size);
	int (*read)(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno);
	int (*write)(block_dev_t *dev, char *buffer, size_t count, blkno_t blkno);
} block_dev_driver_t;

extern block_dev_t *devtab[64];

/* extern const block_dev_module_t __block_dev_registry[]; */
/* extern block_dev_module_t *block_dev_find(char *name); */

extern int dev_read(dev_t devno, char *buffer, size_t count, blkno_t blkno);
extern int dev_write(dev_t devno, char *buffer, size_t count, blkno_t blkno);
extern int dev_ioctl(dev_t devno, int cmd, void *args, size_t size);
extern block_dev_t *device(dev_t devno);
extern int dev_destroy (dev_t devno);
extern dev_t dev_make(char *name, block_dev_driver_t *driver, void *privdata);
extern dev_t devno(char *name);
extern dev_t dev_open(char *name);
extern int dev_close(dev_t devno);

/*
#define EMBOX_BLOCK_DEV(name, block_dev_operations) \
	ARRAY_SPREAD_ADD(__block_dev_registry, {name, block_dev_operations})
*/

#endif /* BLOCK_DEV_H_ */

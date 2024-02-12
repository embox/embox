/**
 * @file
 * @brief
 *
 * @date 20.08.2012
 * @author Andrey Gazukin
 */

#ifndef ISO9660_H_
#define ISO9660_H_

#include <stdint.h>
#include <time.h>

#include <fs/iso9660_format.h>

#define CDFS_BLOCKSIZE         2048
#define CDFS_POOLDEPTH         10

#define PS1                     '/'     /* Primary path separator */
#define PS2                     '\\'    /* Alternate path separator */

struct cdfs_fs_info {
	void *bdev;
	uint64_t blks;
	int volblks;
	int vdblk;
	int joliet;
	unsigned char *path_table_buffer;
	struct iso_pathtable_record **path_table;
	int path_table_records;
};

struct cdfs_file_info {
	int flags;
	int64_t pos;
	int extent;
	int size;
	time_t date;
};


extern struct cdfs_fs_info *iso9660_fsi_alloc(void);

extern void iso9660_fsi_free(struct cdfs_fs_info *fs);

extern struct cdfs_file_info *iso9660_fi_alloc(void);

extern void iso9660_fi_free(struct cdfs_file_info *fs);


#endif /* ISO9660_H_ */

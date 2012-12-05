/**
 * @file
 * @brief
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */


#ifndef EXT_H_
#define EXT_H_

typedef struct ext_fs_info {
	char mntfrom[MAX_LENGTH_PATH_NAME];
	char mntto[MAX_LENGTH_PATH_NAME];
	uint32_t numblocks;			/* number of block in volume */
	uint32_t block_size;		/* size of block */
	uint32_t block_per_file;	/* max number of blocks filesize*/
} ext_fs_info_t;

typedef struct ext_file_info {
	int     index;		        /* number of file in FS*/
	int     mode;				/* mode in which this file was opened */
	uint32_t pointer;			/* current (BYTE) pointer */
} ext_file_info_t;

#endif /* EXT_H_ */

/**
 * @file
 * @brief
 *
 * @date 28.03.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>

#include <util/math.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <fs/fs_driver.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/fat.h>
#include <fs/hlpr_path.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

#include <embox/block_dev.h>

#include <framework/mod/options.h>

#define FAT_MAX_SECTOR_SIZE OPTION_GET(NUMBER, fat_max_sector_size)

uint8_t fat_sector_buff[FAT_MAX_SECTOR_SIZE];

/* fat filesystem description pool */
POOL_DEF(fat_fs_pool, struct fat_fs_info,
	OPTION_GET(NUMBER, fat_descriptor_quantity));

/* fat file description pool */
POOL_DEF(fat_file_pool, struct fat_file_info,
	OPTION_GET(NUMBER, inode_quantity));

extern char bootcode[];

static const struct fs_driver fatfs_driver;
extern int fat_write_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector);
extern int fat_read_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector);
extern uint32_t fat_get_next(struct fat_fs_info *fsi,
		struct dirinfo * dirinfo, struct dirent * dirent);
static int fat_create_dir_entry (struct nas *parent_nas);

static struct fat_file_info *fat_fi_alloc(struct nas *nas, void *fs) {
	struct fat_file_info *fi;

	fi = pool_alloc(&fat_file_pool);
	if (fi) {
		memset(fi, 0, sizeof(*fi));
		nas->fi->privdata = fi;
		nas->fs = fs;
	}

	return fi;
}

extern int fat_create_partition(void *bdev);
extern uint32_t fat_get_ptn_start(void *bdev, uint8_t pnum, uint8_t *pactive,
		uint8_t *pptype, uint32_t *psize);

extern uint32_t fat_get_volinfo(void *bdev, struct volinfo * volinfo, uint32_t startsector);
extern uint32_t fat_get_fat_(struct fat_fs_info *fsi,
		uint8_t *p_scratch,	uint32_t *p_scratchcache, uint32_t cluster);
extern uint32_t fat_set_fat_(struct fat_fs_info *fsi, uint8_t *p_scratch,
		uint32_t *p_scratchcache, uint32_t cluster, uint32_t new_contents);
extern uint32_t fat_get_free_fat_(struct fat_fs_info *fsi, uint8_t *p_scratch);
extern uint32_t fat_open_dir(struct fat_fs_info *fsi,
		uint8_t *dirname, struct dirinfo *dirinfo);
extern uint32_t fat_get_next(struct fat_fs_info *fsi,
		struct dirinfo *dirinfo, struct dirent *dirent);
extern uint32_t fat_get_free_dir_ent(struct fat_fs_info *fsi, uint8_t *path,
		struct dirinfo *di, struct dirent *de);
extern void fat_set_direntry (uint32_t dir_cluster, uint32_t cluster);

/*
 * Create a file or directory. You supply a file_create_param_t
 * structure.
 * Returns various DFS_* error states. If the result is DFS_OK, file
 * was created and can be used.
 */
static int fat_create_file(struct node * parent_node, struct node *node) {
	char tmppath[PATH_MAX];
	uint8_t filename[12];
	struct dirinfo di;
	struct dirent de;
	struct volinfo *volinfo;
	struct nas *nas;
	uint32_t cluster, temp;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;

	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	volinfo = &fsi->vi;
	fi->volinfo = volinfo;

	vfs_get_relative_path(node, tmppath, PATH_MAX);

	fat_get_filename(tmppath, (char *) filename);

	/*
	 *  At this point, if our path was MYDIR/MYDIR2/FILE.EXT,
	 *  filename = "FILE    EXT" and  tmppath = "MYDIR/MYDIR2".
	 */
	di.p_scratch = fat_sector_buff;
	if (fat_open_dir(fsi, (uint8_t *) tmppath, &di)) {
		return DFS_NOTFOUND;
	}

	while (!fat_get_next(fsi, &di, &de));

	/* Locate or create a directory entry for this file */
	if (DFS_OK != fat_get_free_dir_ent(fsi, (uint8_t *) tmppath, &di, &de)) {
		return DFS_ERRMISC;
	}

	assert(nas->fs->bdev == fsi->bdev);
	cluster = fat_get_free_fat_(fsi, fat_sector_buff);
	de = (struct dirent) {
		.attr = S_ISDIR(node->mode) ? ATTR_DIRECTORY : 0,
		.startclus_l_l = cluster & 0xff,
		.startclus_l_h = (cluster & 0xff00) >> 8,
		.startclus_h_l = (cluster & 0xff0000) >> 16,
		.startclus_h_h = (cluster & 0xff000000) >> 24,
	};
	memcpy(de.name, filename, MSDOS_NAME);
	fat_set_filetime(&de);

	fi->volinfo = volinfo;
	fi->pointer = 0;
	/*
	 * The reason we store this extra info about the file is so that we can
	 * speedily update the file size, modification date, etc. on a file
	 * that is opened for writing.
	 */
	if (di.currentcluster == 0) {
		fi->dirsector = volinfo->rootdir + di.currentsector;
	} else {
		fi->dirsector = volinfo->dataarea +
				((di.currentcluster - 2) * volinfo->secperclus) +
				di.currentsector;
	}
	fi->diroffset = di.currententry - 1;
	fi->cluster = cluster;
	fi->firstcluster = cluster;
	nas->fi->ni.size = 0;

	/*
	 * write the directory entry
	 * note that we no longer have the sector containing the directory
	 * entry, tragically, so we have to re-read it
	 */

	if (fat_read_sector(fsi, fat_sector_buff, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	memcpy(&(((struct dirent*) fat_sector_buff)[di.currententry - 1]),
			&de, sizeof(struct dirent));
	if (fat_write_sector(fsi, fat_sector_buff, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	/* Mark newly allocated cluster as end of chain */
	switch(volinfo->filesystem) {
		case FAT12:		cluster = 0xfff;	break;
		case FAT16:		cluster = 0xffff;	break;
		case FAT32:		cluster = 0x0fffffff;	break;
		default:		return DFS_ERRMISC;
	}

	temp = 0;
	assert(nas->fs->bdev == fsi->bdev);
	fat_set_fat_(fsi,
			fat_sector_buff, &temp, fi->cluster, cluster);

	if (node_is_directory(node)) {
		/* create . and ..  files of this catalog */
		fat_set_direntry(di.currentcluster, fi->cluster);
		cluster = fi->volinfo->dataarea +
				  ((fi->cluster - 2) * fi->volinfo->secperclus);
		if (fat_write_sector(fsi, fat_sector_buff, cluster)) {
			return DFS_ERRMISC;
		}
	}

	return DFS_OK;
}

/*
 * Open a file for reading or writing. You supply populated volinfo_t,
 * a path to the file, mode (DFS_READ or DFS_WRITE) and an empty fileinfo
 * structure. You also need to provide a pointer to a sector-sized scratch
 * buffer.
 * Returns various DFS_* error states. If the result is DFS_OK, fileinfo
 * can be used to access the file from this point on.
 */
static uint32_t fat_open_file(struct nas *nas, uint8_t *path, int mode,
		uint8_t *p_scratch) {
	char tmppath[PATH_MAX];
	uint8_t filename[12];
	struct dirinfo di;
	struct dirent de;

	struct volinfo *volinfo;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	volinfo = &fsi->vi;

	fi->mode = mode;

	strncpy((char *) tmppath, (char *) path, PATH_MAX);
	tmppath[PATH_MAX - 1] = 0;
	if (strcmp((char *) path,(char *) tmppath)) {
		return DFS_PATHLEN;
	}

	fat_get_filename(tmppath, (char *) filename);

	/*
	 *  At this point, if our path was MYDIR/MYDIR2/FILE.EXT,
	 *  filename = "FILE    EXT" and  tmppath = "MYDIR/MYDIR2".
	 */

	di.p_scratch = p_scratch;
	if (fat_open_dir(fsi, (uint8_t *) tmppath, &di)) {
		return DFS_NOTFOUND;
	}

	while (!fat_get_next(fsi, &di, &de)) {
		path_canonical_to_dir(tmppath, (char *) de.name);
		if (!memcmp(tmppath, filename, MSDOS_NAME)) {
			if (de.attr & ATTR_DIRECTORY){
				return DFS_NOTFOUND;
			}

			fi->volinfo = volinfo;
			fi->pointer = 0;
			/*
			 * The reason we store this extra info about the file is so that we
			 * can speedily update the file size, modification date, etc. on a
			 * file that is opened for writing.
			 */
			if (di.currentcluster == 0) {
				fi->dirsector = volinfo->rootdir + di.currentsector;
			} else {
				fi->dirsector = volinfo->dataarea +
						((di.currentcluster - 2) *
						volinfo->secperclus) + di.currentsector;
			}
			fi->diroffset = di.currententry - 1;
			if (volinfo->filesystem == FAT32) {
				fi->cluster = (uint32_t) de.startclus_l_l |
				  ((uint32_t) de.startclus_l_h) << 8 |
				  ((uint32_t) de.startclus_h_l) << 16 |
				  ((uint32_t) de.startclus_h_h) << 24;
			} else {
				fi->cluster = (uint32_t) de.startclus_l_l |
				  ((uint32_t) de.startclus_l_h) << 8;
			}
			fi->firstcluster = fi->cluster;
			nas->fi->ni.size = (uint32_t) de.filesize_0 |
			  ((uint32_t) de.filesize_1) << 8 |
			  ((uint32_t) de.filesize_2) << 16 |
			  ((uint32_t) de.filesize_3) << 24;

			return DFS_OK;
		}
	}
	return DFS_NOTFOUND;
}

/*
 * Read an open file
 * You must supply a prepopulated file_info_t as provided by fat_open_file,
 * and a pointer to a volinfo->bytepersec scratch buffer.
 * 	Note that returning DFS_EOF is not an error condition. This function
 * 	updates the	successcount field with the number of bytes actually read.
 */
static uint32_t fat_read_file(struct nas *nas, uint8_t *p_scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len) {
	uint32_t remain;
	uint32_t result;
	uint32_t sector;
	uint32_t bytesread;
	uint32_t clastersize;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	/* Don't try to read past EOF */
	if (len > nas->fi->ni.size - fi->pointer) {
		len = nas->fi->ni.size - fi->pointer;
	}

	result = DFS_OK;
	remain = len;
	*successcount = 0;
	clastersize = fi->volinfo->secperclus * fi->volinfo->bytepersec;

	while (remain && result == DFS_OK) {
		/* This is a bit complicated. The sector we want to read is addressed
		 * at a cluster granularity by the fi->cluster member. The file
		 * pointer tells us how many extra sectors to add to that number.
		 */
		sector = fi->volinfo->dataarea +
		  ((fi->cluster - 2) * fi->volinfo->secperclus) +
		  div(div(fi->pointer, clastersize).rem, fi->volinfo->bytepersec).quot;

		/* Case 1 - File pointer is not on a sector boundary */
		if (div(fi->pointer, fi->volinfo->bytepersec).rem) {
			uint16_t tempreadsize;

			/* We always have to go through scratch in this case */
			result = fat_read_sector(fsi, p_scratch, sector);

			/*
			 * This is the number of bytes that we actually care about in the
			 * sector just read.
			 */
			tempreadsize = fi->volinfo->bytepersec -
					(div(fi->pointer, fi->volinfo->bytepersec).rem);

			/* Case 1A - We want the entire remainder of the sector. After this
			 * point, all passes through the read loop will be aligned on a
			 * sector boundary, which allows us to go through the optimal path
			 *  2A below.
			 */
		   	if (remain >= tempreadsize) {
				memcpy(buffer, p_scratch + (fi->volinfo->bytepersec - tempreadsize),
						tempreadsize);
				bytesread = tempreadsize;
				buffer += tempreadsize;
				fi->pointer += tempreadsize;
				remain -= tempreadsize;
			}
			/* Case 1B - This read concludes the file read operation */
			else {
				memcpy(buffer, p_scratch +
						(fi->volinfo->bytepersec - tempreadsize), remain);

				buffer += remain;
				fi->pointer += remain;
				bytesread = remain;
				remain = 0;
			}
		}
		/* Case 2 - File pointer is on sector boundary */
		else {
			/*
			 * Case 2A - We have at least one more full sector to read and
			 * don't have to go through the scratch buffer. You could insert
			 * optimizations here to read multiple sectors at a time, if you
			 * were thus inclined (note that the maximum multi-read you could
			 * perform is a single cluster, so it would be advantageous to have
			 * code similar to case 1A above that would round the pointer to a
			 * cluster boundary the first pass through, so all subsequent
			 * [large] read requests would be able to go a cluster at a time).
			 */
			 if (remain >= fi->volinfo->bytepersec) {
				result = fat_read_sector(fsi, buffer, sector);
				remain -= fi->volinfo->bytepersec;
				buffer += fi->volinfo->bytepersec;
				fi->pointer += fi->volinfo->bytepersec;
				bytesread = fi->volinfo->bytepersec;
			}
			/* Case 2B - We are only reading a partial sector */
			else {
				result = fat_read_sector(fsi, p_scratch, sector);
				memcpy(buffer, p_scratch, remain);
				buffer += remain;
				fi->pointer += remain;
				bytesread = remain;
				remain = 0;
			}
		}

		*successcount += bytesread;
		/* check to see if we stepped over a cluster boundary */
		if (div(fi->pointer - bytesread, clastersize).quot !=
			div(fi->pointer, clastersize).quot) {
			/*
			 * An act of minor evil - we use bytesread as a scratch integer,
			 * knowing that its value is not used after updating *successcount
			 * above
			 */
			bytesread = 0;
			if (((fi->volinfo->filesystem == FAT12) &&
					(fi->cluster >= 0xff8)) ||
					((fi->volinfo->filesystem == FAT16) &&
							(fi->cluster >= 0xfff8)) ||
							((fi->volinfo->filesystem == FAT32) &&
									(fi->cluster >= 0x0ffffff8))) {
				result = DFS_EOF;
			}
			else {
				assert(nas->fs->bdev == fsi->bdev);
				fi->cluster = fat_get_fat_(fsi, p_scratch, &bytesread, fi->cluster);
			}
		}
	}

	return result;
}


/*
 * Delete a file
 * p_scratch must point to a sector-sized buffer
 */
static int fat_unlike_file(struct nas *nas, uint8_t *path,
		uint8_t *p_scratch) {
	uint32_t cache;
	uint32_t tempclus;
	struct volinfo *volinfo;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	volinfo = &fsi->vi;
	cache = 0;

	if (DFS_OK != fat_open_file(nas, path, O_RDONLY, p_scratch)) {
		return DFS_NOTFOUND;
	}

	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	((struct dirent*) p_scratch)[fi->diroffset].name[0] = 0xe5;
	if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	/* Now follow the cluster chain to free the file space */
	while (!((volinfo->filesystem == FAT12 && fi->firstcluster >= 0x0ff7) ||
			(volinfo->filesystem == FAT16 && fi->firstcluster >= 0xfff7) ||
			(volinfo->filesystem == FAT32 && fi->firstcluster >= 0x0ffffff7))) {
		tempclus = fi->firstcluster;
		assert(fsi->bdev == nas->fs->bdev);
		fi->firstcluster = fat_get_fat_(fsi, p_scratch,
				&cache, fi->firstcluster);
		assert(nas->fs->bdev == fsi->bdev);
		fat_set_fat_(fsi, p_scratch, &cache, tempclus, 0);
	}
	return DFS_OK;
}

/*
 * Delete a file
 * p_scratch must point to a sector-sized buffer
 */
static int fat_unlike_directory(struct nas *nas, uint8_t *path,
		uint8_t *p_scratch) {
	uint32_t cache;
	uint32_t tempclus;
	struct volinfo *volinfo;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;

	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;
	volinfo = &fsi->vi;

	cache = 0;

	/* fat_open_file gives us all the information we need to delete it */
	if (DFS_OK != fat_open_file(nas, path, O_RDONLY, p_scratch)) {
		return DFS_NOTFOUND;
	}

	/* First, read the directory sector and delete that entry */
	if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}
	((struct dirent*) p_scratch)[fi->diroffset].name[0] = 0xe5;
	if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
		return DFS_ERRMISC;
	}

	/* Now follow the cluster chain to free the file space */
	while (!((volinfo->filesystem == FAT12 && fi->firstcluster >= 0x0ff7) ||
			(volinfo->filesystem == FAT16 && fi->firstcluster >= 0xfff7) ||
			(volinfo->filesystem == FAT32 && fi->firstcluster >= 0x0ffffff7))) {
		tempclus = fi->firstcluster;
		assert(nas->fs->bdev == fsi->bdev);
		fi->firstcluster = fat_get_fat_(fsi, p_scratch,
				&cache, fi->firstcluster);
		assert(nas->fs->bdev == fsi->bdev);
		fat_set_fat_(fsi, p_scratch, &cache, tempclus, 0);
	}
	return DFS_OK;
}

/*
 * Write an open file
 * You must supply a prepopulated file_info_t as provided by
 * fat_open_file, and a pointer to a SECTOR_SIZE scratch buffer.
 * This function updates the successcount field with the number
 * of bytes actually written.
 */
static uint32_t fat_write_file(struct nas *nas, uint8_t *p_scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len) {
	uint32_t remain;
	uint32_t result = DFS_OK;
	uint32_t sector;
	uint32_t byteswritten;
	uint32_t lastcluster, nextcluster;
	uint32_t clastersize;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	if (!(fi->mode & O_WRONLY) && !(fi->mode & O_APPEND)) {
		return DFS_ERRMISC;
	}

	remain = len;
	*successcount = 0;
	clastersize = fi->volinfo->secperclus * fi->volinfo->bytepersec;

	while (remain && result == DFS_OK) {
		/*
		 * This is a bit complicated. The sector we want to read is addressed
		 * at a cluster granularity by  the fi->cluster member.
		 * The file pointer tells us how many extra sectors to add to that
		 * number.
		 */
		sector = fi->volinfo->dataarea +
		  ((fi->cluster - 2) * fi->volinfo->secperclus) +
		  div(div(fi->pointer, clastersize).rem, fi->volinfo->bytepersec).quot;

		/* Case 1 - File pointer is not on a sector boundary */
		if (div(fi->pointer, fi->volinfo->bytepersec).rem) {
			uint16_t tempsize;

			/* We always have to go through scratch in this case */
			result = fat_read_sector(fsi, p_scratch, sector);

			/*
			 * This is the number of bytes that we don't want to molest in the
			 * scratch sector just read.
			 */
			tempsize = div(fi->pointer, fi->volinfo->bytepersec).rem;

			/*
			 * Case 1A - We are writing the entire remainder of the sector.
			 * After this point, all passes through the read loop will be
			 * aligned on a sector boundary, which allows us to go through the
			 * optimal path
			 * 2A below.
			 */
		   	if (remain >= fi->volinfo->bytepersec - tempsize) {
				memcpy(p_scratch + tempsize, buffer, fi->volinfo->bytepersec - tempsize);
				if (!result) {
					result = fat_write_sector(fsi, p_scratch, sector);
				}

				byteswritten = fi->volinfo->bytepersec - tempsize;
				buffer += fi->volinfo->bytepersec - tempsize;
				fi->pointer += fi->volinfo->bytepersec - tempsize;
				if (nas->fi->ni.size < fi->pointer) {
					nas->fi->ni.size = fi->pointer;
				}
				remain -= fi->volinfo->bytepersec - tempsize;
			}
			/* Case 1B - This concludes the file write operation */
			else {
				memcpy(p_scratch + tempsize, buffer, remain);
				if (!result) {
					result = fat_write_sector(fsi, p_scratch, sector);
				}

				buffer += remain;
				fi->pointer += remain;
				if (nas->fi->ni.size < fi->pointer) {
					nas->fi->ni.size = fi->pointer;
				}
				byteswritten = remain;
				remain = 0;
			}
		} /* case 1 */
		/* Case 2 - File pointer is on sector boundary */
		else {
			/* Case 2A - We have at least one more full sector to write and
			 * don't have to go through the scratch buffer. You could insert
			 * optimizations here to write multiple sectors at a time, if you
			 * were thus inclined. Refer to similar notes in fat_read_file.
			 */
			if (remain >= fi->volinfo->bytepersec) {
				result = fat_write_sector(fsi, buffer, sector);
				remain -= fi->volinfo->bytepersec;
				buffer += fi->volinfo->bytepersec;
				fi->pointer += fi->volinfo->bytepersec;
				if (nas->fi->ni.size < fi->pointer) {
					nas->fi->ni.size = fi->pointer;
				}
				byteswritten = fi->volinfo->bytepersec;
			}
			/*
			 * Case 2B - We are only writing a partial sector and potentially
			 * need to go through the scratch buffer.
			 */
			else {
				/* If the current file pointer is not yet at or beyond the file
				 * length, we are writing somewhere in the middle of the file
				 * and need to load the original sector to do
				 * a read-modify-write.
				 */
				if (fi->pointer < nas->fi->ni.size) {
					result = fat_read_sector(fsi, p_scratch, sector);
					if (!result) {
						memcpy(p_scratch, buffer, remain);
						result = fat_write_sector(fsi, p_scratch, sector);
					}
				}
				else {
					memset(p_scratch, 0, fi->volinfo->bytepersec);
					memcpy(p_scratch, buffer, remain);
					result = fat_write_sector(fsi, p_scratch, sector);
				}

				buffer += remain;
				fi->pointer += remain;
				if (nas->fi->ni.size < fi->pointer) {
					nas->fi->ni.size = fi->pointer;
				}
				byteswritten = remain;
				remain = 0;
			}
		}

		*successcount += byteswritten;

		/* check to see if we stepped over a cluster boundary */
		if (div(fi->pointer - byteswritten, clastersize).quot !=
				div(fi->pointer, clastersize).quot) {

		  	/* We've transgressed into another cluster. If we were already
		  	 * at EOF, we need to allocate a new cluster.
		  	 * An act of minor evil - we use byteswritten as a scratch integer,
		  	 * knowing that its value is not used after updating *successcount
		  	 * above
		  	 */
		  	byteswritten = 0;

			lastcluster = fi->cluster;
			assert(nas->fs->bdev == fsi->bdev);
			fi->cluster = fat_get_fat_(fsi, p_scratch,
					&byteswritten, fi->cluster);

			/* Allocate a new cluster? */
			if (((fi->volinfo->filesystem == FAT12) &&
					(fi->cluster >= 0xff8)) ||
					((fi->volinfo->filesystem == FAT16) &&
					(fi->cluster >= 0xfff8)) ||
					((fi->volinfo->filesystem == FAT32) &&
					(fi->cluster >= 0x0ffffff8))) {
			  	uint32_t tempclus;
				assert(nas->fs->bdev == fsi->bdev);
				tempclus = fat_get_free_fat_(fsi, p_scratch);
				byteswritten = 0; /* invalidate cache */
				if (tempclus == 0x0ffffff7) {
					return DFS_ERRMISC;
				}
				/* Link new cluster onto file */
				assert(nas->fs->bdev == fsi->bdev);
				fat_set_fat_(fsi, p_scratch,
						&byteswritten, lastcluster, tempclus);
				fi->cluster = tempclus;

				/* Mark newly allocated cluster as end of chain */
				switch(fi->volinfo->filesystem) {
					case FAT12:		tempclus = 0xfff;	break;
					case FAT16:		tempclus = 0xffff;	break;
					case FAT32:		tempclus = 0x0fffffff;	break;
					default:		return DFS_ERRMISC;
				}
				fat_set_fat_(fsi, p_scratch,
						&byteswritten, fi->cluster, tempclus);

				result = DFS_OK;
			}
		}
	}
	    /* If cleared, then mark free clusters*/
		if(nas->fi->ni.size > fi->pointer) {
			if (div(nas->fi->ni.size, clastersize).quot !=
				div(fi->pointer, clastersize).quot) {

				byteswritten = 0;/* invalidate cache */
				assert(nas->fs->bdev == fsi->bdev);
				nextcluster = fat_get_fat_(fsi, p_scratch,
						&byteswritten, fi->cluster);

				switch(fi->volinfo->filesystem) {
					case FAT12:		lastcluster = 0xfff;	break;
					case FAT16:		lastcluster = 0xfff;	break;
					case FAT32:		lastcluster = 0x0fffffff;	break;
					default:		return DFS_ERRMISC;
				}
				assert(nas->fs->bdev == fsi->bdev);
				fat_set_fat_(fsi, p_scratch, &byteswritten,
						fi->cluster, lastcluster);

				/* Now follow the cluster chain to free the file space */
				while (!((fi->volinfo->filesystem == FAT12 &&
						nextcluster >= 0x0ff7) ||
						(fi->volinfo->filesystem == FAT16 &&
						nextcluster >= 0xfff7) ||
						(fi->volinfo->filesystem == FAT32 &&
						nextcluster >= 0x0ffffff7))) {
					lastcluster = nextcluster;
					assert(nas->fs->bdev == fsi->bdev);
					nextcluster = fat_get_fat_(fsi, p_scratch,
							&byteswritten, nextcluster);

					fat_set_fat_(fsi, p_scratch,
							&byteswritten, lastcluster, 0);
				}

			}
		}

		nas->fi->ni.size = fi->pointer;

		/* Update directory entry */
		if (fat_read_sector(fsi, p_scratch, fi->dirsector)) {
			return DFS_ERRMISC;
		}

		((struct dirent*) p_scratch)[fi->diroffset].filesize_0 =
				nas->fi->ni.size & 0xff;

		((struct dirent*) p_scratch)[fi->diroffset].filesize_1 =
				(nas->fi->ni.size & 0xff00) >> 8;

		((struct dirent*) p_scratch)[fi->diroffset].filesize_2 =
				(nas->fi->ni.size & 0xff0000) >> 16;

		((struct dirent*) p_scratch)[fi->diroffset].filesize_3 =
				(nas->fi->ni.size & 0xff000000) >> 24;

		if (fat_write_sector(fsi, p_scratch, fi->dirsector)) {
			return DFS_ERRMISC;
		}

	return result;
}

extern int fat_read_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector);
extern int fat_write_sector(struct fat_fs_info *fsi, uint8_t *buffer, uint32_t sector);
extern int fat_root_dir_record(void *bdev);

static int fat_mount_files(struct nas *dir_nas) {
	uint32_t cluster;
	struct node *node;
	struct nas *nas;
	uint32_t pstart, psize;
	uint8_t pactive, ptype;
	struct dirinfo di;
	struct dirent de;
	uint8_t name[MSDOS_NAME + 2];
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	mode_t mode;

	assert(dir_nas);
	fsi = dir_nas->fs->fsi;

	pstart = fat_get_ptn_start(dir_nas->fs->bdev, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		return -1;
	}
	if (fat_get_volinfo(dir_nas->fs->bdev, &fsi->vi, pstart)) {
		return -1;
	}
	di.p_scratch = fat_sector_buff;
	if (fat_open_dir(fsi, (uint8_t *) ROOT_DIR, &di)) {
		return -EBUSY;
	}
	/* move out from first root directory entry table*/
	cluster = fat_get_next(fsi, &di, &de);

	while (DFS_EOF != (cluster = fat_get_next(fsi, &di, &de))) {
		if (de.name[0] == 0)
			continue;

		path_dir_to_canonical((char *) name, (char *) de.name,
							  de.attr & ATTR_DIRECTORY);
		if ((0 == strncmp((char *) de.name, ".  ", 3)) ||
			(0 == strncmp((char *) de.name, ".. ", 3))) {
			continue;
		}

		mode = (de.attr & ATTR_DIRECTORY) ? S_IFDIR : S_IFREG;

		if (NULL == (fi = pool_alloc(&fat_file_pool))) {
			return -ENOMEM;
		}
		if (NULL == (node = vfs_subtree_create_child(dir_nas->node, (const char *) name, mode))) {
			pool_free(&fat_file_pool, fi);
			return -ENOMEM;
		}

		memset(fi, 0, sizeof(struct fat_file_info));

		nas = node->nas;
		nas->fs = dir_nas->fs;
		nas->fi->privdata = (void *)fi;

		if (de.attr & ATTR_DIRECTORY) {
			fat_create_dir_entry(nas);
		}
	}
	return DFS_OK;
}

static int fat_create_dir_entry(struct nas *parent_nas) {
	struct dirinfo di;
	struct dirent de;
	char name[MSDOS_NAME + 2];
	char dir_path[PATH_MAX];
	char dir_buff[FAT_MAX_SECTOR_SIZE];
	struct nas *nas;
	struct fat_file_info *fi;
	struct node *node;
	mode_t mode;

	di.p_scratch = (uint8_t *) dir_buff;

	vfs_get_relative_path(parent_nas->node, dir_path, PATH_MAX);

	if (fat_open_dir(parent_nas->fs->fsi, (uint8_t *) dir_path, &di)) {
		return -ENODEV;
	}

	while (DFS_EOF != fat_get_next(parent_nas->fs->fsi, &di, &de)) {
		if (de.name[0] == 0)
			continue;

		path_dir_to_canonical(name, (char *) de.name, de.attr & ATTR_DIRECTORY);
		if ((0 == strncmp((char *) de.name, ".  ", 3)) ||
			(0 == strncmp((char *) de.name, ".. ", 3))) {
			continue;
		}

		if (NULL == (fi = pool_alloc(&fat_file_pool))) {
			return -ENOMEM;
		}

		mode = (de.attr & ATTR_DIRECTORY) ? S_IFDIR : S_IFREG;

		if (NULL == (node = vfs_subtree_create_child(parent_nas->node, name, mode))) {
			pool_free(&fat_file_pool, fi);
			return -ENOMEM;
		}

		memset(fi, 0, sizeof(*fi));

		nas = node->nas;
		nas->fs = parent_nas->fs;
		nas->fi->privdata = fi;

		if (de.attr & ATTR_DIRECTORY) {
			fat_create_dir_entry(nas);
		}
	}

	return ENOERR;
}

static void fat_free_fs(struct nas *nas) {
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;

	if (NULL != nas->fs) {
		fsi = nas->fs->fsi;

		if(NULL != fsi) {
			pool_free(&fat_fs_pool, fsi);
		}
		filesystem_free(nas->fs);
	}

	if (NULL != (fi = nas->fi->privdata)) {
		pool_free(&fat_file_pool, fi);
	}
}

static int fat_umount_entry(struct nas *nas) {
	struct node *child;

	if (node_is_directory(nas->node)) {
		while (NULL != (child = vfs_subtree_get_child_next(nas->node, NULL))) {
			if (node_is_directory(child)) {
				fat_umount_entry(child->nas);
			}

			pool_free(&fat_file_pool, child->nas->fi->privdata);
			vfs_del_leaf(child);
		}
	}

	return 0;
}

/* File operations */
static int    fatfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int    fatfs_close(struct file_desc *desc);
static size_t fatfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t fatfs_write(struct file_desc *desc, void *buf, size_t size);
static int    fatfs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations fatfs_fop = {
	.open = fatfs_open,
	.close = fatfs_close,
	.read = fatfs_read,
	.write = fatfs_write,
	.ioctl = fatfs_ioctl,
};

/*
 * file_operation
 */
static int fatfs_open(struct node *node, struct file_desc *desc,  int flag) {
	struct nas *nas;
	uint8_t path [PATH_MAX];
	struct fat_file_info *fi;

	nas = node->nas;
	fi = nas->fi->privdata;

	vfs_get_relative_path(node, (char *) path, PATH_MAX);

	if (DFS_OK == fat_open_file(nas, (uint8_t *)path, flag, fat_sector_buff)) {
		fi->pointer = desc->cursor;
		return 0;
	}

	return -1;
}

static int fatfs_close(struct file_desc *desc) {
	return 0;
}

static size_t fatfs_read(struct file_desc *desc, void *buf, size_t size) {
	size_t rezult;
	uint32_t bytecount;
	struct nas *nas;
	struct fat_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fi->pointer = desc->cursor;

	rezult = fat_read_file(nas, fat_sector_buff, buf, &bytecount, size);
	if (DFS_OK == rezult) {
		desc->cursor = fi->pointer;
		return bytecount;
	}
	return rezult;
}

static size_t fatfs_write(struct file_desc *desc, void *buf, size_t size) {
	size_t rezult;
	uint32_t bytecount;
	struct nas *nas;
	struct fat_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fi->pointer = desc->cursor;

	rezult = fat_write_file(nas, fat_sector_buff, (uint8_t *)buf,
			&bytecount, size);
	if (DFS_OK == rezult) {
		desc->cursor = fi->pointer;
		return bytecount;
	}
	return rezult;
}

static int fatfs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int fat_mount_files (struct nas *dir_nas);
static int fat_create_file(struct node *parent_node, struct node *new_node);
int fat_create_partition (void *bdev);
int fat_root_dir_record(void *bdev);
static int fat_unlike_file(struct nas *nas, uint8_t *path, uint8_t *scratch);
static int fat_unlike_directory(struct nas *nas, uint8_t *path, uint8_t *scratch);

/* File system operations */

static int fatfs_init(void * par);
static int fatfs_format(void * bdev);
static int fatfs_mount(void * dev, void *dir);
static int fatfs_create(struct node *parent_node, struct node *new_node);
static int fatfs_delete(struct node *node);
static int fatfs_truncate (struct node *node, off_t length);
static int fatfs_umount(void *dir);

static struct fsop_desc fatfs_fsop = {
	.init = fatfs_init,
	.format = fatfs_format,
	.mount = fatfs_mount,
	.create_node = fatfs_create,
	.delete_node = fatfs_delete,
	.truncate = fatfs_truncate,
	.umount = fatfs_umount,
};

static const struct fs_driver fatfs_driver = {
	.name = "vfat",
	.file_op = &fatfs_fop,
	.fsop = &fatfs_fsop,
};

static int fatfs_init(void * par) {
	return 0;
}

static int fatfs_format(void *dev) {
	struct node *dev_node;
	struct nas *dev_nas;

	if (NULL == (dev_node = dev)) {
		return -ENODEV;/*device not found*/
	}

	dev_nas = dev_node->nas;

	fat_create_partition(dev_nas->fi->privdata);
	fat_root_dir_record(dev_nas->fi->privdata);

	return 0;
}

static int fatfs_mount(void *dev, void *dir) {
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct node_fi *dev_fi;
	int rc;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		rc = -ENODEV;
		goto error;
	}

	if (NULL == (dir_nas->fs = filesystem_create("vfat"))) {
		rc = -ENOMEM;
		goto error;
	}

	dir_nas->fs->bdev = dev_fi->privdata;

	if (NULL == (fsi = pool_alloc(&fat_fs_pool))) {
		rc =  -ENOMEM;
		goto error;
	}
	memset(fsi, 0, sizeof(struct fat_fs_info));
	dir_nas->fs->fsi = fsi;

	/* allocate this directory info */
	if (NULL == (fi = pool_alloc(&fat_file_pool))) {
		rc =  -ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct fat_file_info));
	dir_nas->fi->privdata = (void *) fi;
	((struct fat_fs_info *) dir_nas->fs->fsi)->bdev = dir_nas->fs->bdev;
	((struct fat_fs_info *) dir_nas->fs->fsi)->root = dir_node;

	return fat_mount_files(dir_nas);

	error:
	fat_free_fs(dir_nas);

	return rc;
}

static int fatfs_create(struct node *parent_node, struct node *node) {
	struct nas *parent_nas, *nas;
	int rc;

	assert(parent_node && node);

	if (0 != (rc = fat_check_filename(node->name))) {
		return -rc;
	}

	nas = node->nas;
	parent_nas = parent_node->nas;

	if (NULL == fat_fi_alloc(nas, parent_nas->fs)) {
		return -ENOMEM;
	}

	if(0 != fat_create_file(parent_node, node)) {
		return -EIO;
	}

	return 0;
}

static int fatfs_delete(struct node *node) {
	struct nas *nas;
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	char path[PATH_MAX];
	int root_path_len;

	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	vfs_get_relative_path(fsi->root, path, PATH_MAX);
	root_path_len = strlen(path);
	vfs_get_relative_path(node, path, PATH_MAX);

	if (path[root_path_len] == '\0') {
		pool_free(&fat_fs_pool, fsi);
	} else {
		if (node_is_directory(node)) {
			if (fat_unlike_directory(nas, (uint8_t *) path + root_path_len,
				(uint8_t *) fat_sector_buff)) {
				return -1;
			}
		} else {
			if (fat_unlike_file(nas, (uint8_t *) path + root_path_len,
				(uint8_t *) fat_sector_buff)) {
				return -1;
			}
		}
	}
	pool_free(&fat_file_pool, fi);

	vfs_del_leaf(node);
	return 0;
}

static int fatfs_truncate(struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	/* TODO realloc blocks*/

	return 0;
}

static int fatfs_umount(void *dir) {
	struct node *dir_node;
	struct nas *dir_nas;

	dir_node = dir;
	dir_nas = dir_node->nas;

	fat_umount_entry(dir_nas);

	fat_free_fs(dir_nas);

	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(fatfs_driver);


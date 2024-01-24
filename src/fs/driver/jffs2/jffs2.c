/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Free Software Foundation, Inc.
 *
 * Created by Dominic Ostrowski <dominic.ostrowski@3glab.com>
 * Contributors: David Woodhouse, Nick Garnett, Richard Panton.
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: fs-ecos.c,v 1.44 2005/07/24 15:29:57 dedekind Exp $
 *
 */

#include <linux/kernel.h>
#include <linux/pagemap.h>
#include <linux/crc32.h>

#include "nodelist.h"
#include "compr.h"

#include <errno.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <fs/fs_driver.h>
#include <fs/super_block.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/hlpr_path.h>
#include <fs/file_desc.h>

#include <lib/libds/array.h>
#include <util/err.h>
#include <embox/unit.h>

#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <mem/sysmalloc.h>

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>
#include <drivers/flash/emulator.h>

#define FS_NAME "jffs2"

/* ext filesystem description pool */
POOL_DEF(jffs2_fs_pool, struct jffs2_fs_info,
		OPTION_GET(NUMBER,jffs2_descriptor_quantity));

/* ext file description pool */
POOL_DEF(jffs2_file_pool, struct jffs2_file_info,
		OPTION_GET(NUMBER,inode_quantity));

static int jffs2fs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *dir_ctx);

static int jffs2_free_fs(struct super_block *sb);
static int jffs2_read_inode (struct _inode *inode);
static void jffs2_clear_inode (struct _inode *inode);

static int jffs2_truncate_file (struct _inode *inode);
static unsigned char gc_buffer[PAGE_CACHE_SIZE];	/*avoids kmalloc when user may be under memory pressure */

/***********************
 * Directory operations
 */
struct jffs2_dirsearch {
	struct _inode *dir;	    	/* directory to search */
	const unsigned char *path;  /* path to follow */
	struct _inode *node;	    /* Node found */
	const unsigned char *name;  /* last name fragment used */
	int namelen;		    	/* name fragment length */
	bool last;		    		/* last name in path? */
};

typedef struct jffs2_dirsearch jffs2_dirsearch_t;

/**
 * Ref count and nlink management
 */

/**
 * FIXME: This seems like real cruft. Wouldn't it be better just to do the
 * right thing?
 */
static void icache_evict(struct _inode *root_i, struct _inode *i) {
	struct _inode *this = root_i, *next;
	struct _inode *parent;

 restart:
	D2(printf("icache_evict\n"));
	/* If this is an absolute search path from the root,
	 * remove all cached inodes with i_count of zero (these are only
	 * held where needed for dotdot filepaths)
	 */
	while (this) {
		next = this->i_cache_next;
		if (this != i && this->i_count == 0) {
			parent = this->i_parent;
			if (this->i_cache_next) {
				this->i_cache_next->i_cache_prev = this->i_cache_prev;
			}
			if (this->i_cache_prev) {
				this->i_cache_prev->i_cache_next = this->i_cache_next;
			}
			jffs2_clear_inode(this);
			memset(this, 0x5a, sizeof(*this));
			sysfree(this);
			if (parent && parent != this) {
				parent->i_count--;
				this = root_i;
				goto restart;
			}
		}
		this = next;
	}
}

/******************************
 * Directory search
 */

/**
 * Initialize a dirsearch object to start a search
 */
static void init_dirsearch(jffs2_dirsearch_t * ds,
			   struct _inode *dir, const unsigned char *name) {
	D2(printf("init_dirsearch name = %s\n", name));
	D2(printf("init_dirsearch dir = %#x\n",(unsigned int) dir));

	dir->i_count++;
	ds->dir = dir;
	ds->path = name;
	ds->node = dir;
	ds->name = name;
	ds->namelen = 0;
	ds->last = false;
}

/**
 * Search a single directory for the next name in a path and update the
 * dirsearch object appropriately.
 */
static int find_entry(jffs2_dirsearch_t * ds) {
	struct _inode *dir = ds->dir;
	const unsigned char *name = ds->path;
	const unsigned char *n = name;
	char namelen = 0;
	struct _inode *d;

	D2(printf("find_entry\n"));

	/* check that we really have a directory */
	if (!S_ISDIR(dir->i_mode)) {
		return ENOTDIR;
	}
	/* Isolate the next element of the path name. */
	while (*n != '\0' && *n != '/') {
		n++, namelen++;
	}
	/* Check if this is the last path element. */
	while( *n == '/') {
		n++;
	}
	if (*n == '\0') {
		ds->last = true;
	}

	/* update name in dirsearch object */
	ds->name = name;
	ds->namelen = namelen;

	if (name[0] == '.') {
		switch (namelen) {
		default:
			break;
		case 2:
			/* Dot followed by not Dot, treat as any other name */
			if (name[1] != '.')
				break;
			/* Dot Dot
			 * Move back up the search path
			 */
			D2(printf("find_entry found ..\n"));
			ds->dir = ds->node;
			ds->node = ds->dir->i_parent;
			ds->node->i_count++;
			return ENOERR;
		case 1:
			/* Dot is consumed */
			D2(printf("find_entry found .\n"));
			ds->node = ds->dir;
			ds->dir->i_count++;
			return ENOERR;
		}
	}

	/* Here we have the name and its length set up. */
	/* Search the directory for a matching entry */

	D2(printf("find_entry for name = %s\n", ds->path));
	d = jffs2_lookup(dir, name, namelen);
	D2(printf("find_entry got dir = %#x\n",(unsigned int) d));

	if (d == NULL) {
		return ENOENT;
	}
	if (IS_ERR(d)) {
		return -PTR_ERR(d);
	}

	/* If it's a new directory inode, increase refcount on its parent */
	if (S_ISDIR(d->i_mode) && !d->i_parent) {
		d->i_parent = dir;
		dir->i_count++;
	}

	/* pass back the node we have found */
	ds->node = d;
	return ENOERR;
}

/**
 * Main interface to directory search code. This is used in all file
 * level operations to locate the object named by the pathname.
 *
 * Returns with use count incremented on both the sought object and
 * the directory it was found in
 */
static int jffs2_find(jffs2_dirsearch_t * d) {
	int err;

	D2(printf("jffs2_find for path =%s\n", d->path));

	/* Short circuit empty paths */
	if (*(d->path) == '\0') {
		d->node->i_count++;
		return ENOERR;
	}
	/* skip dirname separators */
	while (*(d->path) == '/') {
		d->path++;
	}

	/* iterate down directory tree until we find the object we want. */
	for (;;) {
		err = find_entry(d);

		if (err != ENOERR) {
			return err;
		}
		if (d->last) {
			return ENOERR;
		}
		/* We're done with it, although it we found a subdir that
		 * will have caused the refcount to have been increased
		 */
		jffs2_iput(d->dir);

		/* Update dirsearch object to search next directory. */
		d->dir = d->node;
		d->path += d->namelen;
		while (*(d->path) == '/') {
			d->path++;	/* skip dirname separators */
		}
	}
}

/**
 * Process a mount request. This mainly creates a root for the
 * filesystem.
 */
static int jffs2_read_super(struct jffs2_super_block *sb) {
	struct jffs2_sb_info *c;
	int err;

	D1(printk( "jffs2: read_super\n"));

	c = &sb->jffs2_sb;

	c->flash_size = sb->bdev->size < 4096 ? 4096 : sb->bdev->size;
	c->sector_size = sb->bdev->block_size < 4096 ? 4096 : sb->bdev->size;
	/* Number 4096 is used here beacuse actually there are no real flash
	 * drives with less than 4KiB erasable block. But if device is provided
	 * with QEMU, than it's just a block device with 512 bytes block size.
	 * So, this is generally just for continious testing of OS. If JFFS2
	 * will detect block or device size incorrectly, then this device was
	 * detected in wrong way on probe. Changing these values will not fix
	 * such issue.*/

	c->cleanmarker_size = sizeof(struct jffs2_unknown_node);

	err = jffs2_do_mount_fs(c);
	if (err) {
		return -err;
	}
	D1(printk( "jffs2_read_super(): Getting root inode\n"));
	sb->s_root = jffs2_iget(sb, 1);
	if (IS_ERR(sb->s_root)) {
		D1(printk(KERN_WARNING "get root inode failed\n"));
		err = PTR_ERR(sb->s_root);
		sb->s_root = NULL;
		goto out_nodes;
	}

	return 0;

    out_nodes:
	jffs2_free_ino_caches(c);
	jffs2_free_raw_node_refs(c);
	sysfree(c->blocks);

	return err;
}

static int jffs2_mount(struct nas *dir_nas) {
	struct jffs2_super_block *jffs2_sb;
	struct jffs2_sb_info *c;
	int err;

	struct jffs2_fs_info *fsi;

	fsi = dir_nas->fs->sb_data;

	jffs2_sb = &fsi->jffs2_sb;

	if (jffs2_sb == NULL) {
		return ENOMEM;
	}

	c = &jffs2_sb->jffs2_sb;
	memset(jffs2_sb, 0, sizeof (struct jffs2_super_block));

	jffs2_sb->bdev = dir_nas->fs->bdev;

	c->inocache_list = sysmalloc(sizeof(struct jffs2_inode_cache *) * INOCACHE_HASHSIZE);
	if (!c->inocache_list) {
		return ENOMEM;
	}
	memset(c->inocache_list, 0, sizeof(struct jffs2_inode_cache *) * INOCACHE_HASHSIZE);

	spin_init(&c->inocache_lock, __SPIN_UNLOCKED);
	spin_init(&c->erase_completion_lock, __SPIN_UNLOCKED);

	jffs2_compressors_init();

	err = jffs2_read_super(jffs2_sb);

	if (err) {
		jffs2_compressors_exit();

		sysfree(c->inocache_list);
		return err;
	}

	jffs2_sb->s_root->i_parent = jffs2_sb->s_root;	/* points to itself, no dotdot paths above mountpoint */
	jffs2_sb->s_root->i_cache_prev = NULL;	/* root inode, so always null */
	jffs2_sb->s_root->i_cache_next = NULL;
	jffs2_sb->s_root->i_count = 1;	/* Ensures the root inode is always in ram until umount */

	D2(printf("jffs2_mount erasing pending blocks\n"));
	jffs2_erase_pending_blocks(c, 0);

#ifdef CYGOPT_FS_JFFS2_GCTHREAD
	jffs2_start_garbage_collect_thread(c);
#endif

	D2(printf("jffs2_mounted superblock"));

	return ENOERR;
}

static int jffs2fs_umount_entry(struct inode *node) {
	pool_free(&jffs2_file_pool, inode_priv(node));

	return 0;
}

/**
 * Unmount the filesystem.
 */
static int jffs2_clean_sb(struct super_block *sb) {
	struct _inode *root;
	struct jffs2_super_block *jffs2_sb;
	struct jffs2_fs_info *fsi;
	struct jffs2_sb_info *c;
	struct jffs2_full_dirent *fd, *next;

	fsi = sb->sb_data;
	jffs2_sb = &fsi->jffs2_sb;
	root = jffs2_sb->s_root;
	c = &jffs2_sb->jffs2_sb;

	D2(printf("jffs2_umount\n"));

	/* Only really umount if this is the only mount */
	icache_evict(root, NULL);

	if (root->i_count != 1) {
		printf("Ino #1 has use count %d\n", root->i_count);
		return EBUSY;
	}
#ifdef CYGOPT_FS_JFFS2_GCTHREAD
	jffs2_stop_garbage_collect_thread(c);
#endif
	jffs2_iput(root);	/* Time to free the root inode */

	/* free directory entries */
	for (fd = root->jffs2_i.dents; fd; fd = next) {
		next=fd->next;
		jffs2_free_full_dirent(fd);
	}

	sysfree(root);

	/* Clean up the super block and root inode */
	jffs2_free_ino_caches(c);
	jffs2_free_raw_node_refs(c);
	sysfree(c->blocks);
	sysfree(c->inocache_list);

	D2(printf("jffs2_umount No current mounts\n"));

	jffs2_compressors_exit();

	return jffs2_free_fs(sb);
}

/**
 * Open a file for reading or writing.
 */
static int jffs2_open(struct _inode *dir_ino, const char *name, int mode) {
	jffs2_dirsearch_t ds;
	struct _inode *node = NULL;
	int err;

	D2(printf("jffs2_open\n"));

	if (!dir_ino) {
		return ENOENT;
	}

	init_dirsearch(&ds, (struct _inode *) dir_ino,
                       (const unsigned char *) name);

	err = jffs2_find(&ds);

	if (err == ENOENT) {
		if (ds.last && (mode & O_CREAT)) {
		/* No node there, if the O_CREAT bit is set then we must
		 * create a new one. The dir and name fields of the dirsearch
		 * object will have been updated so we know where to put it.
		 */
			err = jffs2_create(ds.dir, ds.name,
					S_IRUGO|S_IXUGO|S_IWUSR|S_IFREG, &node);
			if (err != 0) {
				/*Possible orphaned inode on the flash - but will be gc'd */
				jffs2_iput(ds.dir);
				return -err;
			}
			err = ENOERR;
		}
	} else if (err == ENOERR) {
		/**
		 * The node exists. If the O_CREAT and O_EXCL bits are set, we
		 * must fail the open.
		 */
		if ((mode & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
			jffs2_iput(ds.node);
			err = EEXIST;
		} else {
			node = ds.node;
		}
	}

	/* Finished with the directory now */
	jffs2_iput(ds.dir);

	if (err != ENOERR) {
		return err;
	}
	/* Check that we actually have a file here */
	if (S_ISDIR(node->i_mode)) {
		jffs2_iput(node);
		return EISDIR;
	}
    /* If the O_TRUNC bit is set we must clean out the file data. */
	if (mode & O_TRUNC) {
		err = jffs2_truncate_file(node);
		if (err) {
			jffs2_iput(node);
			return err;
		}
	}
	return ENOERR;
}

/**
 * Remove a file link from its directory.
 */
static int jffs2_ops_unlink(struct _inode * dir, const char *name) {
	struct jffs2_dirsearch ds;
	int err;

	D2(printf("jffs2_ops_unlink\n"));

	init_dirsearch(&ds, (struct _inode *) dir,
                       (const unsigned char *)name);

	err = jffs2_find(&ds);

	if (err != ENOERR) {
		jffs2_iput(ds.dir);
		return err;
	}

	/*  Cannot unlink directories, use rmdir() instead */
	if (S_ISDIR(ds.node->i_mode)) {
		jffs2_iput(ds.dir);
		jffs2_iput(ds.node);
		return EPERM;
	}

	/* Delete it from its directory */
	err = jffs2_unlink(ds.dir, ds.node, ds.name);
	jffs2_iput(ds.dir);
	jffs2_iput(ds.node);

	return -err;
}

/**
 *  Create a new directory.
 */
static int jffs2_ops_mkdir(struct _inode *dir, const char *name, mode_t mode) {
	jffs2_dirsearch_t ds;
	int err;

	D2(printf("jffs2_ops_mkdir\n"));

	init_dirsearch(&ds, (struct _inode *) dir,
                       (const unsigned char *)name);

	err = jffs2_find(&ds);

	if (err == ENOENT) {
		if (ds.last) {
			/* The entry does not exist, and it is the last element in
			 * the pathname, so we can create it here.
			 */
			err = -jffs2_mkdir(ds.dir, ds.name, mode); /*S_IRUGO|S_IXUGO|S_IWUSR); */
		}
		/* If this was not the last element, then an intermediate
		 * directory does not exist.
		 */
	} else {
		/* If there we no error, something already exists with that
		 * name, so we cannot create another one.
		 */
		if (err == ENOERR) {
			jffs2_iput(ds.node);
			err = EEXIST;
		}
	}
	jffs2_iput(ds.dir);
	return err;
}

/**
 *  Remove a directory.
 */
static int jffs2_ops_rmdir(struct _inode * dir, const char *name) {
	struct jffs2_dirsearch ds;
	int err;

	D2(printf("jffs2_ops_rmdir\n"));

	init_dirsearch(&ds, (struct _inode *) dir,
                       (const unsigned char *)name);

	err = jffs2_find(&ds);

	if (err != ENOERR) {
		jffs2_iput(ds.dir);
		return err;
	}

	/* Check that this is actually a directory. */
	if (!S_ISDIR(ds.node->i_mode)) {
		jffs2_iput(ds.dir);
		jffs2_iput(ds.node);
		return EPERM;
	}

	err = jffs2_rmdir(ds.dir, ds.node, ds.name);

	jffs2_iput(ds.dir);
	jffs2_iput(ds.node);
	return -err;
}

/*
// -------------------------------------------------------------------------
// jffs2_ops_rename()
// Rename a file/dir.
static int jffs2_ops_rename(cyg_mtab_entry * mte, cyg_dir dir1,
			    const char *name1, cyg_dir dir2, const char *name2)
{
	jffs2_dirsearch ds1, ds2;
	int err;

	D2(printf("jffs2_ops_rename\n"));

	init_dirsearch(&ds1, (struct _inode *) dir1,
                       (const unsigned char *)name1);

	err = jffs2_find(&ds1);

	if (err != ENOERR) {
		jffs2_iput(ds1.dir);
		return err;
	}

	init_dirsearch(&ds2, (struct _inode *) dir2,
                       (const unsigned char *)name2);

	err = jffs2_find(&ds2);

	// Allow through renames to non-existent objects.
	if (ds2.last && err == ENOENT) {
		ds2.node = NULL;
		err = ENOERR;
	}

	if (err != ENOERR) {
		jffs2_iput(ds1.dir);
		jffs2_iput(ds1.node);
		jffs2_iput(ds2.dir);
		return err;
	}

	// Null rename, just return
	if (ds1.node == ds2.node) {
		err = ENOERR;
		goto out;
	}

	// First deal with any entry that is at the destination
	if (ds2.node) {
		// Check that we are renaming like-for-like

		if (!S_ISDIR(ds1.node->i_mode) && S_ISDIR(ds2.node->i_mode)) {
			err = EISDIR;
			goto out;
		}

		if (S_ISDIR(ds1.node->i_mode) && !S_ISDIR(ds2.node->i_mode)) {
			err = ENOTDIR;
			goto out;
		}

		// Now delete the destination directory entry
		// Er, what happened to atomicity of rename()?
		err = -jffs2_unlink(ds2.dir, ds2.node, ds2.name);

		if (err != 0)
			goto out;

	}
	// Now we know that there is no clashing node at the destination,
	// make a new direntry at the destination and delete the old entry
	// at the source.

	err = -jffs2_rename(ds1.dir, ds1.node, ds1.name, ds2.dir, ds2.name);

	// Update directory times
	if (!err)
		ds1.dir->i_ctime =
		    ds1.dir->i_mtime =
		    ds2.dir->i_ctime = ds2.dir->i_mtime = cyg_timestamp();
 out:
	jffs2_iput(ds1.dir);
	if (S_ISDIR(ds1.node->i_mode)) {
//		   Renamed a directory to elsewhere... so fix up its
//		   i_parent pointer and the i_counts of its old and
//		   new parents.
		jffs2_iput(ds1.node->i_parent);
		ds1.node->i_parent = ds2.dir;
		// We effectively increase its use count by not...
	} else {
		jffs2_iput(ds2.dir); // ... doing this
	}
	jffs2_iput(ds1.node);
	if (ds2.node)
		jffs2_iput(ds2.node);

	return err;
}
*/

/**
 * Write data to file.
 */
static int jffs2_extend_file (struct _inode *inode,
		struct jffs2_raw_inode *ri, unsigned long offset) {
	struct jffs2_sb_info *c;
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
	struct jffs2_full_dnode *fn;
	uint32_t phys_ofs, alloc_len;
	int ret = 0;

	c = &inode->i_sb->jffs2_sb;

	/* Make new hole frag from old EOF to new page */
	D1(printk( "Writing new hole frag %#x-%#x between current EOF and new page\n",
		  (unsigned int)inode->i_size, (unsigned int)offset));

	ret = jffs2_reserve_space(c, sizeof(*ri),
			&phys_ofs, &alloc_len, ALLOC_NORMAL);
	if (ret) {
		return ret;
	}
	down(&f->sem);

	ri->magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
	ri->nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
	ri->totlen = cpu_to_je32(sizeof(*ri));
	ri->hdr_crc = cpu_to_je32(crc32(0, ri,
			sizeof(struct jffs2_unknown_node)-4));

	ri->version = cpu_to_je32(++f->highest_version);
	ri->isize = cpu_to_je32(max((uint32_t)inode->i_size, offset));

	ri->offset = cpu_to_je32(inode->i_size);
	ri->dsize = cpu_to_je32(offset - inode->i_size);
	ri->csize = cpu_to_je32(0);
	ri->compr = JFFS2_COMPR_ZERO;
	ri->node_crc = cpu_to_je32(crc32(0, ri, sizeof(*ri)-8));
	ri->data_crc = cpu_to_je32(0);

	fn = jffs2_write_dnode(c, f, ri, NULL, 0, phys_ofs, ALLOC_NORMAL);
	jffs2_complete_reservation(c);
	if (IS_ERR(fn)) {
		ret = PTR_ERR(fn);
		up(&f->sem);
		return ret;
	}
	ret = jffs2_add_full_dnode_to_inode(c, f, fn);
	if (f->metadata) {
		jffs2_mark_node_obsolete(c, f->metadata->raw);
		jffs2_free_full_dnode(f->metadata);
		f->metadata = NULL;
	}
	if (ret) {
		D1(printk( "Eep. add_full_dnode_to_inode() failed in prepare_write, returned %d\n", ret));
		jffs2_mark_node_obsolete(c, fn->raw);
		jffs2_free_full_dnode(fn);
		up(&f->sem);
		return ret;
	}
	inode->i_size = offset;
	up(&f->sem);
	return 0;
}

/**
 * Truncate a file
 */
static int jffs2_truncate_file (struct _inode *inode) {
     struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);
     struct jffs2_sb_info *c;
     struct jffs2_full_dnode *new_metadata, * old_metadata;
     struct jffs2_raw_inode *ri;
     uint32_t phys_ofs, alloclen;
     int err;

     ri = jffs2_alloc_raw_inode();
     if (!ri) {
          return ENOMEM;
     }
     c = &inode->i_sb->jffs2_sb;
     err = jffs2_reserve_space(c, sizeof(*ri), &phys_ofs, &alloclen, ALLOC_NORMAL);

     if (err) {
          jffs2_free_raw_inode(ri);
          return err;
     }
     down(&f->sem);
     ri->magic = cpu_to_je16(JFFS2_MAGIC_BITMASK);
     ri->nodetype = cpu_to_je16(JFFS2_NODETYPE_INODE);
     ri->totlen = cpu_to_je32(sizeof(*ri));
     ri->hdr_crc = cpu_to_je32(crc32(0, ri, sizeof(struct jffs2_unknown_node)-4));

     ri->ino = cpu_to_je32(inode->i_ino);
     ri->version = cpu_to_je32(++f->highest_version);

     ri->uid = cpu_to_je16(inode->i_uid);
     ri->gid = cpu_to_je16(inode->i_gid);
     ri->mode = cpu_to_jemode(inode->i_mode);
     ri->isize = cpu_to_je32(0);
     ri->atime = cpu_to_je32(inode->i_atime);
     ri->mtime = cpu_to_je32(timestamp());
     ri->offset = cpu_to_je32(0);
     ri->csize = ri->dsize = cpu_to_je32(0);
     ri->compr = JFFS2_COMPR_NONE;
     ri->node_crc = cpu_to_je32(crc32(0, ri, sizeof(*ri)-8));
     ri->data_crc = cpu_to_je32(0);
     new_metadata = jffs2_write_dnode(c, f, ri, NULL, 0,
                                      phys_ofs, ALLOC_NORMAL);
	if (IS_ERR(new_metadata)) {
		jffs2_complete_reservation(c);
		jffs2_free_raw_inode(ri);
		up(&f->sem);
		return PTR_ERR(new_metadata);
	}

     /* It worked. Update the inode */
     inode->i_mtime = timestamp();
     inode->i_size = 0;
     old_metadata = f->metadata;
     jffs2_truncate_fragtree (c, &f->fragtree, 0);
     f->metadata = new_metadata;
     if (old_metadata) {
          jffs2_mark_node_obsolete(c, old_metadata->raw);
          jffs2_free_full_dnode(old_metadata);
     }
     jffs2_free_raw_inode(ri);

     up(&f->sem);
     jffs2_complete_reservation(c);

     return 0;
}

static int jffs2_fo_write(struct file_desc *desc, char *buf, ssize_t size) {
	struct _inode *inode;
	off_t pos = file_get_pos(desc);
	ssize_t resid = size;
	struct jffs2_raw_inode ri;
	struct jffs2_inode_info *f;
	struct jffs2_sb_info *c;
	struct jffs2_file_info *fi;

	uint32_t writtenlen;
	int err;
	off_t len;

	fi = inode_priv(desc->f_inode);

	inode = fi->_inode;
	f = JFFS2_INODE_INFO(inode);
	c = &inode->i_sb->jffs2_sb;

	if (pos < 0) {
		SET_ERRNO(EINVAL);
		return 0;
	}

	memset(&ri, 0, sizeof(ri));

	ri.ino = cpu_to_je32(f->inocache->ino);
	ri.mode = cpu_to_jemode(inode->i_mode);
	ri.uid = cpu_to_je16(inode->i_uid);
	ri.gid = cpu_to_je16(inode->i_gid);
	ri.atime = ri.ctime = ri.mtime = cpu_to_je32(timestamp());

	if (pos > inode->i_size) {
		ri.version = cpu_to_je32(++f->highest_version);
		err = jffs2_extend_file(inode, &ri, pos);
		if (err) {
			SET_ERRNO(-err);
			return 0;
		}

	}
	ri.isize = cpu_to_je32(inode->i_size);

	len = size;

	D2(printf("jffs2_fo_write page_start_pos %d\n", pos));
	D2(printf("jffs2_fo_write transfer size %d\n", len));

	err = jffs2_write_inode_range(c, f, &ri, (unsigned char *)buf,
					  pos, len, &writtenlen);

	if (err) {
		SET_ERRNO(-err);
		return 0;
	}

	if (writtenlen != len) {
		SET_ERRNO(ENOSPC);
		return 0;
	}

	pos += len;
	resid -= len;

	/* We wrote some data successfully, update the modified and access
	 * times of the inode, increase its size appropriately, and update
	 * the file offset and transfer residue.
	 */
	inode->i_mtime = inode->i_ctime = je32_to_cpu(ri.mtime);
	if (pos > inode->i_size) {
		inode->i_size = pos;
	}

	return writtenlen;
}

/**
 * Close a file. We just decrement the refcnt and let it go away if
 * that is all that is keeping it here.
 */
static int jffs2_fo_close(struct _inode *node) {
	D2(printf("jffs2_fo_close\n"));

	jffs2_iput(node);

	return ENOERR;
}

unsigned char *jffs2_gc_fetch_page(struct jffs2_sb_info *c,
	struct jffs2_inode_info *f, unsigned long offset, unsigned long *priv) {
	/* FIXME: This works only with one file system mounted at a time */
	int ret;

	ret = jffs2_read_inode_range(c, f, gc_buffer,
				     offset & ~(PAGE_CACHE_SIZE-1), PAGE_CACHE_SIZE);
	if (ret) {
		return ERR_PTR(ret);
	}

	return gc_buffer;
}

void jffs2_gc_release_page(struct jffs2_sb_info *c,
			   unsigned char *ptr, unsigned long *priv) {
	/* Do nothing */
}

static struct _inode *new_inode(struct jffs2_super_block *sb) {
	/* Only called in write.c jffs2_new_inode
	 * Always adds itself to inode cache	 */

	struct _inode *inode;
	struct _inode *cached_inode;

	inode = sysmalloc(sizeof (struct _inode));
	if (inode == NULL) {
		return 0;
	}

	D2(printf
	   ("malloc new_inode %x ####################################\n",
	    (unsigned int)inode));

	memset(inode, 0, sizeof (struct _inode));
	inode->i_sb = sb;
	inode->i_ino = 1;
	inode->i_count = 1;
	inode->i_nlink = 1;	/* Let JFFS2 manage the link count */
	inode->i_size = 0;

	inode->i_cache_next = NULL;	/* Newest inode, about to be cached */

	/* Add to the icache */
	for (cached_inode = sb->s_root; cached_inode != NULL;
	     cached_inode = cached_inode->i_cache_next) {
		if (cached_inode->i_cache_next == NULL) {
			cached_inode->i_cache_next = inode;	/* Current last in cache points to newcomer */
			inode->i_cache_prev = cached_inode;	/* Newcomer points back to last */
			break;
		}
	}
	return inode;
}

static struct _inode *ilookup(struct jffs2_super_block *sb, uint32_t ino)
{
	struct _inode *inode = NULL;

	D2(printf("ilookup\n"));
	/* Check for this inode in the cache */
	for (inode = sb->s_root; inode != NULL; inode = inode->i_cache_next) {
		if (inode->i_ino == ino) {
			inode->i_count++;
			break;
		}
	}
	return inode;
}

struct _inode *jffs2_iget(struct jffs2_super_block *sb, uint32_t ino) {
	/* Must first check for cached inode
	 * If this fails let new_inode create one
	 */
	struct _inode *inode;
	int err;

	D2(printf("jffs2_iget\n"));

	inode = ilookup(sb, ino);
	if (inode) {
		return inode;
	}

	/* Not cached, so kmalloc it */
	inode = new_inode(sb);
	if (inode == NULL) {
		return ERR_PTR(-ENOMEM);
	}

	inode->i_ino = ino;

	err = jffs2_read_inode(inode);
	if (err) {
		printf("jffs2_read_inode() failed\n");
                inode->i_nlink = 0; /* free _this_ bad inode right now */
		jffs2_iput(inode);
		inode = NULL;
		return ERR_PTR(err);
	}
	return inode;
}

/**
 * Decrement the reference count on an inode. If this makes the ref count
 * zero, then this inode can be freed.
 */
void jffs2_iput(struct _inode *i) {
	struct _inode *parent;

 recurse:
	if (!i) {
		printf("jffs2_iput() called with NULL inode\n");
		/* and let it fault... */
	}

	i->i_count--;

	if (i->i_count < 0) {
		BUG();
	}

	if (i->i_count) {
		return;
	}

	if (!i->i_nlink) {
		/* Remove from the icache linked list and free immediately */
		if (i->i_cache_prev) {
			i->i_cache_prev->i_cache_next = i->i_cache_next;
		}
		if (i->i_cache_next) {
			i->i_cache_next->i_cache_prev = i->i_cache_prev;
		}

		parent = i->i_parent;
		jffs2_clear_inode(i);
		memset(i, 0x5a, sizeof(*i));
		sysfree(i);

		if (parent && parent != i) {
			i = parent;
			goto recurse;
		}

	} else {
	/* Evict some _other_ inode with i_count zero, leaving
	 * this latest one in the cache for a while
	 */
		icache_evict(i->i_sb->s_root, i);
	}
}

static inline void jffs2_init_inode_info(struct jffs2_inode_info *f) {
	memset(f, 0, sizeof(*f));
	init_MUTEX_LOCKED(&f->sem);
}

static void jffs2_clear_inode (struct _inode *inode) {
	/* We can forget about this inode for now - drop all
	*  the nodelists associated with it, etc.
	*/
	struct jffs2_sb_info *c;// = JFFS2_SB_INFO(inode->i_sb);
	struct jffs2_inode_info *f = JFFS2_INODE_INFO(inode);

	c = &inode->i_sb->jffs2_sb;

	D1(printk( "jffs2_clear_inode(): ino #%u mode %o\n", inode->i_ino, inode->i_mode));

	jffs2_do_clear_inode(c, f);
}


/**
 * allocate a new inode and inocache, add it to the hash,
 * fill in the raw_inode while you're at it.
 */
struct _inode *jffs2_new_inode (struct _inode *dir_i,
					int mode, struct jffs2_raw_inode *ri) {
	struct _inode *inode;
	struct jffs2_super_block *sb = dir_i->i_sb;
	struct jffs2_sb_info *c;
	struct jffs2_inode_info *f;
	int ret;

	D1(printk( "jffs2_new_inode(): dir_i %d, mode 0x%x\n", dir_i->i_ino, i_mode));

	c = &sb->jffs2_sb;

	inode = new_inode(sb);

	if (!inode) {
		return ERR_PTR(-ENOMEM);
	}

	f = JFFS2_INODE_INFO(inode);
	jffs2_init_inode_info(f);

	memset(ri, 0, sizeof(*ri));
	/* Set OS-specific defaults for new inodes */
	ri->uid = ri->gid = cpu_to_je16(0);
	ri->mode =  cpu_to_jemode(mode);
	ret = jffs2_do_new_inode (c, f, mode, ri);
	if (ret) {
	/* forceful evict: f->sem is locked already, and the
	 * inode is bad.
	 */
		if (inode->i_cache_prev) {
			inode->i_cache_prev->i_cache_next = inode->i_cache_next;
		}
		if (inode->i_cache_next) {
			inode->i_cache_next->i_cache_prev = inode->i_cache_prev;
		}
		up(&(f->sem));
		jffs2_clear_inode(inode);
		memset(inode, 0x6a, sizeof(*inode));
		sysfree(inode);
		return ERR_PTR(ret);
	}
	inode->i_nlink = 1;
	inode->i_ino = je32_to_cpu(ri->ino);
	inode->i_mode = jemode_to_cpu(ri->mode);
	inode->i_gid = je16_to_cpu(ri->gid);
	inode->i_uid = je16_to_cpu(ri->uid);
	inode->i_atime = inode->i_ctime = inode->i_mtime = timestamp();
	ri->atime = ri->mtime = ri->ctime = cpu_to_je32(inode->i_mtime);

	inode->i_size = 0;

	return inode;
}


static int jffs2_read_inode (struct _inode *inode) {
	struct jffs2_inode_info *f;
	struct jffs2_sb_info *c;
	struct jffs2_raw_inode latest_node;
	int ret;

	D1(printk( "jffs2_read_inode(): inode->i_ino == %d\n", inode->i_ino));

	f = JFFS2_INODE_INFO(inode);
	c = &inode->i_sb->jffs2_sb;

	jffs2_init_inode_info(f);

	ret = jffs2_do_read_inode(c, f, inode->i_ino, &latest_node);

	if (ret) {
		up(&f->sem);
		return ret;
	}
	inode->i_mode = jemode_to_cpu(latest_node.mode);
	inode->i_uid = je16_to_cpu(latest_node.uid);
	inode->i_gid = je16_to_cpu(latest_node.gid);
	inode->i_size = je32_to_cpu(latest_node.isize);
	inode->i_atime = je32_to_cpu(latest_node.atime);
	inode->i_mtime = je32_to_cpu(latest_node.mtime);
	inode->i_ctime = je32_to_cpu(latest_node.ctime);

	inode->i_nlink = f->inocache->nlink;
	up(&f->sem);

	D1(printk( "jffs2_read_inode() returning\n"));
	return 0;
}


void jffs2_gc_release_inode(struct jffs2_sb_info *c,
				   struct jffs2_inode_info *f) {
	jffs2_iput(OFNI_EDONI_2SFFJ(f));
}

struct jffs2_inode_info *jffs2_gc_fetch_inode(struct jffs2_sb_info *c,
						     int inum, int nlink) {
	struct _inode *inode;
	struct jffs2_inode_cache *ic;
	struct jffs2_super_block *sb;

	sb = member_cast_out(c, struct jffs2_super_block, jffs2_sb);

	if (!nlink) {
	/* The inode has zero nlink but its nodes weren't yet marked
	 * obsolete. This has to be because we're still waiting for
	 * the final (close() and) jffs2_iput() to happen.
	 *
	 * There's a possibility that the final jffs2_iput() could have
	 * happened while we were contemplating. In order to ensure
	 * that we don't cause a new read_inode() (which would fail)
	 * for the inode in question, we use ilookup() in this case
	 * instead of jffs2_iget().
	 *
	 * The nlink can't _become_ zero at this point because we're
	 * holding the alloc_sem, and jffs2_do_unlink() would also
	 * need that while decrementing nlink on any inode.
	 */
		inode = ilookup(sb, inum);
		if (!inode) {
			D1(printk( "ilookup() failed for ino #%u; inode is probably deleted.\n",
				  inum));

			spin_lock(&c->inocache_lock);
			ic = jffs2_get_ino_cache(c, inum);
			if (!ic) {
				D1(printk( "Inode cache for ino #%u is gone.\n", inum));
				spin_unlock(&c->inocache_lock);
				return NULL;
			}
			if (ic->state != INO_STATE_CHECKEDABSENT) {
				/* Wait for progress. Don't just loop */
				D1(printk( "Waiting for ino #%u in state %d\n",
					  ic->ino, ic->state));
				sleep_on_spinunlock(&c->inocache_wq, &c->inocache_lock);
			} else {
				spin_unlock(&c->inocache_lock);
			}

			return NULL;
		}
	} else {
		/* Inode has links to it still; they're not going away because
		 * jffs2_do_unlink() would need the alloc_sem and we have it.
		 * Just jffs2_iget() it, and if read_inode() is necessary that's OK.
		 */
		inode = jffs2_iget(sb, inum);
		if (IS_ERR(inode)) {
			return (void *)inode;
		}
	}

	return JFFS2_INODE_INFO(inode);
}

uint32_t jffs2_from_os_mode(uint32_t osmode) {
	uint32_t jmode = ((osmode & S_IRUSR)?00400:0) |
		((osmode & S_IWUSR)?00200:0) |
		((osmode & S_IXUSR)?00100:0) |
		((osmode & S_IRGRP)?00040:0) |
		((osmode & S_IWGRP)?00020:0) |
		((osmode & S_IXGRP)?00010:0) |
		((osmode & S_IROTH)?00004:0) |
		((osmode & S_IWOTH)?00002:0) |
		((osmode & S_IXOTH)?00001:0);

	switch (osmode & S_IFMT) {
	case S_IFSOCK:
		return jmode | 0140000;
	case S_IFLNK:
		return jmode | 0120000;
	case S_IFREG:
		return jmode | 0100000;
	case S_IFBLK:
		return jmode | 0060000;
	case S_IFDIR:
		return jmode | 0040000;
	case S_IFCHR:
		return jmode | 0020000;
	case S_IFIFO:
		return jmode | 0010000;
	case S_ISUID:
		return jmode | 0004000;
	case S_ISGID:
		return jmode | 0002000;
#ifdef S_ISVTX
	case S_ISVTX:
		return jmode | 0001000;
#endif
	}
	printf("os_to_jffs2_mode() cannot convert 0x%x\n", osmode);
	BUG();
	return 0;
}

uint32_t jffs2_to_os_mode (uint32_t jmode) {
	uint32_t osmode = ((jmode & 00400)?S_IRUSR:0) |
		((jmode & 00200)?S_IWUSR:0) |
		((jmode & 00100)?S_IXUSR:0) |
		((jmode & 00040)?S_IRGRP:0) |
		((jmode & 00020)?S_IWGRP:0) |
		((jmode & 00010)?S_IXGRP:0) |
		((jmode & 00004)?S_IROTH:0) |
		((jmode & 00002)?S_IWOTH:0) |
		((jmode & 00001)?S_IXOTH:0);

	switch(jmode & 00170000) {
	case 0140000:
		return osmode | S_IFSOCK;
	case 0120000:
		return osmode | S_IFLNK;
	case 0100000:
		return osmode | S_IFREG;
	case 0060000:
		return osmode | S_IFBLK;
	case 0040000:
		return osmode | S_IFDIR;
	case 0020000:
		return osmode | S_IFCHR;
	case 0010000:
		return osmode | S_IFIFO;
	case 0004000:
		return osmode | S_ISUID;
	case 0002000:
		return osmode | S_ISGID;
#ifdef S_ISVTX
	case 0001000:
		return osmode | S_ISVTX;
#endif
	}
	printf("jffs2_to_os_mode() cannot convert 0x%x\n", osmode);
	BUG();
	return 0;
}

static struct idesc *jffs2fs_open(struct inode *node, struct idesc *idesc, int __oflag);
static int jffs2fs_close(struct file_desc *desc);
static size_t jffs2fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t jffs2fs_write(struct file_desc *desc, void *buf, size_t size);

static struct file_operations jffs2_fop = {
	.open = jffs2fs_open,
	.close = jffs2fs_close,
	.read = jffs2fs_read,
	.write = jffs2fs_write,
};

/*
 * file_operation
 */
static struct idesc *jffs2fs_open(struct inode *node, struct idesc *idesc, int __oflag) {
	struct jffs2_file_info *fi;
	struct jffs2_fs_info *fsi;
	char path[PATH_MAX];
	int res;

	fi = inode_priv(node);
	fsi = node->i_sb->sb_data;

	file_set_size(file_desc_from_idesc(idesc), fi->_inode->i_size);

	vfs_get_relative_path(node, path, PATH_MAX);

	res = jffs2_open(fsi->jffs2_sb.s_root, path, idesc->idesc_flags);
	if (res) {
		return err_ptr(-res);
	}
	return idesc;
}

static int jffs2fs_close(struct file_desc *desc) {
	struct jffs2_file_info *fi;

	if (NULL == desc) {
		return 0;
	}
	fi = inode_priv(desc->f_inode);
	file_set_size(desc, fi->_inode->i_size);

	return jffs2_fo_close(fi->_inode);
}

static size_t jffs2fs_read(struct file_desc *desc, void *buff, size_t size) {
	int rc;
	struct jffs2_file_info *fi;
	struct jffs2_inode_info *f;
	struct jffs2_sb_info *c;
	size_t len;
	off_t pos;

	pos = file_get_pos(desc);

	fi = inode_priv(desc->f_inode);

	f = JFFS2_INODE_INFO(fi->_inode);
	c = &fi->_inode->i_sb->jffs2_sb;

	len = min(size, fi->_inode->i_size - pos);

	if (0 != (rc = jffs2_read_inode_range(c, f,
			(unsigned char *) buff, pos, len))) {
		SET_ERRNO(rc);
		return 0;
	}

	return len;
}

static size_t jffs2fs_write(struct file_desc *desc, void *buff, size_t size) {
	uint32_t bytecount;
	struct jffs2_file_info *fi;

	fi = inode_priv(desc->f_inode);

	bytecount = jffs2_fo_write(desc, buff, size);

	file_set_size(desc, fi->_inode->i_size);

	return bytecount;
}

static int jffs2_free_fs(struct super_block *sb) {
	struct jffs2_fs_info *fsi = sb->sb_data;

	if (NULL != fsi) {
		pool_free(&jffs2_fs_pool, fsi);
	}

	return 0;
}

static int jffs2fs_format(struct block_dev *bdev, void *priv);
static int jffs2fs_mount(struct super_block *sb, struct inode *dest);
static int jffs2fs_create(struct inode *i_new, struct inode *parent_node, int mode);
static int jffs2fs_delete(struct inode *node);
static int jffs2fs_truncate(struct inode *node, off_t length);

static struct fsop_desc jffs2_fsop = {
	.mount	      = jffs2fs_mount,
	.create_node  = jffs2fs_create,
	.delete_node  = jffs2fs_delete,

	.getxattr     = NULL,
	.setxattr     = NULL,
	.listxattr    = NULL,

	.truncate     = jffs2fs_truncate,
	.umount_entry = jffs2fs_umount_entry,
};

static int jffs2_fill_sb(struct super_block *sb, const char *source);
static struct fs_driver jffs2fs_driver = {
	.name = FS_NAME,
	.format      = jffs2fs_format,
	.fill_sb = jffs2_fill_sb,
	.clean_sb = jffs2_clean_sb,
//	.file_op = &jffs2_fop,
	.fsop = &jffs2_fsop,
};

static jffs2_file_info_t *jffs2_fi_alloc(struct inode *i_new, void *fs) {
	jffs2_file_info_t *fi;

	fi = pool_alloc(&jffs2_file_pool);
	if (fi) {
		memset(fi, 0, sizeof(struct jffs2_file_info));
		inode_priv_set(i_new, fi);
		inode_size_set(i_new, 0);
	}

	return fi;
}

static int mount_vfs_dir_enty(struct nas *dir_nas) {
	struct jffs2_inode_info *dir_f;
	struct jffs2_full_dirent *fd_list;
	struct _inode *inode = NULL;
	uint32_t ino = 0;
	struct inode *vfs_node;
	struct nas *nas;
	struct _inode *dir_i;
	struct jffs2_file_info *fi;

	fi = inode_priv(dir_nas->node);
	dir_i = fi->_inode;

	dir_f = JFFS2_INODE_INFO(dir_i);

	for (fd_list = dir_f->dents; NULL != fd_list; fd_list = fd_list->next) {
		if (fd_list) {
			ino = fd_list->ino;
			if (ino) {
				inode = jffs2_iget(dir_i->i_sb, ino);
				if(NULL == (vfs_node = vfs_subtree_lookup(dir_nas->node,
						(const char *) fd_list->name))) {
					vfs_node = vfs_subtree_create(dir_nas->node,
							(const char *) fd_list->name, inode->i_mode);
					if(NULL == vfs_node) {
						return ENOMEM;
					}
				}
				nas = vfs_node->nas;
				if (NULL == inode_priv(vfs_node)) {
					if (NULL == (fi = jffs2_fi_alloc(vfs_node, dir_nas->fs))) {
						inode_priv_set(vfs_node, fi);
						return ENOMEM;
					}
					fi->_inode = inode;
				}

				if(node_is_directory(vfs_node)) {
					mount_vfs_dir_enty(nas);
				}
			}

		}
	}
	return 0;
}

static int jffs2fs_create(struct inode *i_new, struct inode *parent_node, int mode) {
	int rc;
	struct jffs2_file_info *fi, *parents_fi;

	parents_fi = inode_priv(parent_node);

	if (node_is_directory(i_new)) {
		i_new->i_mode |= S_IRUGO|S_IXUGO|S_IWUSR;
		if (0 != (rc = jffs2_ops_mkdir(parents_fi->_inode,
				(const char *) inode_name(i_new), i_new->i_mode))) {
			return -rc;
		}
		/* file info for new dir will be allocate into */
		if (0 != (rc = mount_vfs_dir_enty(parent_node->nas))) {
			return -rc;
		}
	} else {
		if (NULL == (fi = jffs2_fi_alloc(i_new, parent_node->nas->fs))) {
				inode_priv_set(i_new, fi);
				return ENOMEM;
			}
		if (0 != (rc = jffs2_create(parents_fi->_inode,
				(const unsigned char *) inode_name(i_new),
								i_new->i_mode, &fi->_inode))) {
			return -rc;
		}
	}
	return 0;
}

static int jffs2fs_delete(struct inode *node) {
	int rc;
	struct inode *parent;
	struct jffs2_file_info *par_fi, *fi;

	if (NULL == (parent = vfs_subtree_get_parent(node))) {
		rc = ENOENT;
		return -rc;
	}

	par_fi = inode_priv(parent);
	fi = inode_priv(node);
	if (node_is_directory(node)) {
		if (0 != (rc = jffs2_ops_rmdir(par_fi->_inode,
						(const char *) inode_name(node)))) {
			return -rc;
		}
	} else {
		if (0 != (rc = jffs2_ops_unlink(par_fi->_inode,
						(const char *) inode_name(node)))) {
			return -rc;
		}
	}

	if(NULL != (fi = inode_priv(node))) {
		pool_free(&jffs2_file_pool, fi);
	}

	return 0;
}

static int jffs2fs_format(struct block_dev *bdev, void *priv) {
	char flash_node_name[PATH_MAX];

	snprintf(flash_node_name, PATH_MAX, "%s_flash", block_dev_name(bdev));

	return flash_emu_dev_create(flash_node_name, 16 * 1024, 1024);
}

static int jffs2fs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct super_block_operations jffs2fs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = jffs2fs_destroy_inode,
};

struct inode_operations jffs2fs_iops = {
	.iterate = jffs2fs_iterate,
};

struct inode *jffs2fs_lookup(char const *name, struct inode const *dir) {
	return NULL;
}

static int jffs2fs_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx) {
	struct jffs2_inode_info *dir_f;
	struct jffs2_full_dirent *fd_list;
	struct _inode *inode = NULL;
	uint32_t ino = 0;
	struct _inode *dir_i;
	struct jffs2_file_info *fi;
	int idx = 0;

	fi = inode_priv(parent);
	dir_i = fi->_inode;

	dir_f = JFFS2_INODE_INFO(dir_i);

	for (fd_list = dir_f->dents; NULL != fd_list; fd_list = fd_list->next) {
		if (fd_list) {
			ino = fd_list->ino;
			if (ino) {
				inode = jffs2_iget(dir_i->i_sb, ino);

				if (idx++ < (int)(uintptr_t)dir_ctx->fs_ctx) {
					continue;
				}
				if (NULL == (fi = jffs2_fi_alloc(next, parent->nas->fs))) {
					return -1;
				}
				inode_priv_set(next, fi);
				fi->_inode = inode;

				next->i_mode = inode->i_mode;

				next->uid = inode->i_uid;
				next->gid =  inode->i_gid;
				inode_size_set(next,  inode->i_size);

				strncpy(next_name, (const char *)fd_list->name, NAME_MAX - 1);
				next_name[NAME_MAX - 1] = '\0';

				dir_ctx->fs_ctx = (void *)(uintptr_t)idx;

				return 0;
			}

		}
	}

	return -1;
}

static int jffs2_fill_sb(struct super_block *sb, const char *source) {
	struct block_dev *bdev;
	struct jffs2_fs_info *fsi;

	bdev = bdev_by_path(source);
	if (bdev == NULL) {
		return -ENODEV;
	}

	sb->bdev = bdev;

	/* allocate this fs info */
	if (NULL == (fsi = pool_alloc(&jffs2_fs_pool))) {
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct jffs2_fs_info));
	sb->sb_data = fsi;
	sb->sb_ops = &jffs2fs_sbops;
	sb->sb_iops = &jffs2fs_iops;
	sb->sb_fops = &jffs2_fop;

	return 0;
}

static int jffs2fs_mount(struct super_block *sb, struct inode *dest) {
	int rc;
	struct nas *dir_nas;
	struct jffs2_file_info *fi;
	struct jffs2_fs_info *fsi;

	dir_nas = dest->nas;

	if (NULL == (fi = pool_alloc(&jffs2_file_pool))) {
		inode_priv_set(dest, fi);
		rc = ENOMEM;
		goto error;
	}
	memset(fi, 0, sizeof(struct jffs2_file_info));

	if (0 != (rc = jffs2_mount(dir_nas))) {
		goto error;
	}

	inode_priv_set(dest, fi);
	fsi = sb->sb_data;
	fi->_inode = fsi->jffs2_sb.s_root;
#if 0
	if(0 != (rc = mount_vfs_dir_enty(dir_nas))) {
		goto error;
	}
#endif
	return 0;

error:
	jffs2_free_fs(sb);

	return -rc;
}

static int jffs2fs_truncate (struct inode *node, off_t length) {
	struct jffs2_file_info *fi;

	inode_size_set(node, length);

	fi = inode_priv(node);
	jffs2_truncate_file(fi->_inode);

	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(jffs2fs_driver);

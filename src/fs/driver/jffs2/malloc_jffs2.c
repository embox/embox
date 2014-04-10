/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Free Software Foundation, Inc.
 *
 * Created by David Woodhouse <dwmw2@cambridge.redhat.com>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: kmalloc-ecos.c,v 1.4 2003/11/26 15:55:35 dwmw2 Exp $
 *
 */

#include <linux/kernel.h>
#include <mem/sysmalloc.h>
#include "nodelist.h"

#include <stdlib.h>

#if !defined(CYGNUM_FS_JFFS2_RAW_NODE_REF_CACHE_POOL_SIZE)
# define CYGNUM_FS_JFFS2_RAW_NODE_REF_CACHE_POOL_SIZE 0
#endif

struct jffs2_full_dirent *jffs2_alloc_full_dirent(int namesize) {
	return sysmalloc(sizeof(struct jffs2_full_dirent) + namesize);
}

void jffs2_free_full_dirent(struct jffs2_full_dirent *x) {
	sysfree(x);
}

struct jffs2_full_dnode *jffs2_alloc_full_dnode(void) {
	return sysmalloc(sizeof(struct jffs2_full_dnode));
}

void jffs2_free_full_dnode(struct jffs2_full_dnode *x) {
	sysfree(x);
}

struct jffs2_raw_dirent *jffs2_alloc_raw_dirent(void) {
	return sysmalloc(sizeof(struct jffs2_raw_dirent));
}

void jffs2_free_raw_dirent(struct jffs2_raw_dirent *x) {
	sysfree(x);
}

struct jffs2_raw_inode *jffs2_alloc_raw_inode(void) {
	return sysmalloc(sizeof(struct jffs2_raw_inode));
}

void jffs2_free_raw_inode(struct jffs2_raw_inode *x) {
	sysfree(x);
}

struct jffs2_tmp_dnode_info *jffs2_alloc_tmp_dnode_info(void) {
	return sysmalloc(sizeof(struct jffs2_tmp_dnode_info));
}

void jffs2_free_tmp_dnode_info(struct jffs2_tmp_dnode_info *x) {
	sysfree(x);
}

struct jffs2_node_frag *jffs2_alloc_node_frag(void) {
	return sysmalloc(sizeof(struct jffs2_node_frag));
}

void jffs2_free_node_frag(struct jffs2_node_frag *x) {
	sysfree(x);
}

#if CYGNUM_FS_JFFS2_RAW_NODE_REF_CACHE_POOL_SIZE == 0

int jffs2_create_slab_caches(void) {
	return 0;
}

void jffs2_destroy_slab_caches(void) {
}

struct jffs2_raw_node_ref *jffs2_alloc_raw_node_ref(void) {
	return sysmalloc(sizeof(struct jffs2_raw_node_ref));
}

void jffs2_free_raw_node_ref(struct jffs2_raw_node_ref *x) {
	sysfree(x);
}

#endif /* CYGNUM_FS_JFFS2_RAW_NODE_REF_CACHE_POOL_SIZE == 0 */

struct jffs2_inode_cache *jffs2_alloc_inode_cache(void) {
	struct jffs2_inode_cache *ret = sysmalloc(sizeof(struct jffs2_inode_cache));
	D1(printk(KERN_DEBUG "Allocated inocache at %p\n", ret));
	return ret;
}

void jffs2_free_inode_cache(struct jffs2_inode_cache *x) {
	D1(printk(KERN_DEBUG "Freeing inocache at %p\n", x));
	sysfree(x);
}


/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Red Hat, Inc.
 *
 * Created by David Woodhouse <dwmw2@infradead.org>
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: readinode.c,v 1.132 2005/07/28 14:46:40 dedekind Exp $
 *
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/crc32.h>
#include <linux/pagemap.h>
#include <linux/mtd/mtd.h>
#include <linux/compiler.h>
#include "nodelist.h"

void jffs2_truncate_fragtree (struct jffs2_sb_info *c,
					struct rb_root *list, uint32_t size) {
	struct jffs2_node_frag *frag = jffs2_lookup_node_frag(list, size);

	JFFS2_DBG_FRAGTREE("truncating fragtree to 0x%08x bytes\n", size);

	/* We know frag->ofs <= size. That's what lookup does for us */
	if (frag && frag->ofs != size) {
		if (frag->ofs+frag->size >= size) {
			JFFS2_DBG_FRAGTREE2("truncating frag 0x%08x-0x%08x\n", frag->ofs, frag->ofs+frag->size);
			frag->size = size - frag->ofs;
		}
		frag = frag_next(frag);
	}
	while (frag && frag->ofs >= size) {
		struct jffs2_node_frag *next = frag_next(frag);

		JFFS2_DBG_FRAGTREE("removing frag 0x%08x-0x%08x\n", frag->ofs, frag->ofs+frag->size);
		frag_erase(frag, list);
		jffs2_obsolete_node_frag(c, frag);
		frag = next;
	}
}

/*
 * Put a new tmp_dnode_info into the temporaty RB-tree, keeping the list in
 * order of increasing version.
 */
static void jffs2_add_tn_to_tree(struct jffs2_tmp_dnode_info *tn, struct rb_root *list)
{
	struct rb_node **p = &list->rb_node;
	struct rb_node * parent = NULL;
	struct jffs2_tmp_dnode_info *this;

	while (*p) {
		parent = *p;
		this = rb_entry(parent, struct jffs2_tmp_dnode_info, rb);

		/* There may actually be a collision here, but it doesn't
		 * actually matter. As long as the two nodes with the same
		 * version are together, it's all fine.
		 */
		if (tn->version < this->version) {
			p = &(*p)->rb_left;
		} else {
			p = &(*p)->rb_right;
		}
	}

	rb_link_node(&tn->rb, parent, p);
	rb_insert_color(&tn->rb, list);
}

static void jffs2_free_tmp_dnode_info_list(struct rb_root *list) {
	struct rb_node *this;
	struct jffs2_tmp_dnode_info *tn;

	this = list->rb_node;

	/* Now at bottom of tree */
	while (this) {
		if (this->rb_left) {
			this = this->rb_left;
		} else if (this->rb_right) {
			this = this->rb_right;
		} else {
			tn = rb_entry(this, struct jffs2_tmp_dnode_info, rb);
			jffs2_free_full_dnode(tn->fn);
			jffs2_free_tmp_dnode_info(tn);

			this = this->rb_parent;
			if (!this) {
				break;
			}

			if (this->rb_left == &tn->rb) {
				this->rb_left = NULL;
			} else if (this->rb_right == &tn->rb) {
				this->rb_right = NULL;
			} else {
				BUG();
			}
		}
	}
	list->rb_node = NULL;
}

static void jffs2_free_full_dirent_list(struct jffs2_full_dirent *fd) {
	struct jffs2_full_dirent *next;

	while (fd) {
		next = fd->next;
		jffs2_free_full_dirent(fd);
		fd = next;
	}
}

/* Returns first valid node after 'ref'. May return 'ref' */
static struct jffs2_raw_node_ref
			*jffs2_first_valid_node(struct jffs2_raw_node_ref *ref) {
	while (ref && ref->next_in_ino) {
		if (!ref_obsolete(ref)) {
			return ref;
		}
		JFFS2_DBG_NODEREF("node at 0x%08x is obsoleted. Ignoring.\n", ref_offset(ref));
		ref = ref->next_in_ino;
	}
	return NULL;
}

/*
 * Helper function for jffs2_get_inode_nodes().
 * It is called every time an directory entry node is found.
 *
 * Returns: 0 on succes;
 * 	    1 if the node should be marked obsolete;
 * 	    negative error code on failure.
 */
static inline int read_direntry(struct jffs2_sb_info *c,
	      struct jffs2_raw_node_ref *ref, struct jffs2_raw_dirent *rd,
	      uint32_t read, struct jffs2_full_dirent **fdp,
	      uint32_t *latest_mctime, uint32_t *mctime_ver) {
	struct jffs2_full_dirent *fd;
	size_t retlen;

	/* The direntry nodes are checked during the flash scanning */
	BUG_ON(ref_flags(ref) == REF_UNCHECKED);
	/* Obsoleted. This cannot happen, surely? dwmw2 20020308 */
	BUG_ON(ref_obsolete(ref));

	/* Sanity check */
	if (unlikely(PAD((rd->nsize + sizeof(*rd))) != PAD(je32_to_cpu(rd->totlen)))) {
		JFFS2_ERROR("illegal nsize in node at %#08x: nsize %#02x, totlen %#04x\n",
		       ref_offset(ref), rd->nsize, je32_to_cpu(rd->totlen));
		return 1;
	}

	fd = jffs2_alloc_full_dirent(rd->nsize + 1);
	if (unlikely(!fd)) {
		return -ENOMEM;
	}

	fd->raw = ref;
	fd->version = je32_to_cpu(rd->version);
	fd->ino = je32_to_cpu(rd->ino);
	fd->type = rd->type;

	/* Pick out the mctime of the latest dirent */
	if(fd->version > *mctime_ver) {
		*mctime_ver = fd->version;
		*latest_mctime = je32_to_cpu(rd->mctime);
	}

	/*
	 * Copy as much of the name as possible from the raw
	 * dirent we've already read from the flash.
	 */
	if (read > sizeof(*rd)) {
		memcpy(&fd->name[0], &rd->name[0],
		       min_t(uint32_t, rd->nsize, (read - sizeof(*rd)) ));
	}

	/* Do we need to copy any more of the name directly from the flash? */
	if (rd->nsize + sizeof(*rd) > read) {
		/* FIXME: point() */
		int err;
		int already = read - sizeof(*rd);

		err = jffs2_flash_read(c, (ref_offset(ref)) + read, rd->nsize - already,
		    &retlen, &fd->name[already]);
		if (unlikely(retlen != rd->nsize - already) && likely(!err)) {
			return -EIO;
		}
		if (unlikely(err)) {
			JFFS2_ERROR("read remainder of name: error %d\n", err);
			jffs2_free_full_dirent(fd);
			return -EIO;
		}
	}

	fd->nhash = full_name_hash(fd->name, rd->nsize);
	fd->next = NULL;
	fd->name[rd->nsize] = '\0';

	/*
	 * Wheee. We now have a complete jffs2_full_dirent structure, with
	 * the name in it and everything. Link it into the list
	 */
	jffs2_add_fd_to_list(c, fd, fdp);

	return 0;
}

/*
 * Helper function for jffs2_get_inode_nodes().
 * It is called every time an inode node is found.
 *
 * Returns: 0 on succes;
 * 	    1 if the node should be marked obsolete;
 * 	    negative error code on failure.
 */
static inline int read_dnode(struct jffs2_sb_info *c,
	   struct jffs2_raw_node_ref *ref, struct jffs2_raw_inode *rd,
	   uint32_t read, struct rb_root *tnp, uint32_t *latest_mctime,
	   	   	   	   	   	   	   	   	   	   	   uint32_t *mctime_ver) {
	struct jffs2_eraseblock *jeb;
	struct jffs2_tmp_dnode_info *tn;
	size_t retlen;

	/* Obsoleted. This cannot happen, surely? dwmw2 20020308 */
	BUG_ON(ref_obsolete(ref));

	/* If we've never checked the CRCs on this node, check them now */
	if (ref_flags(ref) == REF_UNCHECKED) {
		uint32_t crc, len;

		crc = crc32(0, rd, sizeof(*rd) - 8);
		if (unlikely(crc != je32_to_cpu(rd->node_crc))) {
			JFFS2_NOTICE("header CRC failed on node at %#08x: read %#08x, calculated %#08x\n",
					ref_offset(ref), je32_to_cpu(rd->node_crc), crc);
			return 1;
		}

		/* Sanity checks */
		if (unlikely(je32_to_cpu(rd->offset) > je32_to_cpu(rd->isize)) ||
		    unlikely(PAD(je32_to_cpu(rd->csize) + sizeof(*rd)) != PAD(je32_to_cpu(rd->totlen)))) {
				JFFS2_WARNING("inode node header CRC is corrupted at %#08x\n", ref_offset(ref));
				jffs2_dbg_dump_node(c, ref_offset(ref));
			return 1;
		}

		if (rd->compr != JFFS2_COMPR_ZERO && je32_to_cpu(rd->csize)) {
			unsigned char *buf = NULL;
			uint32_t pointed = 0;
			int err;
			if(!pointed){
				buf = kmalloc(je32_to_cpu(rd->csize), GFP_KERNEL);
				if (!buf) {
					return -ENOMEM;
				}

				err = jffs2_flash_read(c, ref_offset(ref) + sizeof(*rd),
				    je32_to_cpu(rd->csize), &retlen, buf);
				if (unlikely(retlen != je32_to_cpu(rd->csize)) && likely(!err)) {
					err = -EIO;
				}
				if (err) {
					kfree(buf);
					return err;
				}
			}
			crc = crc32(0, buf, je32_to_cpu(rd->csize));
			if(!pointed) {
				kfree(buf);
			}

			if (crc != je32_to_cpu(rd->data_crc)) {
				JFFS2_NOTICE("data CRC failed on node at %#08x: read %#08x, calculated %#08x\n",
					ref_offset(ref), je32_to_cpu(rd->data_crc), crc);
				return 1;
			}

		}

		/* Mark the node as having been checked and fix the accounting accordingly */
		jeb = &c->blocks[ref->flash_offset / c->sector_size];
		len = ref_totlen(c, jeb, ref);

		spin_lock(&c->erase_completion_lock);
		jeb->used_size += len;
		jeb->unchecked_size -= len;
		c->used_size += len;
		c->unchecked_size -= len;

		/* If node covers at least a whole page, or if it starts at the
		 * beginning of a page and runs to the end of the file, or if
		 * it's a hole node, mark it REF_PRISTINE, else REF_NORMAL.
		 *
		 * If it's actually overlapped, it'll get made NORMAL (or OBSOLETE)
		 * when the overlapping node(s) get added to the tree anyway.
		 */
		if ((je32_to_cpu(rd->dsize) >= PAGE_CACHE_SIZE) ||
		    ( ((je32_to_cpu(rd->offset) & (PAGE_CACHE_SIZE-1)) == 0) &&
		      (je32_to_cpu(rd->dsize) + je32_to_cpu(rd->offset) ==
		    		  	  	  	  	  	  	  	  je32_to_cpu(rd->isize)))) {
			JFFS2_DBG_READINODE("marking node at %#08x REF_PRISTINE\n", ref_offset(ref));
			ref->flash_offset = ref_offset(ref) | REF_PRISTINE;
		} else {
			JFFS2_DBG_READINODE("marking node at %#08x REF_NORMAL\n", ref_offset(ref));
			ref->flash_offset = ref_offset(ref) | REF_NORMAL;
		}
		spin_unlock(&c->erase_completion_lock);
	}

	tn = jffs2_alloc_tmp_dnode_info();
	if (!tn) {
		JFFS2_ERROR("alloc tn failed\n");
		return -ENOMEM;
	}

	tn->fn = jffs2_alloc_full_dnode();
	if (!tn->fn) {
		JFFS2_ERROR("alloc fn failed\n");
		jffs2_free_tmp_dnode_info(tn);
		return -ENOMEM;
	}

	tn->version = je32_to_cpu(rd->version);
	tn->fn->ofs = je32_to_cpu(rd->offset);
	tn->fn->raw = ref;

	/* There was a bug where we wrote hole nodes out with
	 * csize/dsize swapped. Deal with it
	 */
	if (rd->compr == JFFS2_COMPR_ZERO && !je32_to_cpu(rd->dsize)
										&& je32_to_cpu(rd->csize)) {
		tn->fn->size = je32_to_cpu(rd->csize);
	} 	else { /* normal case... */
		tn->fn->size = je32_to_cpu(rd->dsize);
	}

	JFFS2_DBG_READINODE("dnode @%08x: ver %u, offset %#04x, dsize %#04x\n",
		  ref_offset(ref), je32_to_cpu(rd->version), je32_to_cpu(rd->offset), je32_to_cpu(rd->dsize));

	jffs2_add_tn_to_tree(tn, tnp);

	return 0;
}

/*
 * Helper function for jffs2_get_inode_nodes().
 * It is called every time an unknown node is found.
 *
 * Returns: 0 on succes;
 * 	    1 if the node should be marked obsolete;
 * 	    negative error code on failure.
 */
static inline int read_unknown(struct jffs2_sb_info *c,
	     struct jffs2_raw_node_ref *ref,
	     struct jffs2_unknown_node *un, uint32_t read) {
	/* We don't mark unknown nodes as REF_UNCHECKED */
	BUG_ON(ref_flags(ref) == REF_UNCHECKED);

	un->nodetype = cpu_to_je16(JFFS2_NODE_ACCURATE |
							je16_to_cpu(un->nodetype));

	if (crc32(0, un, sizeof(struct jffs2_unknown_node) - 4) !=
									je32_to_cpu(un->hdr_crc)) {
		/* Hmmm. This should have been caught at scan time. */
		JFFS2_NOTICE("node header CRC failed at %#08x. But it must have been OK earlier.\n", ref_offset(ref));
		jffs2_dbg_dump_node(c, ref_offset(ref));
		return 1;
	} else {
		switch(je16_to_cpu(un->nodetype) & JFFS2_COMPAT_MASK) {

		case JFFS2_FEATURE_INCOMPAT:
			JFFS2_ERROR("unknown INCOMPAT nodetype %#04X at %#08x\n",
				je16_to_cpu(un->nodetype), ref_offset(ref));
			/* EEP */
			BUG();
			break;

		case JFFS2_FEATURE_ROCOMPAT:
			JFFS2_ERROR("unknown ROCOMPAT nodetype %#04X at %#08x\n",
					je16_to_cpu(un->nodetype), ref_offset(ref));
			BUG_ON(!(c->flags & JFFS2_SB_FLAG_RO));
			break;

		case JFFS2_FEATURE_RWCOMPAT_COPY:
			JFFS2_NOTICE("unknown RWCOMPAT_COPY nodetype %#04X at %#08x\n",
					je16_to_cpu(un->nodetype), ref_offset(ref));
			break;

		case JFFS2_FEATURE_RWCOMPAT_DELETE:
			JFFS2_NOTICE("unknown RWCOMPAT_DELETE nodetype %#04X at %#08x\n",
					je16_to_cpu(un->nodetype), ref_offset(ref));
			return 1;
		}
	}

	return 0;
}

/**
 *  Get tmp_dnode_info and full_dirent for all non-obsolete nodes associated
 * with this ino, returning the former in order of version
 */
static int jffs2_get_inode_nodes(struct jffs2_sb_info *c, struct jffs2_inode_info *f,
				 struct rb_root *tnp, struct jffs2_full_dirent **fdp,
				 uint32_t *highest_version, uint32_t *latest_mctime,
				 uint32_t *mctime_ver) {
	struct jffs2_raw_node_ref *ref, *valid_ref;
	struct rb_root ret_tn = RB_ROOT;
	struct jffs2_full_dirent *ret_fd = NULL;
	union jffs2_node_union node;
	size_t retlen;
	int err;

	*mctime_ver = 0;

	JFFS2_DBG_READINODE("ino #%u\n", f->inocache->ino);

	spin_lock(&c->erase_completion_lock);

	valid_ref = jffs2_first_valid_node(f->inocache->nodes);

	if (!valid_ref && (f->inocache->ino != 1)) {
		JFFS2_WARNING("no valid nodes for ino #%u\n", f->inocache->ino);
	}

	while (valid_ref) {
		/* We can hold a pointer to a non-obsolete node without the spinlock,
		 * but _obsolete_ nodes may disappear at any time, if the block
		 * they're in gets erased. So if we mark 'ref' obsolete while we're
		 * not holding the lock, it can go away immediately. For that reason,
		 * we find the next valid node first, before processing 'ref'.
		 */
		ref = valid_ref;
		valid_ref = jffs2_first_valid_node(ref->next_in_ino);
		spin_unlock(&c->erase_completion_lock);

		cond_resched();

		/* FIXME: point() */
		err = jffs2_flash_read(c, (ref_offset(ref)),
				       min_t(uint32_t, ref_totlen(c, NULL, ref), sizeof(node)),
				       &retlen, (void *)&node);
		if (err) {
			JFFS2_ERROR("error %d reading node at 0x%08x in get_inode_nodes()\n", err, ref_offset(ref));
			goto free_out;
		}

		switch (je16_to_cpu(node.u.nodetype)) {

		case JFFS2_NODETYPE_DIRENT:
			JFFS2_DBG_READINODE("node at %08x (%d) is a dirent node\n", ref_offset(ref), ref_flags(ref));

			if (retlen < sizeof(node.d)) {
				JFFS2_ERROR("short read dirent at %#08x\n", ref_offset(ref));
				err = -EIO;
				goto free_out;
			}

			err = read_direntry(c, ref, &node.d, retlen, &ret_fd,
					latest_mctime, mctime_ver);
			if (err == 1) {
				jffs2_mark_node_obsolete(c, ref);
				break;
			} else if (unlikely(err)) {
				goto free_out;
			}

			if (je32_to_cpu(node.d.version) > *highest_version) {
				*highest_version = je32_to_cpu(node.d.version);
			}

			break;

		case JFFS2_NODETYPE_INODE:
			JFFS2_DBG_READINODE("node at %08x (%d) is a data node\n",
					ref_offset(ref), ref_flags(ref));

			if (retlen < sizeof(node.i)) {
				JFFS2_ERROR("short read dnode at %#08x\n", ref_offset(ref));
				err = -EIO;
				goto free_out;
			}

			err = read_dnode(c, ref, &node.i, retlen, &ret_tn,
					latest_mctime, mctime_ver);
			if (err == 1) {
				jffs2_mark_node_obsolete(c, ref);
				break;
			} else if (unlikely(err)) {
				goto free_out;
			}

			if (je32_to_cpu(node.i.version) > *highest_version) {
				*highest_version = je32_to_cpu(node.i.version);
			}

			JFFS2_DBG_READINODE("version %d, highest_version now %d\n",
					je32_to_cpu(node.i.version), *highest_version);

			break;

		default:
			/* Check we've managed to read at least the common node header */
			if (retlen < sizeof(struct jffs2_unknown_node)) {
				JFFS2_ERROR("short read unknown node at %#08x\n", ref_offset(ref));
				return -EIO;
			}

			err = read_unknown(c, ref, &node.u, retlen);
			if (err == 1) {
				jffs2_mark_node_obsolete(c, ref);
				break;
			} else if (unlikely(err)) {
				goto free_out;
			}

		}
		spin_lock(&c->erase_completion_lock);

	}
	spin_unlock(&c->erase_completion_lock);
	*tnp = ret_tn;
	*fdp = ret_fd;

	return 0;

 free_out:
	jffs2_free_tmp_dnode_info_list(&ret_tn);
	jffs2_free_full_dirent_list(ret_fd);
	return err;
}

static int jffs2_do_read_inode_internal(struct jffs2_sb_info *c,
					struct jffs2_inode_info *f,
					struct jffs2_raw_inode *latest_node) {
	struct jffs2_tmp_dnode_info *tn = NULL;
	struct rb_root tn_list;
	struct rb_node *rb, *repl_rb;
	struct jffs2_full_dirent *fd_list = NULL;
	struct jffs2_full_dnode *fn = NULL;
	uint32_t crc;
	uint32_t latest_mctime = 0, mctime_ver;
	uint32_t mdata_ver = 0;
	size_t retlen;
	int ret;

	JFFS2_DBG_READINODE("ino #%u nlink is %d\n", f->inocache->ino, f->inocache->nlink);

	/* Grab all nodes relevant to this ino */
	ret = jffs2_get_inode_nodes(c, f, &tn_list,
			&fd_list, &f->highest_version, &latest_mctime, &mctime_ver);

	if (ret) {
		JFFS2_ERROR("cannot read nodes for ino %u, returned error is %d\n", f->inocache->ino, ret);
		if (f->inocache->state == INO_STATE_READING) {
			jffs2_set_inocache_state(c, f->inocache, INO_STATE_CHECKEDABSENT);
		}
		return ret;
	}
	f->dents = fd_list;

	rb = rb_first(&tn_list);

	while (rb) {
		tn = rb_entry(rb, struct jffs2_tmp_dnode_info, rb);
		fn = tn->fn;

		if (f->metadata) {
			if (likely(tn->version >= mdata_ver)) {
				JFFS2_DBG_READINODE("obsoleting old metadata at 0x%08x\n", ref_offset(f->metadata->raw));
				jffs2_mark_node_obsolete(c, f->metadata->raw);
				jffs2_free_full_dnode(f->metadata);
				f->metadata = NULL;

				mdata_ver = 0;
			} else {
				/* This should never happen. */
				JFFS2_ERROR("Er. New metadata at 0x%08x with ver %d is actually older than previous ver %d at 0x%08x\n",
					  ref_offset(fn->raw), tn->version, mdata_ver, ref_offset(f->metadata->raw));
				jffs2_mark_node_obsolete(c, fn->raw);
				jffs2_free_full_dnode(fn);
				/* Fill in latest_node from the metadata, not this one we're about to free... */
				fn = f->metadata;
				goto next_tn;
			}
		}

		if (fn->size) {
			jffs2_add_full_dnode_to_inode(c, f, fn);
		} else {
			/* Zero-sized node at end of version list. Just a metadata update */
			JFFS2_DBG_READINODE("metadata @%08x: ver %d\n", ref_offset(fn->raw), tn->version);
			f->metadata = fn;
			mdata_ver = tn->version;
		}
	next_tn:
		BUG_ON(rb->rb_left);
		if (rb->rb_parent && rb->rb_parent->rb_left == rb) {
			/* We were then left-hand child of our parent. We need
			   to move our own right-hand child into our place. */
			repl_rb = rb->rb_right;
			if (repl_rb) {
				repl_rb->rb_parent = rb->rb_parent;
			}
		} else {
			repl_rb = NULL;
		}

		rb = rb_next(rb);

		/* Remove the spent tn from the tree; don't bother rebalancing
		   but put our right-hand child in our own place. */
		if (tn->rb.rb_parent) {
			if (tn->rb.rb_parent->rb_left == &tn->rb) {
				tn->rb.rb_parent->rb_left = repl_rb;
			} else if (tn->rb.rb_parent->rb_right == &tn->rb) {
				tn->rb.rb_parent->rb_right = repl_rb;
			} else  {
				BUG();
			}
		} else if (tn->rb.rb_right) {
			tn->rb.rb_right->rb_parent = NULL;
		}

		jffs2_free_tmp_dnode_info(tn);
	}
	jffs2_dbg_fragtree_paranoia_check_nolock(f);

	if (!fn) {
		/* No data nodes for this inode. */
		if (f->inocache->ino != 1) {
			JFFS2_WARNING("no data nodes found for ino #%u\n", f->inocache->ino);
			if (!fd_list) {
				if (f->inocache->state == INO_STATE_READING) {
					jffs2_set_inocache_state(c,
							f->inocache, INO_STATE_CHECKEDABSENT);
				}
				return -EIO;
			}
			JFFS2_NOTICE("but it has children so we fake some modes for it\n");
		}
		latest_node->mode = cpu_to_jemode(S_IFDIR|S_IRUGO|S_IWUSR|S_IXUGO);
		latest_node->version = cpu_to_je32(0);
		latest_node->atime = latest_node->ctime =
				latest_node->mtime = cpu_to_je32(0);
		latest_node->isize = cpu_to_je32(0);
		latest_node->gid = cpu_to_je16(0);
		latest_node->uid = cpu_to_je16(0);
		if (f->inocache->state == INO_STATE_READING) {
			jffs2_set_inocache_state(c, f->inocache, INO_STATE_PRESENT);
		}
		return 0;
	}

	ret = jffs2_flash_read(c, ref_offset(fn->raw),
			sizeof(*latest_node), &retlen, (void *)latest_node);
	if (ret || retlen != sizeof(*latest_node)) {
		JFFS2_ERROR("failed to read from flash: error %d, %zd of %zd bytes read\n",
			ret, retlen, sizeof(*latest_node));
		/* FIXME: If this fails, there seems to be a memory leak. Find it. */
		up(&f->sem);
		jffs2_do_clear_inode(c, f);
		return ret?ret:-EIO;
	}

	crc = crc32(0, latest_node, sizeof(*latest_node)-8);
	if (crc != je32_to_cpu(latest_node->node_crc)) {
		JFFS2_ERROR("CRC failed for read_inode of inode %u at physical location 0x%x\n",
			f->inocache->ino, ref_offset(fn->raw));
		up(&f->sem);
		jffs2_do_clear_inode(c, f);
		return -EIO;
	}

	switch(jemode_to_cpu(latest_node->mode) & S_IFMT) {
	case S_IFDIR:
		if (mctime_ver > je32_to_cpu(latest_node->version)) {
			/* The times in the latest_node are actually older than
			 * mctime in the latest dirent. Cheat.
			 */
			latest_node->ctime = latest_node->mtime = cpu_to_je32(latest_mctime);
		}
		break;


	case S_IFREG:
		/* If it was a regular file, truncate it to the latest node's isize */
		jffs2_truncate_fragtree(c, &f->fragtree, je32_to_cpu(latest_node->isize));
		break;

	case S_IFLNK:
		/* Hack to work around broken isize in old symlink code.
		 * Remove this when dwmw2 comes to his senses and stops
		 * symlinks from being an entirely gratuitous special
		 * case.
		 */
		if (!je32_to_cpu(latest_node->isize)) {
			latest_node->isize = latest_node->dsize;
		}

		if (f->inocache->state != INO_STATE_CHECKING) {
			/* Symlink's inode data is the target path. Read it and
			 * keep in RAM to facilitate quick follow symlink
			 * operation. */
			f->target = kmalloc(je32_to_cpu(latest_node->csize) + 1, GFP_KERNEL);
			if (!f->target) {
				JFFS2_ERROR("can't allocate %d bytes of memory for the symlink target path cache\n", je32_to_cpu(latest_node->csize));
				up(&f->sem);
				jffs2_do_clear_inode(c, f);
				return -ENOMEM;
			}

			ret = jffs2_flash_read(c, ref_offset(fn->raw) +
					sizeof(*latest_node), je32_to_cpu(latest_node->csize),
										&retlen, (unsigned char *)f->target);

			if (ret  || retlen != je32_to_cpu(latest_node->csize)) {
				if (retlen != je32_to_cpu(latest_node->csize)) {
					ret = -EIO;
				}
				kfree(f->target);
				f->target = NULL;
				up(&f->sem);
				jffs2_do_clear_inode(c, f);
				return -ret;
			}

			f->target[je32_to_cpu(latest_node->csize)] = '\0';
			JFFS2_DBG_READINODE("symlink's target '%s' cached\n", f->target);
		}

	/* fall through... */
	case S_IFBLK:
	case S_IFCHR:
		/* Certain inode types should have only one data node, and it's
		 * kept as the metadata node
		 */
		if (f->metadata) {
			JFFS2_ERROR("Argh. Special inode #%u with mode 0%o had metadata node\n",
			       f->inocache->ino, jemode_to_cpu(latest_node->mode));
			up(&f->sem);
			jffs2_do_clear_inode(c, f);
			return -EIO;
		}
		if (!frag_first(&f->fragtree)) {
			JFFS2_ERROR("Argh. Special inode #%u with mode 0%o has no fragments\n",
			       f->inocache->ino, jemode_to_cpu(latest_node->mode));
			up(&f->sem);
			jffs2_do_clear_inode(c, f);
			return -EIO;
		}
		/* ASSERT: f->fraglist != NULL */
		if (frag_next(frag_first(&f->fragtree))) {
			JFFS2_ERROR("Argh. Special inode #%u with mode 0x%x had more than one node\n",
			       f->inocache->ino, jemode_to_cpu(latest_node->mode));
			/* FIXME: Deal with it - check crc32, check for duplicate node, check times and discard the older one */
			up(&f->sem);
			jffs2_do_clear_inode(c, f);
			return -EIO;
		}
		/* OK. We're happy */
		f->metadata = frag_first(&f->fragtree)->node;
		jffs2_free_node_frag(frag_first(&f->fragtree));
		f->fragtree = RB_ROOT;
		break;
	}
	if (f->inocache->state == INO_STATE_READING) {
		jffs2_set_inocache_state(c, f->inocache, INO_STATE_PRESENT);
	}

	return 0;
}

/* Scan the list of all nodes present for this ino, build map of versions, etc. */
int jffs2_do_read_inode(struct jffs2_sb_info *c, struct jffs2_inode_info *f,
			uint32_t ino, struct jffs2_raw_inode *latest_node) {
	JFFS2_DBG_READINODE("read inode #%u\n", ino);

 retry_inocache:
	spin_lock(&c->inocache_lock);
	f->inocache = jffs2_get_ino_cache(c, ino);

	if (f->inocache) {
		/* Check its state. We may need to wait before we can use it */
		switch(f->inocache->state) {
		case INO_STATE_UNCHECKED:
		case INO_STATE_CHECKEDABSENT:
			f->inocache->state = INO_STATE_READING;
			break;

		case INO_STATE_CHECKING:
		case INO_STATE_GC:
			/* If it's in either of these states, we need
			 * to wait for whoever's got it to finish and
			 * put it back.
			 */
			JFFS2_DBG_READINODE("waiting for ino #%u in state %d\n", ino, f->inocache->state);
			sleep_on_spinunlock(&c->inocache_wq, &c->inocache_lock);
			goto retry_inocache;

		case INO_STATE_READING:
		case INO_STATE_PRESENT:
			/* Eep. This should never happen. It can happen if Linux calls
			 * read_inode() again before clear_inode() has finished though.
			 */
			JFFS2_ERROR("Eep. Trying to read_inode #%u when it's already in state %d!\n", ino, f->inocache->state);
			/* Fail. That's probably better than allowing it to succeed */
			f->inocache = NULL;
			break;

		default:
			BUG();
			break;
		}
	}
	spin_unlock(&c->inocache_lock);

	if (!f->inocache && ino == 1) {
		/* Special case - no root inode on medium */
		f->inocache = jffs2_alloc_inode_cache();
		if (!f->inocache) {
			JFFS2_ERROR("cannot allocate inocache for root inode\n");
			return -ENOMEM;
		}
		JFFS2_DBG_READINODE("creating inocache for root inode\n");
		memset(f->inocache, 0, sizeof(struct jffs2_inode_cache));
		f->inocache->ino = f->inocache->nlink = 1;
		f->inocache->nodes = (struct jffs2_raw_node_ref *)f->inocache;
		f->inocache->state = INO_STATE_READING;
		jffs2_add_ino_cache(c, f->inocache);
	}
	if (!f->inocache) {
		JFFS2_ERROR("requestied to read an nonexistent ino %u\n", ino);
		return -ENOENT;
	}

	return jffs2_do_read_inode_internal(c, f, latest_node);
}

int jffs2_do_crccheck_inode(struct jffs2_sb_info *c,
						struct jffs2_inode_cache *ic) {
	struct jffs2_raw_inode n;
	struct jffs2_inode_info *f = kmalloc(sizeof(*f), GFP_KERNEL);
	int ret;

	if (!f) {
		return -ENOMEM;
	}

	memset(f, 0, sizeof(*f));
	init_MUTEX_LOCKED(&f->sem);
	f->inocache = ic;

	ret = jffs2_do_read_inode_internal(c, f, &n);
	if (!ret) {
		up(&f->sem);
		jffs2_do_clear_inode(c, f);
	}
	kfree (f);
	return ret;
}

void jffs2_do_clear_inode(struct jffs2_sb_info *c,
						struct jffs2_inode_info *f) {
	struct jffs2_full_dirent *fd, *fds;
	int deleted;

	down(&f->sem);
	deleted = f->inocache && !f->inocache->nlink;

	if (f->inocache && f->inocache->state != INO_STATE_CHECKING) {
		jffs2_set_inocache_state(c, f->inocache, INO_STATE_CLEARING);
	}

	if (f->metadata) {
		if (deleted) {
			jffs2_mark_node_obsolete(c, f->metadata->raw);
		}
		jffs2_free_full_dnode(f->metadata);
	}

	jffs2_kill_fragtree(&f->fragtree, deleted?c:NULL);

	if (f->target) {
		kfree(f->target);
		f->target = NULL;
	}

	fds = f->dents;
	while(fds) {
		fd = fds;
		fds = fd->next;
		jffs2_free_full_dirent(fd);
	}

	if (f->inocache && f->inocache->state != INO_STATE_CHECKING) {
		jffs2_set_inocache_state(c, f->inocache, INO_STATE_CHECKEDABSENT);
		if (f->inocache->nodes == (void *)f->inocache) {
			jffs2_del_ino_cache(c, f->inocache);
		}
	}

	up(&f->sem);
}

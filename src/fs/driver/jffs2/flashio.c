/*
 * JFFS2 -- Journalling Flash File System, Version 2.
 *
 * Copyright (C) 2001-2003 Red Hat, Inc.
 *
 * Created by Dominic Ostrowski <dominic.ostrowski@3glab.com>
 * Contributors: David Woodhouse, Nick Garnett, Richard Panton.
 *
 * For licensing information, see the file 'LICENCE' in this directory.
 *
 * $Id: flashio.c,v 1.1 2003/11/26 14:09:29 dwmw2 Exp $
 *
 */

#include <linux/kernel.h>
#include "nodelist.h"

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>
#include <drivers/block_dev.h>
#include <fs/super_block.h>
#include <mem/sysmalloc.h>

bool jffs2_flash_read(struct jffs2_sb_info * c,
		uint32_t read_buffer_offset, const size_t size,
			  size_t * return_size, unsigned char *write_buffer) {
	int err;
	struct jffs2_super_block *sb;
	sb = member_cast_out(c, struct jffs2_super_block, jffs2_sb);

	err = block_dev_read_buffered(sb->bdev,
			(char *) write_buffer, size, read_buffer_offset);

	if(err >= 0) {
		*return_size = (size_t) err;
		err = ENOERR;
	}

	return err;
}

bool jffs2_flash_write(struct jffs2_sb_info * c,
		uint32_t write_buffer_offset, const size_t size,
		size_t * return_size, unsigned char *read_buffer) {
	int err;
	struct jffs2_super_block *sb;

	sb = member_cast_out(c, struct jffs2_super_block, jffs2_sb);
	err = block_dev_write_buffered(sb->bdev,
	(char *) read_buffer, size, write_buffer_offset);

	if(err >= 0) {
		*return_size = (size_t) err;
		err = ENOERR;
	}

	return err;
}

int jffs2_flash_direct_writev(struct jffs2_sb_info *c, const struct iovec *vecs,
		   unsigned long count, loff_t to, size_t * retlen) {
	unsigned long i;
	size_t totlen = 0, thislen = 0;
	int ret = 0;

	for (i = 0; i < count; i++) {
		/*
		 * writes need to be aligned but the data we're passed may not be
		 * Observation suggests most unaligned writes are small, so we
		 *  optimize for that case.
		 */

		if (((vecs[i].iov_len & (sizeof (int) - 1))) ||
		    (((unsigned long) vecs[i].
		      iov_base & (sizeof (unsigned long) - 1)))) {
			/* are there iov's after this one? Or is it so much we'd need
			 * to do multiple writes anyway?
			 */
			if ((i + 1) < count || vecs[i].iov_len > 256) {
				/* cop out and kmalloc */
				unsigned long j;
				ssize_t sizetomalloc = 0, totvecsize = 0;
				char *cbuf, *cbufptr;

				for (j = i; j < count; j++) {
					totvecsize += vecs[j].iov_len;
				}

				/* pad up in case unaligned */
				sizetomalloc = totvecsize + sizeof (int) - 1;
				sizetomalloc &= ~(sizeof (int) - 1);
				cbuf = (char *) sysmalloc(sizetomalloc);
				/* kmalloc returns aligned memory */
				if (!cbuf) {
					ret = -ENOMEM;
					goto writev_out;
				}
				cbufptr = cbuf;
				for (j = i; j < count; j++) {
					memcpy(cbufptr, vecs[j].iov_base,
					       vecs[j].iov_len);
					cbufptr += vecs[j].iov_len;
				}
				ret =
				    jffs2_flash_write(c, to, sizetomalloc,
						      &thislen, (unsigned char *) cbuf);
				if (thislen > totvecsize) {	/* in case it was aligned up */
					thislen = totvecsize;
				}
				totlen += thislen;
				sysfree(cbuf);
				goto writev_out;
			} else {
				/* otherwise optimize for the common case */
				int buf[256 / sizeof (int)];	/* int, so int aligned */
				size_t lentowrite;

				lentowrite = vecs[i].iov_len;
				/* pad up in case its unaligned */
				lentowrite += sizeof (int) - 1;
				lentowrite &= ~(sizeof (int) - 1);
				memcpy(buf, vecs[i].iov_base, lentowrite);

				ret =
				    jffs2_flash_write(c, to, lentowrite,
						      &thislen, (unsigned char *) &buf);
				if (thislen > vecs[i].iov_len) {
					thislen = vecs[i].iov_len;
				}
			}
		} else {
			ret =
			    jffs2_flash_write(c, to, vecs[i].iov_len, &thislen,
					      vecs[i].iov_base);
		}
		totlen += thislen;
		if (ret || thislen != vecs[i].iov_len) {
			break;
		}
		to += vecs[i].iov_len;
	}
      writev_out:
	if (retlen) {
		*retlen = totlen;
	}

	return ret;
}

bool jffs2_flash_erase(struct jffs2_sb_info * c,
			   struct jffs2_eraseblock * jeb) {
	struct flash_ioctl_erase e;
	uint32_t err_addr;
	int err;
	uint32_t len = sizeof (e);
	struct jffs2_super_block *sb;
	sb = member_cast_out(c, struct jffs2_super_block, jffs2_sb);

	e.offset = jeb->offset;
	e.len = c->sector_size;
	e.err_address = (uint32_t) &err_addr;

	err = block_dev_ioctl(sb->bdev, FLASH_IOCTL_ERASE, &e, len);

	return (err != ENOERR || e.flasherr != 0);
}


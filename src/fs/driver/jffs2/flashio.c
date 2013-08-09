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

//#include <cyg/io/io.h>
//#include <cyg/io/config_keys.h>
//#include <cyg/io/flash.h>

cyg_bool jffs2_flash_read(struct jffs2_sb_info * c,
			  cyg_uint32 read_buffer_offset, const size_t size,
			  size_t * return_size, unsigned char *write_buffer)
{
//	Cyg_ErrNo err;
//	cyg_uint32 len = size;
//	struct super_block *sb = OFNI_BS_2SFFJ(c);
//
//	//D2(printf("FLASH READ\n"));
//	//D2(printf("read address = %x\n", CYGNUM_FS_JFFS2_BASE_ADDRESS + read_buffer_offset));
//	//D2(printf("write address = %x\n", write_buffer));
//	//D2(printf("size = %x\n", size));
//	err = cyg_io_bread(sb->s_dev, write_buffer, &len, read_buffer_offset);
//
//	*return_size = (size_t) len;
//	return ((err == ENOERR) ? ENOERR : -EIO);
	return 1;
}

cyg_bool jffs2_flash_write(struct jffs2_sb_info * c,
			   cyg_uint32 write_buffer_offset, const size_t size,
			   size_t * return_size, unsigned char *read_buffer)
{

//	Cyg_ErrNo err;
//	cyg_uint32 len = size;
//	struct super_block *sb = OFNI_BS_2SFFJ(c);
//
//	//    D2(printf("FLASH WRITE ENABLED!!!\n"));
//	//    D2(printf("write address = %x\n", CYGNUM_FS_JFFS2_BASE_ADDRESS + write_buffer_offset));
//	//    D2(printf("read address = %x\n", read_buffer));
//	//    D2(printf("size = %x\n", size));
//
//	err = cyg_io_bwrite(sb->s_dev, read_buffer, &len, write_buffer_offset);
//	*return_size = (size_t) len;
//
//	return ((err == ENOERR) ? ENOERR : -EIO);
	return 1;
}

int
jffs2_flash_direct_writev(struct jffs2_sb_info *c, const struct iovec *vecs,
		   unsigned long count, loff_t to, size_t * retlen)
{
//	unsigned long i;
//	size_t totlen = 0, thislen;
//	int ret = 0;
//
//	for (i = 0; i < count; i++) {
//		// writes need to be aligned but the data we're passed may not be
//		// Observation suggests most unaligned writes are small, so we
//		// optimize for that case.
//
//		if (((vecs[i].iov_len & (sizeof (int) - 1))) ||
//		    (((unsigned long) vecs[i].
//		      iov_base & (sizeof (unsigned long) - 1)))) {
//			// are there iov's after this one? Or is it so much we'd need
//			// to do multiple writes anyway?
//			if ((i + 1) < count || vecs[i].iov_len > 256) {
//				// cop out and malloc
//				unsigned long j;
//				ssize_t sizetomalloc = 0, totvecsize = 0;
//				char *cbuf, *cbufptr;
//
//				for (j = i; j < count; j++)
//					totvecsize += vecs[j].iov_len;
//
//				// pad up in case unaligned
//				sizetomalloc = totvecsize + sizeof (int) - 1;
//				sizetomalloc &= ~(sizeof (int) - 1);
//				cbuf = (char *) malloc(sizetomalloc);
//				// malloc returns aligned memory
//				if (!cbuf) {
//					ret = -ENOMEM;
//					goto writev_out;
//				}
//				cbufptr = cbuf;
//				for (j = i; j < count; j++) {
//					memcpy(cbufptr, vecs[j].iov_base,
//					       vecs[j].iov_len);
//					cbufptr += vecs[j].iov_len;
//				}
//				ret =
//				    jffs2_flash_write(c, to, sizetomalloc,
//						      &thislen, (unsigned char *) cbuf);
//				if (thislen > totvecsize)	// in case it was aligned up
//					thislen = totvecsize;
//				totlen += thislen;
//				free(cbuf);
//				goto writev_out;
//			} else {
//				// otherwise optimize for the common case
//				int buf[256 / sizeof (int)];	// int, so int aligned
//				size_t lentowrite;
//
//				lentowrite = vecs[i].iov_len;
//				// pad up in case its unaligned
//				lentowrite += sizeof (int) - 1;
//				lentowrite &= ~(sizeof (int) - 1);
//				memcpy(buf, vecs[i].iov_base, lentowrite);
//
//				ret =
//				    jffs2_flash_write(c, to, lentowrite,
//						      &thislen, (unsigned char *) &buf);
//				if (thislen > vecs[i].iov_len)
//					thislen = vecs[i].iov_len;
//			}	// else
//		} else
//			ret =
//			    jffs2_flash_write(c, to, vecs[i].iov_len, &thislen,
//					      vecs[i].iov_base);
//		totlen += thislen;
//		if (ret || thislen != vecs[i].iov_len)
//			break;
//		to += vecs[i].iov_len;
//	}
//      writev_out:
//	if (retlen)
//		*retlen = totlen;
//
//	return ret;
	return 0;
}

cyg_bool jffs2_flash_erase(struct jffs2_sb_info * c,
			   struct jffs2_eraseblock * jeb)
{
//	cyg_io_flash_getconfig_erase_t e;
//	cyg_flashaddr_t err_addr;
//	Cyg_ErrNo err;
//	cyg_uint32 len = sizeof (e);
//	struct super_block *sb = OFNI_BS_2SFFJ(c);
//
//	e.offset = jeb->offset;
//	e.len = c->sector_size;
//	e.err_address = &err_addr;
//
//	//        D2(printf("FLASH ERASE ENABLED!!!\n"));
//	//        D2(printf("erase address = %x\n", CYGNUM_FS_JFFS2_BASE_ADDRESS + jeb->offset));
//	//        D2(printf("size = %x\n", c->sector_size));
//
//	err = cyg_io_get_config(sb->s_dev, CYG_IO_GET_CONFIG_FLASH_ERASE,
//				&e, &len);
//
//	return (err != ENOERR || e.flasherr != 0);
	return 0;
}


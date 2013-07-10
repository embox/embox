/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    09.07.2013
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <mem/objalloc.h>
#include <fs/journal.h>

static journal_block_t *journal_new_block(journal_t *jp, block_t nr) {
    journal_block_t *jb;

    if (!(jb = malloc(sizeof(journal_block_t)))) {
    	return NULL;
    }

    jb->blocknr = nr;

    /* Allocate data block. */
    if (!(jb->data = malloc(jp->j_blocksize))) {
		free(jb);
		return NULL;
    }

    return jb;
}

journal_t *journal_load(block_dev_t *jdev, block_t start) {
    journal_t *jp;
    journal_superblock_t *sb;
    char buf[4096];

    if (!(jp = malloc(sizeof(journal_t)))) {
    	return NULL;
    }

    memset(jp, 0, sizeof(*jp));
    jp->j_dev        = jdev;
    jp->j_blk_offset = start;
    dlist_head_init(&jp->j_checkpoint_transactions);

    /* Load superblock from the log. */
    if (!jp->j_dev->driver->read(jp->j_dev, buf,
    		1, jp->j_blk_offset)) {
    	return NULL;
    }

    sb = (journal_superblock_t *)buf;

    jp->j_maxlen     = ntohl(sb->s_maxlen);
    jp->j_blocksize  = ntohl(sb->s_blocksize);
    jp->j_first      = ntohl(sb->s_first);
    jp->j_format_version = ntohl(sb->s_header.h_blocktype);
    jp->j_sb_buffer = journal_new_block(jp, jp->j_blk_offset);

    memcpy(jp->j_sb_buffer->data, buf, jp->j_blocksize);
    jp->j_superblock = (journal_superblock_t *)jp->j_sb_buffer->data;

    return jp;
}

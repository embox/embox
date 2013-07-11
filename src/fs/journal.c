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

static int journal_write_desc_blocks(journal_t *jp);
static journal_block_t *journal_new_block(journal_t *jp, block_t nr);
static void journal_free_block(journal_block_t *jb);
static transaction_t *journal_new_trans(journal_t *journal);

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

static transaction_t *journal_new_trans(journal_t *journal) {
    transaction_t *t;

    if (!(t = malloc(sizeof(transaction_t)))) {
    	return NULL;
    }

    memset(t, 0, sizeof(*t));
    t->t_journal = journal;
    t->t_tid     = journal->j_transaction_sequence++;
    t->t_state   = T_RUNNING;
    dlist_head_init(&t->t_reserved_list);
    dlist_head_init(&t->t_buffers);

    return t;
}

handle_t * journal_start(journal_t *jp, int nblocks) {
    handle_t *h;

    if (nblocks <= 0 || nblocks >= JOURNAL_NBLOCKS_PER_TRANS(jp)) {
		errno = EINVAL;
		return NULL;
    }
#if 0
    if (JOURNAL_NBLOCKS_NEEDED(jp, nblocks) > jp->j_free &&
	journal_checkpoint_transaction(jp) != 0) {
    	return NULL;
    }

    if (JOURNAL_NBLOCKS_NEEDED(jp, nblocks) >= JOURNAL_NBLOCKS_PER_TRANS(jp) &&
        journal_commit_transaction(jp) != 0) {
    	return NULL;
    }
#endif

    if ((h = malloc(sizeof(handle_t))) == NULL) {
    	return NULL;
    }

    memset(h, 0, sizeof(*h));
    h->h_transaction    = jp->j_running_transaction;
    h->h_buffer_credits = nblocks;

    h->h_transaction->t_ref++;

    return h;
}

int journal_stop(handle_t *handle) {
    transaction_t *t;
    //journal_t *jp;

    t  = handle->h_transaction;
    //jp = t->t_journal;

    if (--t->t_ref) {
    	//journal_commit_transaction(jp);
    }

    free(handle);
    return 0;
}

int journal_commit_transaction(journal_t *jp) {
    transaction_t *t;

    /* We must have an active transaction. */
    if (jp->j_running_transaction == NULL) {
		errno = EAGAIN;
		return -1;
    }
    /* We cannot commit two transactions at the same time. */
    if (jp->j_committing_transaction != NULL) {
		errno = EBUSY;
		return -1;
    }

    jp->j_committing_transaction = jp->j_running_transaction;
    jp->j_running_transaction    = journal_new_trans(jp);

    if (journal_write_desc_blocks(jp) != 0) {
    	return -1;
    }
//    if (journal_write_commit_block(jp) != 0) {
//    	return -1;
//    }

    dlist_init(&t->t_next);
    dlist_add_prev(&t->t_next, &jp->j_checkpoint_transactions);
    jp->j_committing_transaction = NULL;

    return 0;
}

static int journal_write_desc_blocks(journal_t *jp) {
    transaction_t *t = jp->j_committing_transaction;
    journal_block_t *b, *bnext, *desc_b = NULL;
    journal_header_t *hdr = NULL;
    journal_block_tag_t *tag = NULL;
    struct dlist_head blk_list;
    int tag_cnt = 0;

    t->t_state = T_FLUSH;
    dlist_head_init(&blk_list);

    desc_b = journal_new_block(jp, jp->j_tail);
    dlist_init(&desc_b->b_next);
    dlist_add_next(&desc_b->b_next, &blk_list);

	hdr = (journal_header_t *)desc_b->data;
	hdr->h_magic     = htonl(JFS_MAGIC_NUMBER);
	hdr->h_blocktype = htonl(JFS_DESCRIPTOR_BLOCK);
	hdr->h_sequence  = htonl(t->t_tid);

	memcpy(desc_b->data, hdr, sizeof(hdr));

    /* Fill descriptor block and add it as first element */
    dlist_foreach_entry(b, bnext, &t->t_buffers, b_next) {
    	dlist_del(&b->b_next);

		t->t_log_blocks++;

		/* Fill descriptor with update tags. */
		tag  = (journal_block_tag_t *) (((journal_header_t *)desc_b->data) + 1);
		tag += tag_cnt++;
		tag->t_blocknr = htonl(b->blocknr);

		if (dlist_empty(&t->t_buffers) ||
			tag_cnt >= JOURNAL_NTAGS_PER_DESC(jp)) {
			tag->t_flags = htonl(JFS_FLAG_LAST_TAG);
		} else {
			tag->t_flags = htonl(0);
		}

		dlist_init(&b->b_next);
		dlist_add_next(&b->b_next, &blk_list);
    }

    dlist_foreach_entry(b, bnext, &blk_list, b_next) {
        if (!jp->j_dev->driver->write(jp->j_dev, b->data,
        		1, jp->j_tail)) {
        	goto err;
        }

        jp->j_tail++;
        jp->j_free--;

        dlist_del(&b->b_next);
        journal_free_block(b);
    }

    return 0;

err:
	//free blocks and transaction abort
	return -1;
}

static journal_block_t *journal_new_block(journal_t *jp, block_t nr) {
    journal_block_t *jb;

    if (!(jb = malloc(sizeof(journal_block_t)))) {
    	return NULL;
    }

    jb->blocknr = nr;

    if (!(jb->data = malloc(jp->j_blocksize))) {
		free(jb);
		return NULL;
    }

    return jb;
}

static void journal_free_block(journal_block_t *jb) {
	free(jb->data);
	free(jb);
}

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
static int journal_commit_transaction(journal_t *jp);
static int journal_write_commit_block(journal_t *jp);
static journal_block_t *journal_new_block(journal_t *jp, block_t nr);
static void journal_free_block(journal_t *jp, journal_block_t *jb);
static transaction_t *journal_new_trans(journal_t *journal);
static void journal_free_trans(journal_t *journal, transaction_t *t);
static int journal_write_blocks_list(journal_t *jp, struct dlist_head *blocks, size_t cnt);
static int journal_write_superblock(journal_t *jp);

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

handle_t * journal_start(journal_t *jp, int nblocks) {
    handle_t *h;

    if (nblocks <= 0 || nblocks >= JOURNAL_NBLOCKS_PER_TRANS(jp)) {
		errno = EINVAL;
		return NULL;
    }

    if (JOURNAL_NBLOCKS_NEEDED(jp, nblocks) > jp->j_free &&
    		journal_checkpoint_transactions(jp) != 0) {
    	return NULL;
    }

    if (JOURNAL_NBLOCKS_NEEDED(jp, nblocks) >= JOURNAL_NBLOCKS_PER_TRANS(jp) &&
        journal_commit_transaction(jp) != 0) {
    	return NULL;
    }

    if ((h = malloc(sizeof(handle_t))) == NULL) {
    	return NULL;
    }

    memset(h, 0, sizeof(*h));
    h->h_transaction    = jp->j_running_transaction;
    h->h_transaction->t_outstanding_credits += nblocks;
    h->h_buffer_credits = nblocks;

    h->h_transaction->t_ref++;

    return h;
}

int journal_stop(handle_t *handle) {
    transaction_t *t;
    journal_t *jp;
    int res = 0;

    t  = handle->h_transaction;
    jp = t->t_journal;

    if (0 == --t->t_ref) {
    	res = journal_commit_transaction(jp);
    }
    free(handle);

    return res;
}

int journal_checkpoint_transactions(journal_t *jp) {
    transaction_t *t, *tnext;
    journal_block_t *b, *bnext;

    t = jp->j_committing_transaction;

    /* XXX make optimization: group writing of neighboring blocks */
    dlist_foreach_entry(t, tnext, &jp->j_checkpoint_transactions, t_next) {
    	dlist_foreach_entry(b, bnext, &t->t_buffers, b_next) {
    	    if (0 >= jp->j_dev->driver->write(jp->j_dev, b->data,
    	    		1, b->blocknr)) {
    	    	return -1;
    	    }
    	}

    	jp->j_tail += t->t_log_blocks;
    	jp->j_free += t->t_log_blocks;
    	jp->j_tail_sequence++;

    	if (jp->j_tail > jp->j_last) {
    		jp->j_tail = jp->j_first;
    	}

    	dlist_del(&t->t_next);
    	journal_free_trans(jp, t);
    }

    journal_write_superblock(jp);

    return 0;
}

int journal_dirty_metadata(handle_t *handle, journal_block_t *block) {
	transaction_t *t = handle->h_transaction;

	if (0 == handle->h_buffer_credits) {
		return -1;
	}

	dlist_add_prev(&block->b_next, &t->t_buffers);
	handle->h_buffer_credits--;
	t->t_nr_buffers++;

	return 0;
}

static int journal_write_superblock(journal_t *jp) {
    journal_superblock_t *sb;

	sb = jp->j_superblock;

    /* Update in-memory superblock. */
    sb->s_header.h_magic     = htonl(JFS_MAGIC_NUMBER);
    sb->s_header.h_blocktype = htonl(jp->j_format_version);
    sb->s_blocksize          = htonl(jp->j_blocksize);
    sb->s_maxlen             = htonl(jp->j_maxlen);
    sb->s_first              = htonl(jp->j_first);
    sb->s_sequence           = htonl(jp->j_tail_sequence);
    sb->s_start              = htonl(jp->j_tail);
    sb->s_errno              = htonl(0);

    /* Write it to the log on disk. */
    return jp->j_dev->driver->write(jp->j_dev, jp->j_sb_buffer->data, 1, jp->j_blk_offset);
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

static void journal_free_trans(journal_t *journal, transaction_t *t) {
	journal_block_t *b, *bnext;

	dlist_foreach_entry(b, bnext, &t->t_buffers, b_next) {
		journal_free_block(journal, b);
	}
	free(t);
}

static int journal_commit_transaction(journal_t *jp) {
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
    	/* XXX Ponder on handling such situation. */
    	return -1;
    }
    if (journal_write_commit_block(jp) != 0) {
    	return -1;
    }

    dlist_init(&jp->j_committing_transaction->t_next);
    dlist_add_prev(&jp->j_committing_transaction->t_next, &jp->j_checkpoint_transactions);
    jp->j_committing_transaction = NULL;

    return 0;
}

static int journal_write_desc_blocks(journal_t *jp) {
    transaction_t *t = jp->j_committing_transaction;
    journal_block_t *b, *bnext, *desc_b = NULL;
    journal_header_t *hdr = NULL;
    journal_block_tag_t *tag = NULL;
    int tag_cnt = 0;

    t->t_state = T_FLUSH;

	if (jp->j_head + t->t_nr_buffers > jp->j_last) {
		jp->j_head = jp->j_first;
	}

    desc_b = journal_new_block(jp, jp->j_head);

	hdr = (journal_header_t *)desc_b->data;
	hdr->h_magic     = htonl(JFS_MAGIC_NUMBER);
	hdr->h_blocktype = htonl(JFS_DESCRIPTOR_BLOCK);
	hdr->h_sequence  = htonl(t->t_tid);

    /* Fill descriptor block and add it as first element */
    dlist_foreach_entry(b, bnext, &t->t_buffers, b_next) {
		/* Fill descriptor with update tags. */
		tag  = (journal_block_tag_t *) (((journal_header_t *)desc_b->data) + 1);
		tag += tag_cnt++;
		tag->t_blocknr = htonl(b->blocknr);

		t->t_log_blocks++;

		if (tag_cnt == t->t_nr_buffers ||
			tag_cnt >= JOURNAL_NTAGS_PER_DESC(jp)) {
			tag->t_flags = htonl(JFS_FLAG_LAST_TAG);
		} else {
			tag->t_flags = htonl(0);
		}
    }

    dlist_add_next(dlist_head_init(&desc_b->b_next), &t->t_buffers);

    if (0 >= journal_write_blocks_list(jp, &t->t_buffers, t->t_log_blocks)) {
    	dlist_del(&desc_b->b_next);
    	journal_free_block(jp, desc_b);
    	return -1;
    }

    dlist_del(&desc_b->b_next);

    return 0;
}

static int journal_write_commit_block(journal_t *jp) {
    transaction_t *t = jp->j_committing_transaction;
    journal_block_t *commit_b;
    journal_header_t *hdr;

    t->t_state = T_COMMIT;

    commit_b = journal_new_block(jp, jp->j_head);

    /* Fill commit block. */
    memset(commit_b->data, 0, jp->j_blocksize);
    hdr = (journal_header_t *) commit_b->data;
    hdr->h_magic     = htonl(JFS_MAGIC_NUMBER);
    hdr->h_blocktype = htonl(JFS_COMMIT_BLOCK);
    hdr->h_sequence  = htonl(t->t_tid);

    /* Write it. */
    if (0 >= jp->j_dev->driver->write(jp->j_dev, commit_b->data,
    		1, jp->j_head)) {
    	journal_free_block(jp, commit_b);
    	return -1;
    }
    journal_free_block(jp, commit_b);

    jp->j_head++;
    t->t_log_blocks++;
    t->t_state = T_FINISHED;

    return 0;
}

static journal_block_t *journal_new_block(journal_t *jp, block_t nr) {
    journal_block_t *jb;

    if (!(jb = malloc(sizeof(journal_block_t)))) {
    	return NULL;
    }

    if (!(jb->data = malloc(jp->j_blocksize))) {
		free(jb);
		return NULL;
    }

    jb->blocknr = nr;
    jp->j_free--;

    return jb;
}

static void journal_free_block(journal_t *jp, journal_block_t *jb) {
	free(jb->data);
	free(jb);
	jp->j_free++;
}

static int journal_write_blocks_list(journal_t *jp, struct dlist_head *blocks, size_t cnt) {
	journal_block_t *b, *bnext;
	int ret;
	char *blocks_data = malloc(cnt * jp->j_blocksize);
	char *cur = blocks_data;

	dlist_foreach_entry(b, bnext, blocks, b_next) {
		memcpy(cur, b->data, jp->j_blocksize);
		cur += jp->j_blocksize;
	}

	ret = jp->j_dev->driver->write(jp->j_dev, blocks_data,
    		cnt, jp->j_head);
	if (ret > 0) {
		jp->j_head += cnt;
	}
	free(blocks_data);

	return ret;
}

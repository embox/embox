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
#include <fs/bcache.h>

journal_t *journal_create(journal_fs_specific_t *spec) {
    journal_t *jp;

    if (!(jp = malloc(sizeof(journal_t)))) {
        return NULL;
    }

    memset(jp, 0, sizeof(*jp));
    jp->j_fs_specific = *spec;

    return jp;
}

journal_handle_t *journal_start(journal_t *jp, int nblocks) {
	journal_handle_t *h;

	if (jp->j_fs_specific.trans_freespace(jp, nblocks) < 0) {
		return NULL;
	}

    if ((h = malloc(sizeof(journal_handle_t))) == NULL) {
    	return NULL;
    }

    memset(h, 0, sizeof(*h));
    h->h_transaction    = jp->j_running_transaction;
    h->h_transaction->t_outstanding_credits += nblocks;
    h->h_buffer_credits = nblocks;
    jp->j_free -= nblocks;

    h->h_transaction->t_ref++;

    return h;
}

int journal_stop(journal_handle_t *handle) {
    transaction_t *t;
    journal_t *jp;
    int res = 0;

    t  = handle->h_transaction;
    jp = t->t_journal;

    if (0 == --t->t_ref) {
    	res = jp->j_fs_specific.commit(jp);
    }
    /* And add to free blocks unused by handle */
    jp->j_free += handle->h_buffer_credits - t->t_log_blocks;
    free(handle);

    return res;
}

int journal_checkpoint_transactions(journal_t *jp) {
    transaction_t *t, *tnext;
    journal_block_t *b, *bnext;

    /* XXX make optimization: group writing of neighboring blocks */
    dlist_foreach_entry(t, tnext, &jp->j_checkpoint_transactions, t_next) {
    	dlist_foreach_entry(b, bnext, &t->t_buffers, b_next) {
    	    if (0 >= journal_write_block(jp, b->data, 1, b->blocknr)) {
    	    	return -1;
    	    }
    	}

    	jp->j_tail += t->t_log_blocks;
    	jp->j_tail_sequence++;
    	jp->j_free += t->t_log_blocks;

    	if (jp->j_tail > jp->j_last) {
    		jp->j_tail = jp->j_first;
    	}

    	dlist_del(&t->t_next);
    	journal_free_trans(jp, t);
    }

    jp->j_fs_specific.update(jp);

    return 0;
}

int journal_dirty_block(journal_handle_t *handle, journal_block_t *block) {
	struct buffer_head *bh;
	int i, blkcount;
	transaction_t *t = handle->h_transaction;
	journal_t *jp = t->t_journal;

	if (0 == handle->h_buffer_credits) {
		return -1;
	}

	blkcount = jp->j_blocksize / jp->j_disk_sectorsize;

	/* Write block into cache */
	for (i = 0; i < blkcount; i++) {
		bh = bcache_getblk_locked(jp->j_dev, journal_jb2db(jp, block->blocknr) + i, jp->j_disk_sectorsize);
		{
			if (buffer_new(bh)) {
				buffer_clear_flag(bh, BH_NEW);
			}
			buffer_set_flag(bh, BH_DIRTY);
			memcpy(bh->data, block->data + i * jp->j_disk_sectorsize, jp->j_disk_sectorsize);
		}
		bcache_buffer_unlock(bh);
	}

	dlist_add_prev(&block->b_next, &t->t_buffers);
	handle->h_buffer_credits--;
	t->t_nr_buffers++;

	return 0;
}

journal_block_t *journal_new_block(journal_t *jp, block_t nr) {
    journal_block_t *jb;

    if (!(jb = malloc(sizeof(journal_block_t)))) {
    	return NULL;
    }

    if (!(jb->data = malloc(jp->j_blocksize))) {
		free(jb);
		return NULL;
    }

    dlist_head_init(&jb->b_next);
    jb->blocknr = nr;

    return jb;
}

void journal_free_block(journal_t *jp, journal_block_t *jb) {
	free(jb->data);
	free(jb);
}

transaction_t *journal_new_trans(journal_t *journal) {
    transaction_t *t;

    if (!(t = malloc(sizeof(transaction_t)))) {
    	return NULL;
    }

    memset(t, 0, sizeof(*t));
    t->t_journal = journal;
    t->t_tid     = journal->j_transaction_sequence++;
    t->t_state   = T_RUNNING;
    //dlist_init(&t->t_reserved_list);
    dlist_init(&t->t_buffers);
    dlist_head_init(&t->t_next);

    return t;
}

void journal_free_trans(journal_t *journal, transaction_t *t) {
	journal_block_t *b, *bnext;

	dlist_foreach_entry(b, bnext, &t->t_buffers, b_next) {
		journal_free_block(journal, b);
	}
	free(t);
}

int journal_write_blocks_list(journal_t *jp, struct dlist_head *blocks, size_t cnt) {
	journal_block_t *b, *bnext;
	int ret = 0;

	/* XXX Increase speed up of below writing on hd by grouping blocks */
	dlist_foreach_entry(b, bnext, blocks, b_next) {
		ret += journal_write_block(jp, b->data, 1, jp->j_fs_specific.bmap(jp, jp->j_head++));
	}

	return ret;
}

int journal_write_block(journal_t *jp, char *data, int cnt, int blkno) {
	return jp->j_dev->driver->write(jp->j_dev, data,
    		cnt * jp->j_blocksize, journal_jb2db(jp, blkno));
}

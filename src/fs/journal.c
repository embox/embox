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
#include <mem/misc/slab.h>
#include <mem/sysmalloc.h>
#include <fs/journal.h>
#include <fs/bcache.h>

/**
 * Slab allocator is used here because we don't want to preallocate a memory
 * for journal's internal structures because it is needed only if some device mounted with
 * journaling file system.
 */
CACHE_DEF(journal_cache, journal_t, 0);
CACHE_DEF(journal_block_cache, journal_block_t, 0);
CACHE_DEF(trans_cache, transaction_t, 0);
CACHE_DEF(handle_cache, journal_handle_t, 0);

journal_t *journal_create(journal_fs_specific_t *spec) {
    journal_t *jp;

    if (!(jp = cache_alloc(&journal_cache))) {
        return NULL;
    }

    memset(jp, 0, sizeof(journal_t));
    jp->j_fs_specific = *spec;

    return jp;
}

int journal_delete(journal_t *jp) {
	assert(jp);

	/* Force commit and checkpoint */
	if (jp->j_fs_specific.commit(jp) != 0
			|| journal_checkpoint_transactions(jp) != 0) {
		return -1;
	}

	/* Free new running transaction after commit */
	journal_free_trans(jp, jp->j_running_transaction);

	/* Mark journal as empty */
	jp->j_tail = 0;
	jp->j_transaction_sequence = 1;
	jp->j_fs_specific.update(jp);

	cache_free(&journal_cache, jp);

	return 0;
}

journal_handle_t *journal_start(journal_t *jp, size_t nblocks) {
	journal_handle_t *h;

	assert(jp);
	assert(jp->j_running_transaction);

	if (jp->j_fs_specific.trans_freespace(jp, nblocks) < 0) {
		return NULL;
	}

    if ((h = cache_alloc(&handle_cache)) == NULL) {
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

    assert(handle);
    assert(handle->h_transaction);

    t  = handle->h_transaction;
    jp = t->t_journal;

    if (0 == --t->t_ref) {
    	res = jp->j_fs_specific.commit(jp);
    	/* XXX Ponder on how to handle situation when transaction was uncommitted. */
    	assert(res == 0);
    	journal_checkpoint_transactions(jp);
    }
    /* XXX See the comment in the header to journal_dirty_block.
     *  jp->j_free += handle->h_buffer_credits;
     */
    cache_free(&handle_cache, handle);

    return res;
}

int journal_checkpoint_transactions(journal_t *jp) {
    transaction_t *t;
    journal_block_t *b;
    struct buffer_head *bh;
    int blkcount, i;

    assert(jp);

    blkcount = jp->j_blocksize / jp->j_disk_sectorsize;

    dlist_foreach_entry(t, &jp->j_checkpoint_transactions, t_next) {
    	dlist_foreach_entry(b, &t->t_buffers, b_next) {
    		for (i = 0; i < blkcount; i++) {
    			bh = b->bh[i];

    			bcache_buffer_lock(bh);
    			{
					/* That means - if @a b is not up-to-date, than go to next journal block */
					if (bh->journal_block != b) {
						bcache_buffer_unlock(bh);
						break;
					}
	    			/* If @a b is up-to-date (i.e. @a t is the last transaction that updated @b),
	    			 * than unlock corresponding @a bh */
					buffer_clear_flag(bh, BH_JOURNAL);
					buffer_clear_flag(bh, BH_DIRTY);
    			}
    			bcache_buffer_unlock(bh);
    		}
    		journal_write_block(jp, b->data, 1, b->blocknr);
    	}

    	jp->j_tail += t->t_log_blocks;
    	jp->j_tail = journal_wrap(jp, jp->j_tail);

    	jp->j_tail_sequence++;

    	/**
    	 * XXX See the comment in the header to journal_dirty_block.
    	 * the t_outstanding_credits is a reserve space and the t_log_blocks is actually
    	 * used space, so we should use jp->j_free += t->t_log_blocks;
    	 */
    	jp->j_free += t->t_outstanding_credits;
    	//jp->j_free += t->t_log_blocks;

    	dlist_del(&t->t_next);
    	journal_free_trans(jp, t);
    }

    jp->j_fs_specific.update(jp);

    return 0;
}

int journal_dirty_block(journal_t *jp, journal_block_t *block) {
	struct buffer_head *bh;
	int i, blkcount;
	transaction_t *t = jp->j_running_transaction;

	assert(t);
	assert(block);
	assert(t->t_nr_buffers < t->t_outstanding_credits);

	/* See the comment in the header to journal_dirty_block */
	#if 0
		if (0 == handle->h_buffer_credits) {
			return -1;
		}
		handle->h_buffer_credits--;
	#endif

	blkcount = jp->j_blocksize / jp->j_disk_sectorsize;

	/* Write block into cache */
	for (i = 0; i < blkcount; i++) {
		bh = bcache_getblk_locked(jp->j_dev, journal_jb2db(jp, block->blocknr) + i, jp->j_disk_sectorsize);
		{
			if (buffer_new(bh)) {
				buffer_clear_flag(bh, BH_NEW);
			}
			buffer_set_flag(bh, BH_DIRTY);

			/* Lock block in buffer cache until the last transaction in jp->j_checkpoint_transactions list,
			 * that modified this block, will be checkpointed */
			buffer_set_flag(bh, BH_JOURNAL);

			bh->journal_block = block;
			block->bh[i] = bh;

			/*
			 * TODO
			 * Remove this memcpy() because we can share journal's and buffer cache's data and so do only one copy.
			 * This can be done in many ways and I don't know which is better - share buffer_head, share some common
			 * block_data, or create journal's internal block_data and share it with buffer cache.
			 * So I postpone this optimization.
			 */
			memcpy(bh->data, block->data + i * jp->j_disk_sectorsize, jp->j_disk_sectorsize);
		}
		bcache_buffer_unlock(bh);
	}

	dlist_add_prev(&block->b_next, &t->t_buffers);
	t->t_nr_buffers++;

	return 0;
}

journal_block_t *journal_new_block(journal_t *jp, block_t nr) {
    journal_block_t *jb;

    assert(jp);

    if (!(jb = cache_alloc(&journal_block_cache))) {
    	return NULL;
    }

    /* TODO use kmalloc instead */
    if (!(jb->data = sysmalloc(jp->j_blocksize))) {
    	cache_free(&journal_block_cache, jb);
		return NULL;
    }
    dlist_head_init(&jb->b_next);
    jb->blocknr = nr;

    return jb;
}

void journal_free_block(journal_t *jp, journal_block_t *jb) {
	assert(jp && jb);

	sysfree(jb->data);
	cache_free(&journal_block_cache, jb);
}

transaction_t *journal_new_trans(journal_t *jp) {
    transaction_t *t;

    assert(jp);

    if (!(t = cache_alloc(&trans_cache))) {
    	return NULL;
    }

    memset(t, 0, sizeof(*t));
    t->t_journal = jp;
    t->t_tid     = jp->j_transaction_sequence++;
    t->t_state   = T_RUNNING;

    dlist_init(&t->t_buffers);
    dlist_head_init(&t->t_next);

    return t;
}

void journal_free_trans(journal_t *jp, transaction_t *t) {
	journal_block_t *b;

	assert(jp);
	assert(t);

	dlist_foreach_entry(b, &t->t_buffers, b_next) {
		journal_free_block(jp, b);
	}
	cache_free(&trans_cache, t);
}

int journal_write_blocks_list(journal_t *jp, struct dlist_head *blocks, size_t cnt) {
	journal_block_t *b;
	unsigned long j_head;
	int ret = 0;

	assert(jp);

	/* used to restore previous j_head position if write failed */
	j_head = jp->j_head;

	/* XXX Increase speed up of below writing on hd by grouping blocks */
	dlist_foreach_entry(b, blocks, b_next) {
		jp->j_head = journal_wrap(jp, jp->j_head++);
		ret = journal_write_block(jp, b->data, 1,
				jp->j_fs_specific.bmap(jp, jp->j_head));
		if (ret < 0) {
			jp->j_head = j_head;
			return ret;
		}
	}

	return 0;
}

int journal_write_block(journal_t *jp, char *data, int cnt, int blkno) {
	assert(jp);
	return jp->j_dev->driver->write(jp->j_dev, data,
    		cnt * jp->j_blocksize, journal_jb2db(jp, blkno));
}

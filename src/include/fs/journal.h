/**
 * @file
 * @brief Journal's common API
 *
 * @author  Alexander Kalmuk
 * @date    09.07.2013
 */

#ifndef FS_JOURNAL_H
#define FS_JOURNAL_H

#include <lib/libds/dlist.h>
#include <drivers/block_dev.h>
#include <stdint.h>

typedef unsigned int block_t;

/**
 * typedef handle_t - The handle_t type represents a single
 *                    atomic update being performed by some process.
 *
 * All filesystem modifications made by the process go
 * through this handle.  Recursive operations (such as quota operations)
 * are gathered into a single update.
 *
 * The buffer credits field is used to account for journaled buffers
 * being modified by the running process.  To ensure that there is
 * enough log space for all outstanding operations, we need to limit the
 * number of outstanding buffers possible at any time.  When the
 * operation completes, any buffer credits not used are credited back to
 * the transaction, so that at all times we know how many buffers the
 * outstanding updates on a transaction might possibly touch..
 *
 * This is an opaque datatype.
 */
typedef struct journal_handle_s journal_handle_t;       /* Atomic operation type */

/**
 * typedef transaction_t - Represents a compound set of atomic updates.
 *
 * This is an opaque datatype.
 */
typedef struct transaction_s transaction_t;  /* Compound transaction type */

/**
 * typedef journal_t - The journal_t maintains all of the journaling
 *                     state information for a single filesystem.
 *
 * journal_t is linked to from the fs superblock structure.
 *
 * We use the journal_t to keep track of all outstanding transaction
 * activity on the filesystem, and to manage the state of the log
 * writing process.
 *
 * This is an opaque datatype.
 */
typedef struct journal_s journal_t;      /* Journal control structure */

/**
 * Represents a single in-memory block.
 */
typedef struct journal_block_s {
	/**
	 * XXX suppose the better solution will be add *bh not *bh[8]
	 */
	struct buffer_head *bh[8]; /** Corresponding buffer_heads (up to 8) */
    block_t blocknr;	 /** Block number. */
    char *data;		 /** Pointer to in-memory data. */
    struct dlist_head b_next; /** Linked list entry. */
} journal_block_t;


/*********** File system specific ***********/

/**
 * Below functions should be implemented for each concrete file system's driver.
 */

/**
 * Maps journal's block into filesystem block.
 *
 * @param jp    - journal
 * @param block - block number in journal address space (0..jp->max_len - 1)
 *
 * @return number of disk block
 */
typedef uint32_t (*journal_bmap_t)(journal_t *jp, block_t block);

/**
 * Commit current running transaction.
 *
 * @param jp    - journal
 *
 * @return error code
 * @retval 0 on success
 */
typedef int (*journal_commit_t)(journal_t *jp);
/**
 * Update journal's superblock after checkpointing.
 */
typedef int (*journal_update_t)(journal_t *jp);

/**
 * Check if there are free space for nblocks.
 */
typedef int (*journal_trans_freespace_t)(journal_t *jp, size_t nblocks);

typedef struct journal_fs_specific_s {
	journal_commit_t commit;
	journal_update_t update;
    journal_bmap_t bmap;
    journal_trans_freespace_t trans_freespace;
	void *data;
} journal_fs_specific_t;

/*********** File system specific ***********/


/**
 * struct handle_s - The handle_s type is the concrete type associated with
 *                   handle_t.
 * @h_transaction: Which compound transaction is this update a part of?
 * @h_buffer_credits: Number of remaining buffers we are allowed to dirty.
 */
struct journal_handle_s {
    transaction_t *h_transaction;
    int h_buffer_credits;
};

/*
 * The transaction_t type is the guts of the journaling mechanism.  It
 * tracks a compound transaction through its various states:
 *
 * RUNNING:     accepting new updates
 * LOCKED:      Updates still running but we don't accept new ones
 * RUNDOWN:     Updates are tidying up but have finished requesting
 *              new buffers to modify (state not used for now)
 * FLUSH:       All updates complete, but we are still writing to disk
 * COMMIT:      All data on disk, writing commit record
 * FINISHED:    We still have to keep the transaction for checkpointing.
 *
 * The transaction keeps track of all of the buffers modified by a
 * running transaction, and all of the buffers committed but not yet
 * flushed to home for finished transactions.
 */
struct transaction_s {
    /* Pointer to the journal for this transaction. [no locking] */
    journal_t *t_journal;

    /* Sequence number for this transaction [no locking] */
    uint32_t t_tid;

    /*
     * Transaction's current state
     * [no locking - only kjournald alters this]
     * FIXME: needs barriers
     * KLUDGE: [use j_state_lock]
     */
    enum {
        T_RUNNING,
        T_LOCKED,
        T_RUNDOWN,
        T_FLUSH,
        T_COMMIT,
        T_FINISHED
    } t_state;

    /*
     * Where in the log does this transaction's commit start? [no locking]
     */
    unsigned long t_log_start;

    /* Number of committed blocks used in the log by this transaction. */
    unsigned long t_log_blocks;

    /* Number of buffers on the t_buffers list [j_list_lock] */
    int t_nr_buffers;

    /*
     * Doubly-linked circular list of all buffers reserved but not yet
     * modified by this transaction [j_list_lock]
     */
    struct dlist_head t_reserved_list;

    /*
     * Doubly-linked circular list of all metadata buffers owned by this
     * transaction [j_list_lock]
     */
    struct dlist_head t_buffers;

    /*
     * Number of buffers reserved for use by all handles in this transaction
     * handle but not yet modified. [t_handle_lock]
     */
    int t_outstanding_credits;

    /* Reference count of handlers [j_list_lock] */
    int t_ref;

    /*
     * Forward and backward links for the circular list of all transactions
     * awaiting checkpoint. [j_list_lock]
     */
    struct dlist_head t_next;
};

/**
 * struct journal_s - The journal_s type is the concrete type
 *                    associated with journal_t.
 */
struct journal_s {
	/**
	 * Journal's filesystem specific operations.
	 */
	journal_fs_specific_t j_fs_specific;

    /*
     * Transactions: The current running transaction...
     * [j_state_lock] [caller holding open handle]
     */
    transaction_t *j_running_transaction;

    /*
     * the transaction we are pushing to disk
     * [j_state_lock] [caller holding open handle]
     */
    transaction_t *j_committing_transaction;

    /*
     * ... and a linked circular list of all transactions waiting for
     * checkpointing. [j_list_lock]
     */
    struct dlist_head j_checkpoint_transactions;

    /*
     * Journal head: identifies the first unused block in the journal.
     * [j_state_lock]
     */
    unsigned long j_head;

    /*
     * Journal tail: identifies the oldest still-used block in the journal.
     * [j_state_lock]
     */
    unsigned long j_tail;

    /*
     * Journal free: how many free blocks are there in the journal?
     * [j_state_lock]
     */
    unsigned long j_free;

    /*
     * Journal start and end: the block numbers of the first usable block
     * and one beyond the last usable block in the journal. [j_state_lock]
     */
    unsigned long j_first;
    unsigned long j_last;

    /*
     * Device, blocksize and starting block offset for the location where we
     * store the journal.
     */
    struct block_dev *j_dev;
    size_t j_blocksize;
    size_t j_disk_sectorsize;
    block_t j_blk_offset;

    /* Total maximum capacity of the journal region on disk. */
    size_t j_maxlen;

    /*
     * Sequence number of the oldest transaction in the log [j_state_lock]
     */
    uint32_t j_tail_sequence;

     /*
     * Sequence number of the next transaction to grant [j_state_lock]
     */
    uint32_t j_transaction_sequence;
};

extern journal_t *journal_create(journal_fs_specific_t *spec);
extern int journal_delete(journal_t *jp);
extern journal_handle_t * journal_start(journal_t *jp, size_t nblocks);
extern int journal_stop(journal_handle_t *handle);

extern journal_block_t *journal_new_block(journal_t *jp, block_t nr);
extern void journal_free_block(journal_t *jp, journal_block_t *jb);

extern transaction_t *journal_new_trans(journal_t *journal);
extern void journal_free_trans(journal_t *journal, transaction_t *t);
extern int journal_checkpoint_transactions(journal_t *jp);

/**
 * TODO
 * When an atomic handler starts by the call of journal_start(jp, nblocks), journal
 * reserves nblocks for the current running transaction. nblocks - is the lower upper limit
 * of blocks that an atomic handler is going to modify. Actually nblocks does'nt correspond
 * to real count of blocks that an atomic handler will modify (real one is lower).
 * Therefore we should keep track of really modified blocks to prevent themselves from
 * waste the journal space. So signature of this function should be
 * int journal_dirty_block(journal_handle_t *handle, journal_block_t *block)
 */
extern int journal_dirty_block(journal_t *jp, journal_block_t *block);

extern int journal_write_blocks_list(journal_t *jp, struct dlist_head *blocks, size_t cnt);
extern int journal_write_block(journal_t *jp, char *data, int cnt, int blkno);

static inline int journal_wrap(journal_t *jp, int var) {
	if (var >= jp->j_last) {
		var -= jp->j_last - jp->j_first;
	}
	return var;
}

#define journal_db2jb(jp, block) \
		(block / (jp->j_blocksize / jp->j_disk_sectorsize))

#define journal_jb2db(jp, block) \
		(block * (jp->j_blocksize / jp->j_disk_sectorsize))

#endif /* FS_JOURNAL_H */

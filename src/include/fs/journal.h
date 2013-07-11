/**
 * @file
 * @brief Minix journal structures adapted for Embox
 *
 * @author  Alexander Kalmuk
 * @date    09.07.2013
 */

/*
 * This file is part of libjournal.
 *
 * Written by Niek Linnenbank <nieklinnenbank@gmail.com>
 */

#ifndef _LIBJOURNAL_H
#define _LIBJOURNAL_H

#include <util/dlist.h>
#include <embox/block_dev.h>
#include <stdint.h>

/**
 * Maximum number of tags per block descriptor.
 */
#define JOURNAL_NTAGS_PER_DESC(jp) ( ((jp)->j_blocksize - sizeof(journal_header_t)) \
				    / (sizeof(journal_block_tag_t)))

/**
 * Maximum number of blocks per transaction.
 */
#define JOURNAL_NBLOCKS_PER_TRANS(jp) ((jp)->j_maxlen / 4)

/**
 * @brief Maximum number of total on-disk blocks needed in the log
 *        to store the given number of user blocks.
 */
#define JOURNAL_NBLOCKS_NEEDED(jp,num) \
    ((((jp)->j_running_transaction->t_outstanding_credits) + (num)) + \
    (((((jp)->j_running_transaction->t_outstanding_credits) + (num)) / \
       JOURNAL_NTAGS_PER_DESC(jp)) + 2))

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
typedef struct handle_s handle_t;       /* Atomic operation type */

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
    block_t blocknr;	 /** Block number. */
    char *data;		 /** Pointer to in-memory data. */
    struct dlist_head b_next; /** Linked list entry. */
} journal_block_t;

/**
 * struct handle_s - The handle_s type is the concrete type associated with
 *                   handle_t.
 * @h_transaction: Which compound transaction is this update a part of?
 * @h_buffer_credits: Number of remaining buffers we are allowed to dirty.
 * @h_ref: Reference count on this handle
 */
struct handle_s {
    transaction_t *h_transaction;
    int h_buffer_credits;
    int h_ref;
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

    /* Reference count of handlers */
    int t_ref;

    /*
     * Forward and backward links for the circular list of all transactions
     * awaiting checkpoint. [j_list_lock]
     */
    struct dlist_head t_next;
};

/*
 * On-disk structures
 */

#define JFS_MAGIC_NUMBER 0xc03b3998U /* The first 4 bytes of /dev/random! */

/*
 * Descriptor block types:
 */

#define JFS_DESCRIPTOR_BLOCK    1
#define JFS_COMMIT_BLOCK        2
#define JFS_SUPERBLOCK_V1       3
#define JFS_SUPERBLOCK_V2       4
#define JFS_REVOKE_BLOCK        5

/*
 * Standard header for all descriptor blocks:
 */
typedef struct journal_header_s {
	uint32_t h_magic;
	uint32_t h_blocktype;
	uint32_t h_sequence;
} journal_header_t;

/*
 * The block tag: used to describe a single buffer in the journal.
 */
typedef struct journal_block_tag_s {
	uint32_t t_blocknr; /* The on-disk block number */
	uint32_t t_flags;   /* See below */
} journal_block_tag_t;

/*
 * Definitions for the journal tag flags word
 */
#define JFS_FLAG_ESCAPE         1       /* on-disk block is escaped */
#define JFS_FLAG_SAME_UUID      2       /* block has same uuid as previous */
#define JFS_FLAG_DELETED        4       /* block deleted by this transaction */
#define JFS_FLAG_LAST_TAG       8       /* last tag in this descriptor block */

/*
 * The journal superblock.  All fields are in big-endian byte order.
 */
typedef struct journal_superblock_s {
    /* 0x0000 */
    journal_header_t s_header;

    /* 0x000C */
    /* Static information describing the journal */
    uint32_t s_blocksize;            /* journal device blocksize */
    uint32_t s_maxlen;               /* total blocks in journal file */
    uint32_t s_first;                /* first block of log information */

    /* 0x0018 */
    /* Dynamic information describing the current state of the log */
    uint32_t s_sequence;             /* first commit ID expected in log */
    uint32_t s_start;                /* blocknr of start of log */

    /* 0x0020 */
    /* Error value, as set by journal_abort(). */
    uint32_t s_errno;

    /* 0x0024 */
    /* Remaining fields are only valid in a version-2 superblock */
    uint32_t s_feature_compat;       /* compatible feature set */
    uint32_t s_feature_incompat;     /* incompatible feature set */
    uint32_t s_feature_ro_compat;    /* readonly-compatible feature set */
    /* 0x0030 */
    uint8_t  s_uuid[16];             /* 128-bit uuid for journal */
    /* 0x0040 */
    uint32_t s_nr_users;             /* Nr of filesystems sharing log */
    uint32_t s_dynsuper;             /* Blocknr of dynamic superblock copy*/

    /* 0x0048 */
    uint32_t s_max_transaction;      /* Limit of journal blocks per trans.*/
    uint32_t s_max_trans_data;       /* Limit of data blocks per trans. */

    /* 0x0050 */
    uint32_t s_padding[44];

    /* 0x0100 */
    uint32_t  s_users[16*48];         /* ids of all fs'es sharing the log */
    /* 0x0400 */
}
journal_superblock_t;

/**
 * struct journal_s - The journal_s type is the concrete type
 *                    associated with journal_t.
 */
struct journal_s {
    /* The superblock buffer */
    journal_block_t *j_sb_buffer;
    journal_superblock_t *j_superblock;

    /* Version of the superblock format */
    int j_format_version;

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
    block_dev_t *j_dev;
    size_t j_blocksize;
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

    /*
     * Journal uuid: identifies the object (filesystem, LVM volume etc)
     * backed by this journal.  This will eventually be replaced by an array
     * of uuids, allowing us to index multiple devices within a single
     * journal and to perform atomic updates across them.
     */
    uint8_t j_uuid[16];
};

extern journal_t *journal_load(block_dev_t *jdev, block_t start);
extern handle_t * journal_start(journal_t *jp, int nblocks);
extern int journal_stop(handle_t *handle);

#endif /* _LIBJOURNAL_H */

/**
 * @file
 * @brief Ext3 journal
 *
 * @date 23.07.2013
 * @author Alexander Kalmuk
 */


#ifndef EXT3_JOURNAL_H_
#define EXT3_JOURNAL_H_

#include <fs/journal.h>
#include <fs/ext2.h>

/**
 * Maximum number of tags per block descriptor.
 */
#define EXT3_JOURNAL_NTAGS_PER_DESC(jp) (((jp)->j_blocksize - sizeof(ext3_journal_header_t)) \
				    / (sizeof(ext3_journal_block_tag_t)))

/**
 * Maximum number of blocks per transaction.
 */
#define EXT3_JOURNAL_NBLOCKS_PER_TRANS(jp) ((jp)->j_maxlen / 4)

/**
 * @brief Maximum number of total on-disk blocks needed in the log
 *        to store the given number of user blocks.
 */
#define EXT3_JOURNAL_NBLOCKS_NEEDED(jp,num) \
    ((((jp)->j_running_transaction->t_outstanding_credits) + (num)) + \
    (((((jp)->j_running_transaction->t_outstanding_credits) + (num)) / \
       EXT3_JOURNAL_NTAGS_PER_DESC(jp)) + 2))

/*
 * On-disk structures
 */

#define EXT3_MAGIC_NUMBER 0xc03b3998U /* The first 4 bytes of /dev/random! */

/*
 * Descriptor block types:
 */

#define EXT3_DESCRIPTOR_BLOCK    1
#define EXT3_COMMIT_BLOCK        2
#define EXT3_SUPERBLOCK_V1       3
#define EXT3_SUPERBLOCK_V2       4
#define EXT3_REVOKE_BLOCK        5

/*
 * Standard header for all descriptor blocks:
 */
typedef struct ext3_journal_header_s {
	uint32_t h_magic;
	uint32_t h_blocktype;
	uint32_t h_sequence;
} ext3_journal_header_t;

/*
 * The block tag: used to describe a single buffer in the journal.
 */
typedef struct ext3_journal_block_tag_s {
	uint32_t t_blocknr; /* The on-disk block number */
	uint32_t t_flags;   /* See below */
} ext3_journal_block_tag_t;

/*
 * Definitions for the journal tag flags word
 */
#define EXT3_FLAG_ESCAPE         1       /* on-disk block is escaped */
#define EXT3_FLAG_SAME_UUID      2       /* block has same uuid as previous */
#define EXT3_FLAG_DELETED        4       /* block deleted by this transaction */
#define EXT3_FLAG_LAST_TAG       8       /* last tag in this descriptor block */

/*
 * The journal superblock.  All fields are in big-endian byte order.
 */
typedef struct ext3_journal_superblock_s {
    /* 0x0000 */
	ext3_journal_header_t s_header;

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
} ext3_journal_superblock_t;

typedef struct ext3_journal_specific_s {
	journal_block_t *j_sb_buffer;
	ext3_journal_superblock_t *j_superblock;
	/* Version of the superblock format */
	int j_format_version;
	struct ext2fs_dinode *ext3_journal_inode;
    /*
     * Journal uuid: identifies the object (filesystem, LVM volume etc)
     * backed by this journal.  This will eventually be replaced by an array
     * of uuids, allowing us to index multiple devices within a single
     * journal and to perform atomic updates across them.
     */
    uint8_t j_uuid[16];
} ext3_journal_specific_t;

extern struct ext3_journal_specific_s *ext3_journal_cache_alloc(void);

extern void ext3_journal_cache_free(struct ext3_journal_specific_s *ext3_spec);

/**
 * Count of blocks needed for transaction that is going to modify 1 block of data.
 * 1 inode
 * 2 bitmaps
 * up to 3 indirection blocks
 * 1 superblock
 * 1 group descriptor
 */
#define EXT3_TRANS_SINGLEDATA_BLOCK 8

/**
 * Count of blocks needed for transaction that is going to modify N blocks of data.
 * N blocks of data
 * 1 indirect block
 * 2 dindirect
 * 3 tindirect
 * N bitmaps
 * N group descriptor
 * 1 inode
 * 1 superblock
 */
static inline int ext3_trans_blocks(int nblocks) {
	return (nblocks == 1 ? EXT3_TRANS_SINGLEDATA_BLOCK : 3 * nblocks + 6 + 2);
}

extern int ext3_journal_commit(journal_t *jp);
extern int ext3_journal_update(journal_t *jp);
extern uint32_t ext3_journal_bmap(journal_t *jp, block_t block);
extern int ext3_journal_trans_freespace(journal_t *jp, size_t nblocks);

#endif /* EXT3_JOURNAL_H_ */

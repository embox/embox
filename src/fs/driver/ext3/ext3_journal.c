/**
 * @file
 * @brief Ext3 journal
 *
 * @date 23.07.2013
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <mem/objalloc.h>
#include <arpa/inet.h>
#include <fs/vfs.h>
#include <fs/ext2.h>
#include "ext3_journal.h"
#include <string.h>

static int journal_write_desc_blocks(journal_t *jp);
static int journal_write_commit_block(journal_t *jp);

int ext3_journal_load(journal_t *jp, block_dev_t *jdev, block_t start) {
    ext3_journal_superblock_t *sb;
    ext3_journal_specific_t *spec = (ext3_journal_specific_t *)jp->j_fs_specific.data;
    char buf[4096];

    assert(jp);
    assert(jdev);

    jp->j_dev = jdev;
    jp->j_disk_sectorsize = block_dev_ioctl(jdev, IOCTL_GETBLKSIZE, NULL, 0);

    assert(jp->j_disk_sectorsize >= 512);

    /* Load superblock from the log. */
    if (!jp->j_dev->driver->read(jp->j_dev, buf,
    		4096, start)) {
    	return -1;
    }

    sb = (ext3_journal_superblock_t *)buf;

    jp->j_maxlen         = ntohl(sb->s_maxlen);
    jp->j_blocksize      = ntohl(sb->s_blocksize);
    jp->j_blk_offset     = journal_db2jb(jp, start);
    jp->j_first          = 1;
    jp->j_last           = jp->j_maxlen;
    spec->j_format_version = ntohl(sb->s_header.h_blocktype);

    /* Initialize transaction specific data */
    jp->j_head                 = jp->j_first;
    jp->j_tail                 = jp->j_head;
    jp->j_free                 = jp->j_last - jp->j_first;
    jp->j_tail_sequence        = 1;
    jp->j_transaction_sequence = 1;
    jp->j_running_transaction  = journal_new_trans(jp);
    dlist_init(&jp->j_checkpoint_transactions);

    /* Update journal superblock */
    spec->j_sb_buffer  = journal_new_block(jp, jp->j_blk_offset);
    spec->j_superblock = (ext3_journal_superblock_t *)spec->j_sb_buffer->data;
    memcpy(spec->j_sb_buffer->data, buf, jp->j_blocksize);
    jp->j_fs_specific.update(jp);

    return 0;
}

int ext3_journal_commit(journal_t *jp) {
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

    dlist_add_prev(&jp->j_committing_transaction->t_next, &jp->j_checkpoint_transactions);
    jp->j_committing_transaction = NULL;

    return 0;
}

int ext3_journal_update(journal_t *jp) {
    ext3_journal_superblock_t *sb;
    ext3_journal_specific_t *spec = (ext3_journal_specific_t *)jp->j_fs_specific.data;

    sb = ((ext3_journal_specific_t *)jp->j_fs_specific.data)->j_superblock;

    /* Update in-memory superblock. */
    sb->s_header.h_magic     = htonl(EXT3_MAGIC_NUMBER);
    sb->s_header.h_blocktype = htonl(spec->j_format_version);
    sb->s_blocksize          = htonl(jp->j_blocksize);
    sb->s_maxlen             = htonl(jp->j_maxlen);
    sb->s_first              = htonl(jp->j_first);
    sb->s_sequence           = htonl(jp->j_tail_sequence);
    sb->s_start              = htonl(jp->j_tail);
    sb->s_errno              = htonl(0);

    /* Write it to the log on disk. */
    return journal_write_block(jp, spec->j_sb_buffer->data, 1, jp->j_blk_offset);
}

uint32_t ext3_journal_bmap(journal_t *jp, block_t block) {
	uint32_t buf[jp->j_blocksize / sizeof(uint32_t)];
	ext3_journal_specific_t *spec = (ext3_journal_specific_t *)jp->j_fs_specific.data;

	/* TODO handle also dindirect and tindirect blocks indeed this assert */
	assert(block < NDADDR + jp->j_blocksize / sizeof(uint32_t));

	if (block < NDADDR) {
		return spec->ext3_journal_inode->i_block[block];
	}

	jp->j_dev->driver->read(jp->j_dev, (char *)buf, jp->j_blocksize,
			2 * spec->ext3_journal_inode->i_block[NDADDR]);

	return buf[block - NDADDR];
}

int ext3_journal_trans_freespace(journal_t *jp, size_t nblocks) {
    if (EXT3_JOURNAL_NBLOCKS_NEEDED(jp, nblocks) > EXT3_JOURNAL_NBLOCKS_PER_TRANS(jp)) {
    	return -1;
    }

    if (EXT3_JOURNAL_NBLOCKS_NEEDED(jp, nblocks) > jp->j_free) {
    	journal_checkpoint_transactions(jp);
    	if (EXT3_JOURNAL_NBLOCKS_NEEDED(jp, nblocks) > jp->j_free) {
    		return -1;
    	}
    }

    return 0;
}

static int journal_write_desc_blocks(journal_t *jp) {
    transaction_t *t = jp->j_committing_transaction;
    journal_block_t *b, *bnext, *desc_b = NULL;
    ext3_journal_header_t *hdr = NULL;
    ext3_journal_block_tag_t *tag = NULL;

    t->t_state = T_FLUSH;

	if (jp->j_head + t->t_nr_buffers > jp->j_last) {
		jp->j_head = jp->j_first;
	}

    desc_b = journal_new_block(jp, jp->j_head);

	hdr = (ext3_journal_header_t *)desc_b->data;
	hdr->h_magic     = htonl(EXT3_MAGIC_NUMBER);
	hdr->h_blocktype = htonl(EXT3_DESCRIPTOR_BLOCK);
	hdr->h_sequence  = htonl(t->t_tid);

	tag  = (ext3_journal_block_tag_t *) (((ext3_journal_header_t *)desc_b->data) + 1);

    /* Fill descriptor block and add it as first element */
    dlist_foreach_entry(b, bnext, &t->t_buffers, b_next) {
		/* Fill descriptor with update tags. */
		t->t_log_blocks++;

		if (t->t_log_blocks == t->t_nr_buffers ||
				t->t_log_blocks >= EXT3_JOURNAL_NTAGS_PER_DESC(jp)) {
			tag->t_flags = htonl(EXT3_FLAG_LAST_TAG);
		} else {
			tag->t_flags = htonl(EXT3_FLAG_SAME_UUID); /* XXX check it */
		}

		(tag++)->t_blocknr = htonl(b->blocknr);
    }
    /* desc_b */
    t->t_log_blocks++;

    /**
     * TODO journal must support multiple descriptor blocks per transaction
     */
    assert(t->t_log_blocks <= EXT3_JOURNAL_NTAGS_PER_DESC(jp));

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
    ext3_journal_header_t *hdr;

    t->t_state = T_COMMIT;

    commit_b = journal_new_block(jp, jp->j_head);

    /* Fill commit block. */
    memset(commit_b->data, 0, jp->j_blocksize);
    hdr = (ext3_journal_header_t *) commit_b->data;
    hdr->h_magic     = htonl(EXT3_MAGIC_NUMBER);
    hdr->h_blocktype = htonl(EXT3_COMMIT_BLOCK);
    hdr->h_sequence  = htonl(t->t_tid);

    /* Write it. */
    if (0 >= journal_write_block(jp, commit_b->data, 1, jp->j_fs_specific.bmap(jp, jp->j_head))) {
    	journal_free_block(jp, commit_b);
    	return -1;
    }
    journal_free_block(jp, commit_b);
    jp->j_head++;

    t->t_log_blocks++;
    t->t_state = T_FINISHED;

    return 0;
}

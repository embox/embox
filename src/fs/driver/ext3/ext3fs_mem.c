/**
 * @file
 * @brief ext3 file system
 *
 * @date 12.03.2013
 * @author Andrey Gazukin
 */

#include <mem/objalloc.h>

#include <framework/mod/options.h>

#include "ext3_journal.h"

#define EXT3_JOURNAL_CNT 16 // XXX to Mybuild

OBJALLOC_DEF(ext3_journal_cache, ext3_journal_specific_t, EXT3_JOURNAL_CNT);

struct ext3_journal_specific_s *ext3_journal_cache_alloc(void) {
	return  objalloc(&ext3_journal_cache);
}

void ext3_journal_cache_free(struct ext3_journal_specific_s *ext3_spec) {
	objfree(&ext3_journal_cache, ext3_spec);
}

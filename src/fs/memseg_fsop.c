#include "common.h"
#include "fs/fsop.h"
#include "fs/memseg.h"


extern SEGMENT mm_segtable[MAX_NSEGMENTS];

// TODO: probably we should not provide init() in fs interace, and initialize mem manger globally
static BOOL init()
{
	return memseg_init();
}

static size_t list_files (FLIST_ITEM* out_flist, int max_nitems)
{
	//TODO: max_nitems check
	short sidx, flist_idx = 0;
	for (sidx=0; sidx<MAX_NSEGMENTS; sidx++)
		if (IS_SEGTBL_ITEM_USED(mm_segtable,sidx)) {
			strcpy (out_flist[flist_idx].name, mm_segtable[sidx].name);
			out_flist[flist_idx].size = mm_segtable[sidx].end - mm_segtable[sidx].start;
			flist_idx++;
		}
	return flist_idx;
}

static short get_file_idx (const char* file_name)
{
	return get_seg_idx (file_name);
}


static BOOL delete_file (const char *file_name)
{
	short sidx = get_seg_idx (file_name);
	if (!IS_SEGTBL_ITEM_USED(mm_segtable, sidx)) {
		TRACE ("invalid name!\n");
		return FALSE;
	}

	RELEASE_SEGTBL_ITEM(mm_segtable,sidx);
	return TRUE;
}

static size_t file_read (short fidx, long start_offset, void *buf, size_t nbytes)
{
	int bytesleft = mm_segtable[fidx].end - mm_segtable[fidx].start - start_offset;
	if (bytesleft<=0) {
		//TRACE("EOF\n", start_offset);
		return -1;
	}
	TRACE("reading %d bytes from offset 0x%08x\n", nbytes, start_offset);
	size_t nrbytes = (nbytes > bytesleft) ? bytesleft : nbytes;
	memcpy(buf, mm_segtable[fidx].start+start_offset, nrbytes);
	return nrbytes;
}

static size_t file_write (short fidx, size_t start_offset, const void *buf, size_t nbytes)
{
	void* wrstart = mm_segtable[fidx].start+start_offset;
	short idx;
	TRACE("wrstart %08x ;  end %08x\n",wrstart, wrstart+nbytes);
	for (idx=0; idx<MAX_NSEGMENTS; idx++)
		if (IS_SEGTBL_ITEM_USED(mm_segtable,idx))
			if ((idx!=fidx) && is_overlap(wrstart, wrstart+nbytes, mm_segtable[idx])) {
				TRACE("segments could not overlap!\n");
				return -1;
			}

	memcpy(wrstart, buf, nbytes);
	mm_segtable[fidx].end = wrstart+nbytes;
	return nbytes;
}



FSOP memseg_fsop ={
	(FS_INIT_FUNC) init,

	(FS_LISTFILES_FUNC) list_files,
	(FS_GETFILEIDX_FUNC) get_file_idx,

	(FILE_CREATE_FUNC) NULL, // use create_segment, interface for segment creation differs
	(FILE_DELETE_FUNC) delete_file,

	(FILE_READ_FUNC) file_read,
	(FILE_WRITE_FUNC) file_write,

	(FS_FORMAT_FUNC) NULL,
	(FS_SYNC_FUNC) NULL,

	(FS_GETCAPACITY_FUNC) NULL,
	(FS_GETFREESPACE_FUNC) NULL
};


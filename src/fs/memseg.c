
#include "fs/memseg.h"
#include "string.h"

static int piggysize;

SEGMENT mm_segtable[MAX_NSEGMENTS];


bool memseg_init()
{
	int i;
	TRACE("mm init..\n");

	extern char _piggy_end, _piggy_start, _pigmon_start, _pigmon_end;

	extern char _text_start, _text_end, _data_start, _data_end, _trap_table;

	short sidx;
	for (sidx=0; sidx<MAX_NSEGMENTS; sidx++)
		RELEASE_SEGTBL_ITEM(mm_segtable,sidx);

	create_segment ("piggy", &_piggy_start, &_piggy_end);
	create_segment ("pigmon", &_pigmon_start, &_pigmon_end);
	create_segment ("traptable", &_trap_table, &_text_start);
	create_segment ("text", &_text_start, &_text_end);
	create_segment ("data", &_data_start, &_data_end);

	return true;
}

inline bool is_overlap (void* start, void* end, SEGMENT seg)
{
	//LOG_DEBUG("req: %08x | %08x \n", start, end);
	//LOG_DEBUG("seg: %08x | %08x \n", seg.start, seg.end);
	if ((start>=seg.start) && (start<seg.end)) {
		//LOG_DEBUG("1 (start>=seg.start) && (start<seg.end)\n");
		return true;
	}
	if ((end>seg.start) && (end<=seg.end)) {
		//LOG_DEBUG("2 ((end>seg.start) && (end<=seg.end))\n");
		return true;
	}
	if ((start<seg.start) && (end>seg.end)) {
		//LOG_DEBUG("3 (start<seg.start) && (end>seg.end)\n");
		return true;
	}
	return false;
}

short get_seg_idx (const char* seg_name)
{
	short idx;
	for (idx=0; idx<MAX_NSEGMENTS; idx++)
		if (IS_SEGTBL_ITEM_USED(mm_segtable,idx))
			if (strcmp(mm_segtable[idx].name,seg_name) == 0)
				return idx;

	return IDX_INVALID;
}

size_t list_segments (SEGMENT* out_slist, int max_nitems)
{
	short idx, list_idx=0;
	for (idx=0; idx<MAX_NSEGMENTS; idx++) {
		if (IS_SEGTBL_ITEM_USED(mm_segtable,idx)) {
			// TODO: out_slist size check
			strcpy (out_slist[list_idx].name, mm_segtable[idx].name);
			out_slist[list_idx].start = mm_segtable[idx].start;
			out_slist[list_idx].end = mm_segtable[idx].end;
			//TRACE("found segment %s with idx %d\n",mm_segtable[idx].name,idx);
			list_idx++;
		}
	}

	return list_idx;
}

bool create_segment (const char* name, void* start, void* end)
{
	//LOG_DEBUG("in create_segment %s\n",name);
	//TODO: check input args
	short idx, free_idx=IDX_INVALID;
	for (idx=0; idx<MAX_NSEGMENTS; idx++) {
		if (!IS_SEGTBL_ITEM_USED(mm_segtable,idx))
			free_idx=idx;
		else if (is_overlap(start,end,mm_segtable[idx])) {
			TRACE("segments could not overlap!\n");
			return false;
		}
	}

	if (free_idx==IDX_INVALID) {
		TRACE("max segments count reached\n");
		return false;
	}

	//LOG_DEBUG("creating segment %s with idx %d\n",name,free_idx);
	strcpy(mm_segtable[free_idx].name,name);
	mm_segtable[free_idx].start = start;
	mm_segtable[free_idx].end = end;
	return true;
}


#ifndef MM_H_
#define MM_H_

//#include "file_interface.h"
#include "common.h"

#define MAX_SEGNAME_LEN 80

typedef struct  {
	char name[MAX_SEGNAME_LEN];
	void* start;
	void* end;
} SEGMENT;

// TODO: linked list of segments will be much more convenient
#define MAX_NSEGMENTS 100

#define IS_SEGTBL_ITEM_USED(segtable,idx) (segtable[idx].name[0]!='0') ? TRUE : FALSE
#define RELEASE_SEGTBL_ITEM(segtable,idx) segtable[idx].name[0]='0'

inline BOOL is_overlap (void* start, void* end, SEGMENT seg);

size_t list_segments (SEGMENT* out_slist, int max_nitems);

BOOL create_segment(const char* name, void* start, void* end);


#endif // MM_H_

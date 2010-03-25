/**
 * @file
 *
 * @date 28.08.2009
 * @author Roman Evstifeev
 */
#ifndef MM_H_
#define MM_H_

//#include <file_interface.h>

#define MAX_SEGNAME_LEN 80

typedef struct  {
	char name[MAX_SEGNAME_LEN];
	void* start;
	void* end;
} SEGMENT;

// TODO: linked list of segments will be much more convenient
#define MAX_NSEGMENTS 100

#define IS_SEGTBL_ITEM_USED(segtable,idx) (segtable[idx].name[0]!='0') ? true : false
#define RELEASE_SEGTBL_ITEM(segtable,idx) segtable[idx].name[0]='0'

inline bool is_overlap (void* start, void* end, SEGMENT seg);

size_t list_segments (SEGMENT* out_slist, int max_nitems);

bool create_segment(const char* name, void* start, void* end);

#endif // MM_H_

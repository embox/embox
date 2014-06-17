/*
 * malloc.h
 *
 *  Created on: 2013-okt-14
 *      Author: fsulima
 */

#ifndef MALLOC_H_
#define MALLOC_H_

struct mallinfo {
   int arena;     /* Non-mmapped space allocated (bytes) */
   int ordblks;   /* Number of free chunks */
   int smblks;    /* Number of free fastbin blocks */
   int hblks;     /* Number of mmapped regions */
   int hblkhd;    /* Space allocated in mmapped regions (bytes) */
   int usmblks;   /* Maximum total allocated space (bytes) */
   int fsmblks;   /* Space in freed fastbin blocks (bytes) */
   int uordblks;  /* Total allocated space (bytes) */
   int fordblks;  /* Total free space (bytes) */
   int keepcost;  /* Top-most, releasable space (bytes) */
};

static inline
struct mallinfo mallinfo(void) {
	struct mallinfo mi;
	return mi;
}

#endif /* MALLOC_H_ */

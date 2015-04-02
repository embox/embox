/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#ifndef MEM_MMAP_H_
#define MEM_MMAP_H_

#include <module/embox/mem/mmap_api.h>

//TODO:
#define MAREA_ALIGMENT            0x1000
#define MAREA_ALIGMENT_MASK       (MAREA_ALIGMENT - 1)

#define MAREA_ALIGN_DOWN(x)       ((x) & (~MAREA_ALIGMENT_MASK))
#define MAREA_ALIGN_UP(x)         (MAREA_ALIGN_DOWN((x) + MAREA_ALIGMENT_MASK))

extern void mmap_init(struct emmap *mmap);

extern void mmap_clear(struct emmap *mmap);

extern void mmap_free(struct emmap *mmap);

extern int mmap_inherit(struct emmap *mmap, struct emmap *parent_mmap);

extern struct marea *mmap_place_marea(struct emmap *mmap, uint32_t start, uint32_t end, uint32_t flags);

extern struct marea *mmap_alloc_marea(struct emmap *mmap, size_t size, uint32_t flags);

static inline uint32_t marea_get_start(struct marea *marea) {
	return marea->start;
}

static inline void *mmap_get_brk(struct emmap *mmap) {
	return mmap->brk;
}

static inline void mmap_set_brk(struct emmap *mmap, void *new_brk) {
	mmap->brk = new_brk;
}

/* XXX shouldn't be public */
extern int mmap_do_marea_map(struct emmap *mmap, struct marea *marea);
extern void mmap_do_marea_unmap(struct emmap *mmap, struct marea *marea);

#endif /* MEM_MMAP_H_ */

/*
 * @file
 *
 * @date Apr 12, 2013
 * @author: Anton Bondarev
 */

#ifndef NO_PAGE_H_
#define NO_PAGE_H_

#define PAGE_SIZE() OPTION_MODULE_GET(embox__mem__NoPage,NUMBER,page_size)

#ifndef __LDS__

static inline struct page_allocator *page_allocator_init(char *start, size_t len, size_t page_size) {
	return NULL;
}

static inline int page_belong(struct page_allocator *allocator, void *page) {
	return 0;
}

static inline void *page_alloc(struct page_allocator *allocator, size_t page_q) {
	return NULL;
}

static inline void *page_alloc_zero(struct page_allocator *allocator, size_t page_q) {
	return NULL;
}

static inline void page_free(struct page_allocator *allocator, void *page, size_t page_q) {

}

#endif /* __LDS__ */

#endif /* NO_PAGE_H_ */

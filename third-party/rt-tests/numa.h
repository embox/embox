#ifndef _NUMA_H
#define _NUMA_H

#include <stddef.h>
#include <sys/types.h>

struct bitmask {
	unsigned long size; /* number of bits in the map */
	unsigned long *maskp;
};

static inline int numa_available(void) {
	return -1;
}
static inline unsigned int numa_bitmask_weight(const struct bitmask *bm) {
	return 0;
}
static inline int numa_bitmask_isbitset(const struct bitmask *bm, unsigned int n) {
	return 0;
}
static inline struct bitmask *numa_allocate_cpumask(void) {
	return NULL;
}
static inline struct bitmask *numa_bitmask_clearbit(struct bitmask *bm,
    unsigned int n) {
	return NULL;
}
static inline void numa_bitmask_free(struct bitmask *bm) {
}
static inline struct bitmask *numa_parse_cpustring_all(const char *s) {
	return NULL;
}
static inline void *numa_alloc_onnode(size_t size, int node) {
	return NULL;
}
static inline void numa_free(void *mem, size_t size) {
}
static inline int numa_run_on_node(int node) {
	return 0;
}

static inline int numa_node_of_cpu(int cpu) {
	return 0;
}

static inline int numa_sched_getaffinity(pid_t pid, struct bitmask *bm) {
	return 0;
}
static inline int numa_sched_setaffinity(pid_t pid, struct bitmask *bm) {
	return 0;
}

#endif
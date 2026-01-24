#ifndef _NUMA_H
#define _NUMA_H 

struct bitmask {
	unsigned long size; /* number of bits in the map */
	unsigned long *maskp;
};

int numa_available(void);
unsigned int numa_bitmask_weight(const struct bitmask *);
int numa_bitmask_isbitset(const struct bitmask *, unsigned int);
struct bitmask *numa_allocate_cpumask(void);
int numa_sched_getaffinity(pid_t, struct bitmask *);
struct bitmask *numa_bitmask_clearbit(struct bitmask *, unsigned int);
void numa_bitmask_free(struct bitmask *);
struct bitmask *numa_parse_cpustring_all(const char *);


#endif
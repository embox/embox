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
struct bitmask *numa_bitmask_clearbit(struct bitmask *, unsigned int);
void numa_bitmask_free(struct bitmask *);
struct bitmask *numa_parse_cpustring_all(const char *);

void *numa_alloc_onnode(size_t size, int node);

void numa_free(void *mem, size_t size);

int numa_run_on_node(int node);

int numa_node_of_cpu(int cpu);

int numa_sched_getaffinity(pid_t, struct bitmask *);
int numa_sched_setaffinity(pid_t, struct bitmask *);


#endif
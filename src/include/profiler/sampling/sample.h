#ifndef PROFILER_SAMPLING_SAMPLE_H_
#define PROFILER_SAMPLING_SAMPLE_H_

#define SAMPLE_TIMER_INTERVAL 500
#define SAMPLE_HASH_SIZE 997

struct function_sample_info {
	const char *name;
	int counter;
};

extern int start_profiler(int interval);
extern int stop_profiler(void);

extern int *get_counters(void);
extern char *get_name(int ind);
extern bool sampling_profiler_is_running(void);

#endif /* PROFILER_SAMPLING_SAMPLE_H_ */

#ifndef PROFILER_SAMPLING_SAMPLE_H_
#define PROFILER_SAMPLING_SAMPLE_H_

#define TIMER_INTERVAL 500
#define SAMPLE_HASH_SIZE 997

struct function_sample_info {
	const char *name;
	int counter;
};

int start_profiler(void);
int stop_profiler(void);

int *get_counters(void);
char *get_name(int ind);
bool sampling_profiler_is_running(void);

#endif /* PROFILER_SAMPLING_SAMPLE_H_ */

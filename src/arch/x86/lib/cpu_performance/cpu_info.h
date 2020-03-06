#ifndef X86_CPU_INFO_H_
#define X86_CPU_INFO_H_

struct cpu_info {
	char vendor_id[13];
	unsigned int base_freq;
	unsigned int max_freq;
	unsigned int FPU;
	unsigned int SSE;
	unsigned int MMX;
};

extern uint64_t get_cpu_counter(void);
extern struct cpu_info *get_cpu_info(void);

#endif /* X86_CPU_INFO_H_ */

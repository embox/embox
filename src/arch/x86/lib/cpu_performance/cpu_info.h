#ifndef X86_CPU_INFO_H_
#define X86_CPU_INFO_H_

struct cpu_info {
	char vendor_id[13];
	unsigned int freq;
};

extern uint64_t get_cpu_counter(void);
extern struct cpu_info *get_cpu_info(void);

#endif /* X86_CPU_INFO_H_ */

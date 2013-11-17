#ifndef HAL_CPU_INFO_H_
#define HAL_CPU_INFO_H_

#include <stdint.h>

#include <module/embox/arch/cpu_info.h>

struct cpu_info;

extern struct cpu_info *get_cpu_info(void);
extern uint64_t get_cpu_counter(void);

#endif /* HAL_CPU_INFO_H_ */

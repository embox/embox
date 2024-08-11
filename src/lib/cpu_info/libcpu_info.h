/**
 * @file
 * @brief CPU Information Library
 *
 * @date 19.03.2020
 * @author Archit Checker
 */

#ifndef LIB_CPU_INFO_H_
#define LIB_CPU_INFO_H_

#define FEATURE_NAME_LEN 32
#define MAX_NUM_FEATURES 10

#include <stdint.h>

#include <module/embox/arch/cpu_info.h>

struct cpu_feature {
	char name[FEATURE_NAME_LEN];
	char strval[FEATURE_NAME_LEN];
	unsigned int val; 
};

struct cpu_info {
	char vendor_id[FEATURE_NAME_LEN];
	struct cpu_feature feature[MAX_NUM_FEATURES];
	unsigned int feature_count;
};

extern void set_feature_val(struct cpu_info *info, const char *name, unsigned int val);
extern void set_feature_strval(struct cpu_info *info, const char *name, const char *strval);

extern struct cpu_info *get_cpu_info(void);
extern uint64_t get_cpu_counter(void);

extern void cpu_feature_print(struct cpu_feature *cpu_feature);

#endif /* LIB_CPU_INFO_H_ */

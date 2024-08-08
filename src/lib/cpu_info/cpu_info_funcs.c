/**
 * @file
 * @brief
 *
 * @date 19.03.2020
 * @author Archit Checker
 */

#include <lib/libcpu_info.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

void set_feature_val(struct cpu_info *info, const char *name,
    unsigned int val) {
	int cur = info->feature_count;

	assert(cur < MAX_NUM_FEATURES);

	memset(&info->feature[cur], 0, sizeof(info->feature[cur]));

	strcpy(info->feature[cur].name, name);
	info->feature[cur].val = val;

	info->feature_count += 1;
}

void set_feature_strval(struct cpu_info *info, const char *name,
    const char *strval) {
	int cur = info->feature_count;

	assert(cur < MAX_NUM_FEATURES);

	memset(&info->feature[cur], 0, sizeof(info->feature[cur]));

	strcpy(info->feature[cur].name, name);
	strcpy(info->feature[cur].strval, strval);

	info->feature_count += 1;
}

void cpu_feature_print(struct cpu_feature *cpu_feature) {
	if (strlen(cpu_feature->strval)) {
		printf("%-24s %s", cpu_feature->name, cpu_feature->strval);
	}
	else {
		printf("%-24s %u", cpu_feature->name, cpu_feature->val);
	}
}

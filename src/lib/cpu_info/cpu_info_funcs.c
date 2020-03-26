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

void set_feature_val(struct cpu_info *info, const char *name, unsigned int val) {
	int current_feature = info->feature_count;
	assert((current_feature < MAX_NUM_FEATURES) && "Exceeded Number of Features!");
	strcpy(info->feature[current_feature].name, name);
	info->feature[current_feature].val = val;
	info->feature_count += 1;
}

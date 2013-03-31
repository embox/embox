/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <string.h>
#include <kernel/task.h>
#include "security/smac.h"

static void smac_init(struct task *task, void* security) {
	struct smac_task *smac_task = (struct smac_task *) security;
	memset(smac_task, 0, sizeof(struct smac_task));
	task->security = smac_task;
}

static int smac_inherit(struct task *task, struct task *parent) {
	memcpy(task->security, parent->security, sizeof(struct smac_task));
	return 0;
}

static void smac_deinit(struct task *task) {
}

static const struct task_resource_desc smac_res = {
	.init = smac_init,
	.inherit = smac_inherit,
	.deinit = smac_deinit,
	.resource_size = sizeof(struct smac_task),
};

TASK_RESOURCE_DESC(&smac_res);

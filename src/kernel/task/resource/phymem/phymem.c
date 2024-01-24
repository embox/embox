/**
 * @file
 *
 * @date Jun 11, 2014
 * @author: Anton Bondarev
 */
#include <stdlib.h>

#include <mem/phymem.h>
#include <lib/libds/dlist.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/task_phymem.h>

struct phymem_link {
	struct dlist_head link;
	void *paddr;
	int pages;
};

TASK_RESOURCE_DEF(task_phymem_desc, struct dlist_head);

static size_t task_phymem_offset;

static void task_phymem_init(const struct task *task, void *phymem_space) {
	dlist_init(phymem_space);
}

static void task_phymem_deinit(const struct task *task) {
	struct dlist_head *head;
	void *ptr;
	struct phymem_link *pl = NULL;

	head = (void *)task->resources + task_phymem_offset;

	dlist_foreach_entry(pl, head, link) {
		phymem_free(pl->paddr, pl->pages);
	}

	while (head->next != head) {
		ptr = head->next;
		dlist_del(ptr);
		free(ptr);
	}
}

static const struct task_resource_desc task_phymem_desc = {
	.init = task_phymem_init,
	.deinit = task_phymem_deinit,
	.resource_size = sizeof(struct dlist_head),
	.resource_offset = &task_phymem_offset
};

int task_resource_phymem_add(const struct task *task, void *paddr, int pages) {
	struct dlist_head *head;
	struct phymem_link *pl;

	assert(task != NULL);

	head = (void *)task->resources + task_phymem_offset;
	pl = malloc(sizeof(*pl));
	pl->paddr = paddr;
	pl->pages = pages;

	dlist_head_init(&pl->link);
	dlist_add_next(&pl->link, head);

	return 0;
}

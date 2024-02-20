/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.02.24
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include <drivers/char_dev.h>
#include <embox/unit.h>
#include <lib/libds/array.h>
#include <lib/libds/dlist.h>

EMBOX_UNIT_INIT(char_dev_registry_init);

static DLIST_DEFINE(char_dev_list);

struct dlist_head *const __char_dev_registry = &char_dev_list;

static int char_dev_check_name(const char *name) {
	struct char_dev *tmp;
	size_t len;

	len = strlen(name);

	if (len == 0) {
		return -EINVAL;
	}

	if (len >= sizeof(((struct char_dev *)0)->name)) {
		return -ENAMETOOLONG;
	}

	char_dev_foreach(tmp) {
		if (!strcmp(tmp->name, name)) {
			return -EEXIST;
		}
	}

	return 0;
}

struct char_dev *char_dev_iterate(struct char_dev *cdev) {
	if (!cdev) {
		return dlist_first_entry_or_null(&char_dev_list, struct char_dev,
		    list_item);
	}

	return dlist_next_entry_if_not_last(cdev, &char_dev_list, struct char_dev,
	    list_item);
}

struct char_dev *char_dev_find(const char *name) {
	struct char_dev *cdev;

	assert(name);

	char_dev_foreach(cdev) {
		if (!strcmp(name, cdev->name)) {
			return cdev;
		}
	}

	return NULL;
}

int char_dev_register(struct char_dev *cdev) {
	int err;

	assert(cdev);
	assert(cdev->ops);

	if ((err = char_dev_check_name(cdev->name))) {
		return err;
	}

	dlist_add_next(&cdev->list_item, __char_dev_registry);

	return 0;
}

int char_dev_unregister(struct char_dev *cdev) {
	struct char_dev *tmp;

	assert(cdev);

	char_dev_foreach(tmp) {
		if (tmp == cdev) {
			dlist_del_init(&cdev->list_item);
			return 0;
		}
	}

	return -EINVAL;
}

int char_dev_rename(struct char_dev *cdev, const char *new_name) {
	int err;

	assert(cdev);
	assert(new_name);

	if ((err = char_dev_check_name(cdev->name))) {
		return err;
	}

	strcpy(cdev->name, new_name);

	return 0;
}

void char_dev_init(struct char_dev *cdev, const char *name,
    const struct char_dev_ops *ops) {
	assert(cdev);
	assert(name);
	assert(ops);

	dlist_init(&cdev->list_item);
	cdev->usage_count = 0;
	cdev->ops = ops;

	strncpy(cdev->name, name, sizeof(cdev->name) - 1);
	cdev->name[sizeof(cdev->name) - 1] = '\0';
}

ARRAY_SPREAD_DEF(struct char_dev *const, __char_dev_static_registry);

static int char_dev_registry_init(void) {
	struct char_dev *cdev;
	int err;

	array_spread_foreach(cdev, __char_dev_static_registry) {
		if ((err = char_dev_register(cdev))) {
			return err;
		}
	}

	return 0;
}

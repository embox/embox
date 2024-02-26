/**
 * @file fpga.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 27.01.2020
 */

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/fpga.h>
#include <framework/mod/options.h>
#include <kernel/task/resource/idesc.h>
#include <lib/libds/indexator.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#define FPGA_MAX OPTION_GET(NUMBER, fpga_pool_sz)

static struct fpga fpga_tab[FPGA_MAX];
INDEX_DEF(fpga_idx, 0, FPGA_MAX);

static const struct char_dev_ops fpga_dev_ops;

static int fpga_dev_open(struct char_dev *cdev, struct idesc *idesc) {
	struct fpga *fpga;
	int err;

	fpga = (struct fpga *)cdev;

	err = fpga->ops->config_init(fpga);
	if (err) {
		log_error("Failed to init config for FPGA");
		return -1;
	}

	return 0;
}

static void fpga_dev_close(struct char_dev *cdev) {
	struct fpga *fpga;
	int err;

	fpga = (struct fpga *)cdev;

	err = fpga->ops->config_complete(fpga);
	if (err) {
		log_error(".conf_complete() finished with error code %d", err);
	}
}

static ssize_t fpga_dev_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	/* Dumping FPGA config is currently NIY */
	return 0;
}

static ssize_t fpga_dev_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	struct fpga *fpga;
	int err;

	fpga = (struct fpga *)cdev;

	err = fpga->ops->config_write(fpga, buf, nbyte);
	if (err) {
		log_error("Failed to write FPGA config");
		return -1;
	}

	return nbyte;
}

struct fpga *fpga_register(struct fpga_ops *ops, void *priv) {
	size_t id;
	int err;
	char name[NAME_MAX];

	id = index_alloc(&fpga_idx, INDEX_MIN);
	snprintf(name, sizeof(name), "fpga%d", id);

	fpga_tab[id] = (struct fpga){
	    .id = id,
	    .ops = ops,
	    .priv = priv,
	};

	char_dev_init(&fpga_tab[id].cdev, name, &fpga_dev_ops);

	err = char_dev_register(&fpga_tab[id].cdev);
	if (err) {
		index_free(&fpga_idx, id);
		return NULL;
	}

	return &fpga_tab[id];
}

int fpga_unregister(struct fpga *fpga) {
	if (fpga == NULL) {
		return -EINVAL;
	}

	index_free(&fpga_idx, fpga->id);

	char_dev_unregister(&fpga->cdev);

	return 0;
}

struct fpga *fpga_by_id(size_t id) {
	if (id >= FPGA_MAX) {
		return NULL;
	}

	if (!index_locked(&fpga_idx, id)) {
		return NULL;
	}

	return &fpga_tab[id];
}

size_t fpga_max_id(void) {
	return FPGA_MAX;
}

static const struct char_dev_ops fpga_dev_ops = {
    .open = fpga_dev_open,
    .close = fpga_dev_close,
    .read = fpga_dev_read,
    .write = fpga_dev_write,
};

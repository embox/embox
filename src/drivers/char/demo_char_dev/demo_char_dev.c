/**
 * @file
 *
 * @date Mar 28, 2022
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <sys/types.h>

#include <drivers/char_dev.h>
#include <drivers/device.h>
#include <embox/unit.h>
#include <lib/libds/indexator.h>
#include <mem/misc/pool.h>
#include <util/log.h>

EMBOX_UNIT_INIT(demo_char_dev_module_init);

#define DEMO_CHAR_DEV_MAX  2

#define DEMO_CHAR_DEV_NAME "demo_char_dev"

struct demo_char_dev_priv {
	int idx;
	char name[16];
};

INDEX_DEF(demo_char_dev_idx, 0, DEMO_CHAR_DEV_MAX);

POOL_DEF(priv_pool, struct demo_char_dev_priv, DEMO_CHAR_DEV_MAX);

static int demo_char_dev_module_init(void) {
	int i;

	for (i = 0; i < DEMO_CHAR_DEV_MAX; i++) {
		demo_char_dev_create();
	}
	return 0;
}

static ssize_t demo_char_dev_idesc_read(struct idesc *desc,
    const struct iovec *iov, int cnt) {
	return 0;
}

static ssize_t demo_char_dev_idesc_write(struct idesc *desc,
    const struct iovec *iov, int cnt) {
	return 0;
}

static int demo_char_dev_idesc_ioctl(struct idesc *idesc, int cmd, void *args) {
	return 0;
}

static const struct idesc_ops demo_char_dev_iops;

int demo_char_dev_register(struct demo_char_dev_priv *priv) {
	struct dev_module *dev;

	dev = dev_module_create(priv->name, &demo_char_dev_iops, priv);
	if (!dev) {
		log_error("failed to allocate new char device \"%s\"", priv->name);
		return -1;
	}

	if (0 != char_dev_register(dev)) {
		log_error("failed to register char device for \"%s\"", priv->name);
		dev_module_destroy(dev);
		return -1;
	}

	return 0;
}

int demo_char_dev_create(void) {
	struct demo_char_dev_priv *priv;
	int id;

	id = index_alloc(&demo_char_dev_idx, INDEX_MIN);
	if (id == INDEX_NONE) {
		log_error("failed to allocate idx for demo char dev");
		return -1;
	}

	priv = pool_alloc(&priv_pool);
	if (!priv) {
		log_error("failed to allocate new priv for demo char dev");
		return -1;
	}
	memset(priv, 0, sizeof(*priv));

	priv->idx = id;
	snprintf(priv->name, sizeof(priv->name), "%s%d", DEMO_CHAR_DEV_NAME, id);

	demo_char_dev_register(priv);

	return 0;
}

int demo_char_dev_destroy(struct dev_module *dev) {
	if (dev->dev_priv) {
		struct demo_char_dev_priv *priv;
		priv = dev->dev_priv;

		index_free(&demo_char_dev_idx, priv->idx);

		pool_free(&priv_pool, priv);
	}

	if (0 != char_dev_unregister(dev)) {
		log_error("failed to unregister char device for \"%s\"", dev->name);
		return -1;
	}

	dev_module_destroy(dev);
	return 0;
}

static const struct idesc_ops demo_char_dev_iops = {
    .open = char_dev_default_open,
    .close = char_dev_default_close,
    .fstat = char_dev_default_fstat,
    .id_readv = demo_char_dev_idesc_read,
    .id_writev = demo_char_dev_idesc_write,
    .ioctl = demo_char_dev_idesc_ioctl,
};

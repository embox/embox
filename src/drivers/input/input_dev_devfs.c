/**
 * @file
 *
 * @date 30.11.20
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <string.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/device.h>
#include <drivers/input/input_dev.h>
#include <kernel/task/resource/idesc_event.h>
#include <kernel/thread/thread_sched_wait.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#define INPUT_DEV_CNT OPTION_GET(NUMBER, input_dev_cnt)

POOL_DEF(cdev_input_pool, struct dev_module, INPUT_DEV_CNT);

static void input_dev_fs_close(struct idesc *desc) {
	struct input_dev *inpdev = idesc_to_dev_module(desc)->dev_priv;

	input_dev_close(inpdev);

	inpdev->fs_data = NULL;
}

static int input_dev_fs_wait(struct idesc *desc, int flags) {
	struct idesc_wait_link wl;

	return IDESC_WAIT_LOCKED(/* no unlock */, desc, &wl, flags,
	    SCHED_TIMEOUT_INFINITE,
	    /* no lock */);
}

static ssize_t input_dev_fs_read(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	struct input_dev *inpdev = idesc_to_dev_module(desc)->dev_priv;
	ssize_t sz, ret_size = 0;
	struct input_event *ev;
	int i, res = 0;

	do {
		for (i = 0; i < cnt; i++) {
			sz = iov[i].iov_len;
			ev = (struct input_event *)iov[i].iov_base;

			while ((sz >= sizeof *ev) && !input_dev_event(inpdev, ev)) {
				sz -= sizeof *ev;
				ev++;
			}

			ret_size += iov[i].iov_len - sz;
		}

		if (!ret_size) {
			res = input_dev_fs_wait(desc, POLLIN);
		}
	} while (ret_size == 0 && res == 0);

	return ret_size;
}

static int input_dev_fs_status(struct idesc *desc, int mask) {
	struct input_dev *inpdev = idesc_to_dev_module(desc)->dev_priv;
	int res;

	switch (mask) {
	case POLLIN:
		res = input_dev_bytes_count(inpdev);
		break;
	default:
	case POLLOUT:
	case POLLERR:
		res = 0;
		break;
	}

	return res;
}

static struct idesc *input_dev_fs_open(struct dev_module *mod, void *dev_priv) {
	struct input_dev *inpdev = mod->dev_priv;
	struct idesc *idesc;

	idesc = char_dev_idesc_create(mod);
	inpdev->fs_data = idesc;

	input_dev_open(inpdev, NULL);

	return idesc;
}

static const struct dev_module_ops input_dev_fs_ops;
static const struct idesc_ops input_dev_fs_iops;

int input_dev_private_register(struct input_dev *inpdev) {
	struct dev_module *dev;

	dev = pool_alloc(&cdev_input_pool);
	if (!dev) {
		log_error("failed to allocate new input device \"%s\"", inpdev->name);
		return -1;
	}

	memset(dev, 0, sizeof(*dev));

	strncat(dev->name, inpdev->name, sizeof(dev->name) - 1);

	dev->dev_ops = &input_dev_fs_ops;
	dev->dev_iops = &input_dev_fs_iops;
	dev->dev_priv = inpdev;

	if (0 != char_dev_register(dev)) {
		log_error("failed to register char device for \"%s\"", inpdev->name);
		pool_free(&cdev_input_pool, dev);
		return -1;
	}

	return 0;
}

int input_dev_private_notify(struct input_dev *inpdev, struct input_event *ev) {
	if (inpdev->fs_data) {
		idesc_notify((struct idesc *)inpdev->fs_data, POLLIN);
	}
	else if (inpdev->event_cb) {
		inpdev->event_cb(inpdev);
	}
	else {
		log_error("input device has not been opend \"%s\"", inpdev->name);
	}

	return 0;
}

static const struct dev_module_ops input_dev_fs_ops = {
    .dev_open = input_dev_fs_open,
};

static const struct idesc_ops input_dev_fs_iops = {
    .close = input_dev_fs_close,
    .id_readv = input_dev_fs_read,
    .status = input_dev_fs_status,
    .fstat = char_dev_idesc_fstat,
};

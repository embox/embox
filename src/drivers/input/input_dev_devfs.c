/**
 * @file
 *
 * @date 30.11.20
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/input/input_dev.h>
#include <kernel/task/resource/idesc_event.h>
#include <kernel/thread/thread_sched_wait.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#define INPUT_DEV_CNT OPTION_GET(NUMBER, input_dev_cnt)

struct cdev_input {
	struct char_dev cdev;
	struct input_dev *inpdev;
};

POOL_DEF(cdev_input_pool, struct cdev_input, INPUT_DEV_CNT);

static int input_dev_fs_wait(struct idesc *desc, int flags) {
	struct idesc_wait_link wl;

	return IDESC_WAIT_LOCKED(/* no unlock */, desc, &wl, flags,
	    SCHED_TIMEOUT_INFINITE,
	    /* no lock */);
}

static ssize_t input_dev_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	struct input_dev *inpdev;
	ssize_t sz;
	ssize_t ret_size = 0;
	struct input_event *ev;
	int i, res = 0;

	inpdev = ((struct cdev_input *)cdev)->inpdev;

	assert(inpdev);

	do {
		sz = nbyte;
		ev = (struct input_event *)buf;

		while ((sz >= sizeof *ev) && !input_dev_event(inpdev, ev)) {
			sz -= sizeof *ev;
			ev++;
		}

		ret_size += nbyte - sz;

		if (!ret_size) {
			res = input_dev_fs_wait(desc, POLLIN);
		}
	} while (ret_size == 0 && res == 0);

	return ret_size;
}

static int input_dev_status(struct char_dev *cdev, int mask) {
	struct input_dev *inpdev;
	int res;

	inpdev = ((struct cdev_input *)cdev)->inpdev;

	assert(inpdev);

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

static int input_dev_open(struct char_dev *cdev, struct idesc *idesc) {
	struct input_dev *inpdev;

	inpdev = ((struct cdev_input *)cdev)->inpdev;

	assert(inpdev);

	input_dev_open(inpdev, NULL);

	return 0;
}

static void input_dev_close(struct char_dev *cdev) {
	struct input_dev *inpdev;

	inpdev = ((struct cdev_input *)cdev)->inpdev;

	assert(inpdev);

	input_dev_close(inpdev);
}

static const struct char_dev_ops input_cdev_ops = {
    .read = input_dev_read,
    .status = input_dev_status,
    .open = input_dev_open,
    .close = input_dev_close,
};

int input_dev_private_register(struct input_dev *inpdev) {
	struct cdev_input *dev;
	int err;

	dev = pool_alloc(&cdev_input_pool);
	if (!dev) {
		log_error("failed to allocate new input device \"%s\"", inpdev->name);
		return -ENOMEM;
	}

	char_dev_init(&dev->cdev, inpdev->name, &input_cdev_ops);
	dev->inpdev = inpdev;

	if ((err = char_dev_register(dev))) {
		log_error("failed to register char device for \"%s\"", inpdev->name);
		pool_free(&cdev_input_pool, dev);
	}

	return err;
}

int input_dev_private_notify(struct input_dev *inpdev, struct input_event *ev) {
	if (inpdev->event_cb) {
		inpdev->event_cb(inpdev);
	}

	return 0;
}

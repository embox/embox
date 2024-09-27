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
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_event.h>
#include <kernel/thread/thread_sched_wait.h>
#include <util/log.h>

static int input_dev_fs_wait(struct idesc *desc, int flags) {
	struct idesc_wait_link wl;

	return IDESC_WAIT_LOCKED(/* no unlock */, desc, &wl, flags,
	    SCHED_TIMEOUT_INFINITE,
	    /* no lock */);
}

static ssize_t input_cdev_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	struct input_dev *inpdev;
	struct input_event *ev;
	ssize_t sz;
	ssize_t ret_size = 0;
	int res = 0;

	inpdev = (struct input_dev *)cdev;

	do {
		sz = nbyte;
		ev = (struct input_event *)buf;

		while ((sz >= sizeof *ev) && !input_dev_event(inpdev, ev)) {
			sz -= sizeof *ev;
			ev++;
		}

		ret_size += nbyte - sz;

		if (!ret_size) {
			res = input_dev_fs_wait(inpdev->idesc, POLLIN);
		}
	} while (ret_size == 0 && res == 0);

	return ret_size;
}

static int input_cdev_status(struct char_dev *cdev, int mask) {
	struct input_dev *inpdev;
	int res;

	inpdev = (struct input_dev *)cdev;

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

static int input_cdev_open(struct char_dev *cdev, struct idesc *idesc) {
	struct input_dev *inpdev;

	inpdev = (struct input_dev *)cdev;

	if (inpdev->idesc) {
		return -EBUSY;
	}

	inpdev->idesc = idesc;

	input_dev_open(inpdev, NULL);

	return 0;
}

static void input_cdev_close(struct char_dev *cdev) {
	struct input_dev *inpdev;

	inpdev = (struct input_dev *)cdev;

	input_dev_close(inpdev);

	inpdev->idesc = NULL;
}

static const struct char_dev_ops input_cdev_ops = {
    .read = input_cdev_read,
    .status = input_cdev_status,
    .open = input_cdev_open,
    .close = input_cdev_close,
};

int input_dev_private_register(struct input_dev *inpdev) {
	int err;

	char_dev_init(&inpdev->cdev, inpdev->name, &input_cdev_ops);

	if ((err = char_dev_register(&inpdev->cdev))) {
		log_error("failed to register char device for \"%s\"", inpdev->name);
	}

	return err;
}

int input_dev_private_notify(struct input_dev *inpdev, struct input_event *ev) {
	if (inpdev->idesc) {
		idesc_notify(inpdev->idesc, POLLIN);
	}
	else if (inpdev->event_cb) {
		inpdev->event_cb(inpdev);
	}
	else {
		log_error("input device \"%s\" has not been opened", inpdev->name);
	}

	return 0;
}

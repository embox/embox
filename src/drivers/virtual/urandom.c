/**
 * @file
 * @brief Creates file /dev/urandom
 *
 * @date 24.05.24
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <drivers/char_dev.h>

static ssize_t urandom_read(struct char_dev *cdev, void *buf, size_t nbyte) {	
	static uint32_t rand_seed = 0xdeadbeaf;
	int i;

	for(i = 0; i < nbyte; i += 4) {
		rand_seed += clock();
		rand_seed >>= 4;
		rand_seed *= 0x837478;
		memcpy(buf + i, &rand_seed, nbyte - i);
	}

	return nbyte;
}

static ssize_t urandom_write(struct char_dev *cdev, const void *buf, size_t nbyte) {
	return nbyte;
}

static const struct char_dev_ops urandom_cdev_ops = {
    .read = urandom_read,
    .write = urandom_write,
};

static struct char_dev urandom_cdev =
		CHAR_DEV_INIT(urandom_cdev, "urandom", &urandom_cdev_ops);

CHAR_DEV_REGISTER(&urandom_cdev);

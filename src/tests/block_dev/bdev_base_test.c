/**
 * @file
 *
 * @date 03 sep 2015
 * @author: Anton Bondarev
 */
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <drivers/block_dev.h>

#include <embox/test.h>

#define BDEV_NAME    OPTION_STRING_GET(bdev_name)
#define BLOCK_NUMBER OPTION_GET(NUMBER,block_number)

EMBOX_TEST_SUITE("bdev base tests");

TEST_CASE("Fill one block with constant value") {
	int bdev;
	int res;
	int block_size;
	char wbuffer[0x1000];
	char rbuffer[sizeof(wbuffer)];

	bdev = open(BDEV_NAME, O_RDWR);
	test_assert(bdev >0);

	block_size = 512;
#if 0
	res = ioctl(bdev, IOCTL_GETBLKSIZE, &block_size);
	test_assert(res);
#endif
	test_assert(block_size < sizeof(wbuffer));

	memset(wbuffer, 0x55, block_size);

	res = write(bdev, wbuffer, block_size);
	test_assert(res);

	res = read(bdev, rbuffer, block_size);
	test_assert(res);

	res = memcmp(wbuffer, rbuffer, block_size);
	test_assert(res == 0);

	close(bdev);
}

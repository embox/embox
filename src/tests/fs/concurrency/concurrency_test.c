/**
 * @file
 * @brief Test for concurrent reading on a single file
 *
 * @date 19.12.2018
 * @author Alexander Kalmuk
 */

#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("File system concurrency test suite");

/* File's content is "abcdef" */
#define TEST_FILE_NAME "/concurrency_test_file.txt"

TEST_CASE("Reading regular file from one file descriptor should not affect "
			"reading the same file from another file descriptor") {
	char buf[3];
	int fd1, fd2;

	test_assert(0 <= (fd1 = open(TEST_FILE_NAME, O_RDONLY)));
	test_assert_equal(lseek(fd1, 3, SEEK_SET), 3);

	test_assert(0 <= (fd2 = open(TEST_FILE_NAME, O_RDONLY)));

	read(fd2, buf, 3);
	test_assert_mem_equal("abc", buf, 3);

	close(fd1);
	close(fd2);
}

static void *test_fs_thr_hnd(void *arg) {
	int i;
	int fd;
	char buf[6];
	int *res = (int *) arg;

	test_assert(0 <= (fd = open(TEST_FILE_NAME, O_RDONLY)));

	for (i = 0; i < 100000; i++) {
		lseek(fd, 0, SEEK_SET);
		if (read(fd, buf, 6) < 0) {
			close(fd);
			*res = -1;
			return NULL;
		}
	}

	close(fd);
	*res = 0;

	return NULL;
}

TEST_CASE("Regular file should be able to read from different threads") {
	pthread_t threads[2];
	int res[2];
	int status;

	status = pthread_create(&threads[0], NULL, test_fs_thr_hnd, &res[0]);
	if (status != 0) {
		test_assert(0);
	}

	status = pthread_create(&threads[1], NULL, test_fs_thr_hnd, &res[1]);
	if (status != 0) {
		test_assert(0);
	}

	pthread_join(threads[0], (void**)&status);
	pthread_join(threads[1], (void**)&status);

	test_assert(res[0] == 0 && res[1] == 0);
}

TEST_CASE("dup2 on the regular file should not affect its copy") {
	char buf[3];
	int fd1, fd2;

	test_assert(0 <= (fd1 = open(TEST_FILE_NAME, O_RDONLY)));
	test_assert_equal(lseek(fd1, 3, SEEK_SET), 3);

	fd2 = fd1 + 1;
	dup2(fd1, fd2);

	read(fd2, buf, 3);
	test_assert_mem_equal("def", buf, 3);

	close(fd1);
	close(fd2);
}

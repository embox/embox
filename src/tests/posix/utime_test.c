/**
 * @file
 * @brief test for utime
 *
 * @author  Filipp Chubukov
 * @date    21.04.2019
 */

#include <fcntl.h>
#include <utime.h>
#include <sys/stat.h>
#include <embox/test.h>
#include <errno.h>
#include <unistd.h>

#define TEST_EXIST_FILE "test"
#define TEST_NOT_EXIST_FILE "not_exist_test"
EMBOX_TEST_SUITE("posix/utime_test");

TEST_CASE("check changed time") {
	struct utimbuf prev_time;
	struct utimbuf new_time;

	struct stat filestat;
	int fd;

	new_time.actime = 999;
	new_time.modtime = 999;

	test_assert((fd = creat(TEST_EXIST_FILE, 0666)) >= 0);
	test_assert(close(fd) == 0);

	test_assert(stat(TEST_EXIST_FILE, &filestat) == 0);

	prev_time.actime = filestat.st_ctime;
	prev_time.modtime = filestat.st_mtime;

	test_assert(utime(TEST_EXIST_FILE, &new_time) == 0);

	test_assert(stat(TEST_EXIST_FILE, &filestat) == 0);
	
	test_assert(filestat.st_ctime == new_time.actime);
	test_assert(filestat.st_mtime == new_time.modtime);

	test_assert(utime (TEST_EXIST_FILE, &prev_time) == 0);

	test_assert(stat(TEST_EXIST_FILE, &filestat) == 0);

	test_assert(filestat.st_ctime == prev_time.actime);
	test_assert(filestat.st_mtime == prev_time.modtime);
}

TEST_CASE("file not exist") {
	struct utimbuf time;

	time.actime = 0;
	time.modtime = 0;

	test_assert(utime(TEST_NOT_EXIST_FILE, &time) == -1);
}

TEST_CASE("times == NULL") {
	struct timeval now;
	struct stat filestat;

	int fd;

	test_assert((fd = creat(TEST_EXIST_FILE, 0666)) >= 0);
	test_assert(0 == close(fd));

	gettimeofday(&now, NULL);
	test_assert(utime(TEST_EXIST_FILE, NULL) == 0);

	test_assert(stat(TEST_EXIST_FILE, &filestat) == 0);

	test_assert(now.tv_sec <= filestat.st_ctime);
}

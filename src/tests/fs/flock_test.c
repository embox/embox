/**
 * @file
 * @brief
 *
 * @date Apr 3, 2014
 * @author Anton Bondarev
 */



#include <unistd.h>
#include <sys/file.h>
#include <fs/vfs.h>
#include <errno.h>
#include <kernel/printk.h>
#include <fs/file_operation.h>

/* TODO: Test cases for vfs fcntl advisory locking:
 *  - Command validation checks
 *  - Flock validation and fix checks
 *  - Permissions check for shared, exclusive lock and unlock
 *  - GETLK command test
 *  - Get dispositions checks for all variants
 *  - Lock and unlock operations checks for exclusive and shared locks
 *  - Combine and cut operations checks for exclusive and shared locks
 *  - Locks conversions checks
 *  - Test blocking and non-blocking mode
 */
TEST_CASE("vfs_fcntl advisory locking tests") {
	int fd;
	struct flock shlock = { F_RDLCK, SEEK_SET, 0, 0, 0 };
	struct flock exlock = { F_WRLCK, SEEK_SET, 0, 0, 0 };

	printk("\n\n========= vfs_fcntl test =========	\n");

	/*fd = open("/tmp/vfs_fcntl_test_file", O_CREAT | O_RDONLY | O_TRUNC,
			S_IRUSR | S_IWUSR);*/
	fd = open("/tmp/vfs_fcntl_test_file", O_CREAT | O_RDWR | O_TRUNC,
			S_IRUSR | S_IWUSR);

	test_assert_not_equal(-1, fd);
	test_assert_zero(vfs_fcntl(fd, F_SETLK, &shlock));

	test_assert_zero(vfs_fcntl(fd, F_SETLKW, &exlock));

	shlock.l_type = F_UNLCK;

	test_assert_zero(vfs_fcntl(fd, F_SETLK, &shlock));
}

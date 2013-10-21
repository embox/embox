/**
 * @file flock.c
 *
 * @brief flock usage example tested on Linux
 *
 * @date 17.07.2013
 * @author Ivan Tretyakov
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <framework/example/self.h>

#include <sys/file.h>
/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(flock_example);

#define SH_FLOCK_FILES  "/tmp/shflocktest"

int flock_example(int argc, char **argv) {
    int fd1, fd2;

    printf("Program started\n");

    system("touch "SH_FLOCK_FILES"1");
    printf("File "SH_FLOCK_FILES"1 created\n");
    system("touch "SH_FLOCK_FILES"2");
    printf("File "SH_FLOCK_FILES"2 created\n");

    fd1 = open(SH_FLOCK_FILES"1", O_RDONLY);
    printf("File "SH_FLOCK_FILES"1 opened\n");
    fd2 = open(SH_FLOCK_FILES"2", O_RDONLY);
    printf("File "SH_FLOCK_FILES"2 opened\n");

    /**
     * We can lock many files simaultenously using flock
     * We can free up lock more times than is was acquired
     * Lock of one type can be converted to lock of another type
     */

    if (0 == flock(fd1, LOCK_SH))
            printf("Shared lock acquired for file %s\n", SH_FLOCK_FILES"1");
    else
            printf("Problem with acquiring Shared lock for file %s\n", SH_FLOCK_FILES"1");

    if (0 == flock(fd2, LOCK_SH))
            printf("Shared lock acquired for file %s\n", SH_FLOCK_FILES"2");
    else
            printf("Problem with acquiring Shared lock for file %s\n", SH_FLOCK_FILES"2");

    if (0 == flock(fd2, LOCK_EX))
            printf("Shared lock converted to exclusive for file %s\n", SH_FLOCK_FILES"2");
    else
            printf("Problem with converting Shared lock to exclusive for file %s\n", SH_FLOCK_FILES"2");

    if (0 == flock(fd2, LOCK_SH))
            printf("Exclusive lock converted to shared for file %s\n", SH_FLOCK_FILES"2");
    else
            printf("Problem with converting exclusive lock to shared for file %s\n", SH_FLOCK_FILES"2");
    if (0 == flock(fd1, LOCK_UN))
            printf("Shared lock freed up for file %s\n", SH_FLOCK_FILES"1");
    else
            printf("Problem with freeing Shared lock for file %s\n", SH_FLOCK_FILES"1");

    if (0 == flock(fd2, LOCK_UN))
            printf("Shared lock freed up for file %s\n", SH_FLOCK_FILES"2");
    else
            printf("Problem with freeing Shared lock for file %s\n", SH_FLOCK_FILES"2");

    close(fd1);
    close(fd2);
    system("rm -f "SH_FLOCK_FILES"1");
    printf("File "SH_FLOCK_FILES"1 deleted\n");
    system("rm -f "SH_FLOCK_FILES"2");
    printf("File "SH_FLOCK_FILES"2 deleted\n");

    return 0;

}

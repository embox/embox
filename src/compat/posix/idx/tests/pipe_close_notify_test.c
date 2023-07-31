
/**
 * @file
 *
 * @date July 31, 2023
 * @author 
 */

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Pipe's close() notification test");

TEST_CASE("When pipe's writing end is closed, reading end should get notified") {
    int childpipe[2];
    fd_set readfs;
    char *buff;
    int res;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500;

    FD_ZERO(&readfs);
    buff = malloc(5*sizeof(char));
    memset(buff, 0, 5*sizeof(char));

    res = pipe(childpipe);
    if(res!=0)
        test_fail("Couldn't create pipe");

    FD_SET(childpipe[0], &readfs);
    write(childpipe[1], "test", 4);

    res = select(10, &readfs, NULL, NULL, &timeout);
    if(res<=0)
        test_fail("Error or timeout waiting for test string from pipe");
    res = read(childpipe[0], buff, 4);
    if(res<=0)
        test_fail("Didn't get test string");
        
    close(childpipe[1]);

    res = select(10, &readfs, NULL, NULL, &timeout);
    if(res<0)
        test_fail("Error in select() after pipe write end close()");
    if(res==0)
        test_fail("Select() timeout waiting for close() event notification");

    close(childpipe[0]);
    free(buff);
}

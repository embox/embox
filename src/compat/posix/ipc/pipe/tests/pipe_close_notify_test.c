
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
    fd_set readfds;
    char *buff;
    int res;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500;

    FD_ZERO(&readfds);
    buff = malloc(5*sizeof(char));
    memset(buff, 0, 5*sizeof(char));

    res = pipe(childpipe);
    if(res!=0)
        test_fail("Couldn't create pipe");

    /* First send a test string */
    FD_SET(childpipe[0], &readfds);
    write(childpipe[1], "test", 4);

    res = select(10, &readfds, NULL, NULL, &timeout);
    if(res<=0)
        test_fail("Error or timeout waiting for test string from pipe");
    res = read(childpipe[0], buff, 4);
    if(res<=0)
        test_fail("Didn't get test string");
        
    close(childpipe[1]);

    /* Now test if close event wakes up on select
     * and the read buffer is zero length*/
    res = select(10, &readfds, NULL, NULL, &timeout);
    if(res<0)
        test_fail("Error in select() after pipe write end close()");
    if(res==0)
        test_fail("Select() timeout waiting for close() event notification");
    if(res>0) {
        if(!FD_ISSET(childpipe[0],&readfds)) {
            test_fail("After select file descriptor isn't in readfds");
        } else {
            if(read(childpipe[0], buff, 4)!=0)
                test_fail("read() read more than 0 bytes after close event");
        }
    }

    /* Test that consequent select wakesup on reading from pipe,
     * and read buffer is still zero length*/
    res = select(10, &readfds, NULL, NULL, &timeout);
    if(res<0)
        test_fail("Error in second time select() after write-end close");
    if(res==0)
        test_fail("Timeout during second call of select() after close() event");
    if(res>0) {
        if(!FD_ISSET(childpipe[0],&readfds)) {
            test_fail("After second select() file descriptor isn't in readfds");
        } else {
            if(read(childpipe[0], buff, 4)!=0)
                test_fail("Second time read() read more than 0 bytes after close event");
        }
    }

    close(childpipe[0]);
    free(buff);
}

/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>
#include <module/embox/arch/syscall.h>
#include <types.h>

EMBOX_TEST_SUITE("basic syscall tests");

static int errno;

static _syscall3(int,write,int,fd,const char *,buf,int,count);

TEST_CASE("syscall") {
	write(1, "abc", 3);
}

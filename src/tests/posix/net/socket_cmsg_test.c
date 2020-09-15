/**
 * @file
 * @brief
 *
 * @date   15.09.2020
 * @author Alexander Kalmuk
 */

#include <embox/test.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

EMBOX_TEST_SUITE("Tests for CMSG macros");

TEST_CASE("Fill two CMSGs") {
	struct msghdr msg = { 0 };
	struct cmsghdr *cmsg;
	struct cmsghdr *cmsgs[2];
	int i;
	int data[2] = { 1, 2 };
	char iobuf[1];
	struct iovec io = {
		.iov_base = iobuf,
		.iov_len = sizeof(iobuf)
	};
	char buf[CMSG_SPACE(sizeof(data[0])) + CMSG_SPACE(sizeof(data[1]))];

	msg.msg_iov = &io;
	msg.msg_iovlen = 1;
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);

	for (cmsg = CMSG_FIRSTHDR(&msg), i = 0; cmsg != NULL;
			cmsg = CMSG_NXTHDR(&msg, cmsg), i++) {
		cmsgs[i] = cmsg;
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		cmsg->cmsg_len = CMSG_LEN(sizeof(data[i]));
		memcpy(CMSG_DATA(cmsg), &data[i], sizeof(data[i]));
	}
	test_assert_equal(i, 2);
	test_assert_equal((char *) cmsgs[0], buf);
	test_assert_equal((char *) cmsgs[1], buf + CMSG_SPACE(sizeof(data[0])));
	test_assert(CMSG_DATA(cmsgs[1]) > CMSG_DATA(cmsgs[0]));
}

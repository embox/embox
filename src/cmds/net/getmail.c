/**
 * @file
 * @brief get mail via POP3
 *
 * @date 16.03.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/cmd.h>
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <net/pop3.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	int ret;
	struct pop3_session p3s;

	if (argc != 4) {
		printf("Usage: %s user password server\n", argv[0]);
		return -EINVAL;
	}

	ret = pop3_session_create(&p3s, argv[3], POP3_PORT);
	if (ret != 0) goto error;

	ret = pop3_user(&p3s, argv[1]);
	if (ret != 0) goto error;

	ret = pop3_pass(&p3s, argv[2]);
	if (ret != 0) goto error;

	ret = pop3_retr(&p3s, 1);
	if (ret != 0) goto error;

	ret = pop3_quit(&p3s);
	if (ret != 0) goto error;

	ret = pop3_session_destroy(&p3s);
	if (ret != 0) {
		return ret;
	}

	return 0;
error:
	pop3_session_destroy(&p3s);
	return ret;
}

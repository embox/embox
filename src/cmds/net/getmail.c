/**
 * @file
 * @brief get mail via POP3
 *
 * @date 16.03.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <net/lib/pop3.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int ret;
	struct pop3_session p3s;
	char *password;

	if (argc != 3) {
		printf("Usage: %s user server\n", argv[0]);
		return -EINVAL;
	}

	ret = pop3_open(&p3s, argv[2], POP3_PORT);
	if (ret != 0) {
		printf("%s: pop3_open failure\n", argv[0]);
		return ret;
	}
	else if (!pop3_ok(&p3s)) goto error;

	ret = pop3_user(&p3s, argv[1]);
	if ((ret != 0) || !pop3_ok(&p3s)) goto error;

	password = getpass("Password: ");
	if (password == NULL) { ret = -EINVAL; goto error; }

	ret = pop3_pass(&p3s, password);
	if ((ret != 0) || !pop3_ok(&p3s)) goto error;

	ret = pop3_retr(&p3s, 1);
	if ((ret != 0) || !pop3_ok(&p3s)) goto error;

	ret = pop3_quit(&p3s);
	if ((ret != 0) || !pop3_ok(&p3s)) goto error;

	ret = pop3_close(&p3s);
	if (ret != 0) {
		return ret;
	}

	return 0;
error:
	if (ret == 0) {
		assert(!pop3_ok(&p3s));
		printf("%s: server error: %s\n", argv[0], pop3_status(&p3s));
	}
	pop3_quit(&p3s);
	pop3_close(&p3s);
	return ret;
}

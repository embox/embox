/**
 * @file
 * @brief
 *
 * @date 25.03.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/cmd.h>
#include <stdio.h>
#include <errno.h>
#include <stddef.h>
#include <net/lib/smtp.h>
#include <unistd.h>
#include <string.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	int ret;
	struct smtp_session ss;
	char *password;
	char from[SMTP_PATH_LEN + 1], to[SMTP_PATH_LEN + 1];
	char text[SMTP_TEXT_LEN + 1];
	size_t text_len;

	if (argc != 3) {
		printf("Usage: %s user server\n", argv[0]);
		return -EINVAL;
	}

	ret = smtp_open(&ss, argv[2], SMTP_PORT);
	if (ret != 0) {
		printf("%s: smtp_open failure\n", argv[0]);
		return ret;
	}
	else if (!smtp_ok(&ss)) goto error;


	ret = smtp_helo(&ss, "embox");
	if ((ret != 0) || !smtp_ok(&ss)) goto error;

	password = getpass("Password: ");
	if (password == NULL) { ret = -EINVAL; goto error; }

	ret = smtp_auth_plain(&ss, argv[1], password);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;

	fprintf(stdout, "Enter from/to email address with <>\n");
	fprintf(stdout, "brackets (<nick@mail.ru>, for example);\n");
	fprintf(stdout, "specify a email subject and message text\n");

	/* From */
	fprintf(stdout, "From: ");
	if (NULL == fgets(&from[0], sizeof from,
			stdin)) { ret = -EIO; goto error; }
	from[strlen(&from[0]) - 1] = '\0'; /* remove \n */

	ret = smtp_mail_from(&ss, &from[0]);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;
//	printf("%s: server: %s\n", argv[0], smtp_status(&ss));

	/* To */
	fprintf(stdout, "To: ");
	if (NULL == fgets(&to[0], sizeof to,
			stdin)) { ret = -EIO; goto error; }
	to[strlen(&to[0]) - 1] = '\0'; /* remove \n */

	ret = smtp_rcpt_to(&ss, &to[0]);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;
//	printf("%s: server: %s\n", argv[0], smtp_status(&ss));

	/* Data */
	ret = smtp_data(&ss);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;

	/* Data header: From, To */
	if (0 > snprintf(&text[0], sizeof text, "From: %s\r\nTo: %s\r\n",
			&from[0], &to[0])) { ret = -EIO; goto error; }

	ret = smtp_data_add(&ss, &text[0]);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;

	/* Data header: Subject */
	if (0 > snprintf(&text[0], sizeof text,
			"Subject: ")) { ret = -EIO; goto error; }
	text_len = strlen(&text[0]);
	fprintf(stdout, "%s", &text[0]);
	if (NULL == fgets(&text[0] + text_len, sizeof text - text_len,
			stdin)) { ret = -EIO; goto error; }
	text[strlen(&text[0]) - 1] = '\0'; /* remove \n */
	if (0 > snprintf(&text[0], sizeof text, "%s\r\n\r\n",
			&text[0])) { ret = -EIO; goto error; }

	ret = smtp_data_add(&ss, &text[0]);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;

	/* Data header: Message */
	fprintf(stdout, "Enter the message text (dot for exit):\n");

	while (1) {
		if (NULL == fgets(&text[0], sizeof text - 2, /* for \r\n */
				stdin)) { ret = -EIO; goto error; }
		text[strlen(&text[0]) - 1] = '\0'; /* remove \n */

		if (0 == strcmp(&text[0], ".")) {
			break;
		}

		if (0 > snprintf(&text[0], sizeof text, "%s\r\n",
				&text[0])) { ret = -EIO; goto error; }

		ret = smtp_data_add(&ss, &text[0]);
		if ((ret != 0) || !smtp_ok(&ss)) goto error;
	}

	ret = smtp_data_end(&ss);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;

	ret = smtp_quit(&ss);
	if ((ret != 0) || !smtp_ok(&ss)) goto error;

	ret = smtp_close(&ss);
	if (ret != 0) {
		return ret;
	}

	return 0;
error:
	if (ret == 0) {
		assert(!smtp_ok(&ss));
		printf("%s: server error: %s\n", argv[0], smtp_status(&ss));
	}
	smtp_quit(&ss);
	smtp_close(&ss);
	return ret;
}

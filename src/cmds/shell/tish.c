/**
 * @file
 * @brief Tiny Shell
 * @details New shell build around tiny readline impl.
 *	    Supports history and completions and tends to be extremely small.
 *
 * @date 13.09.11
 * @author Anton Kozlov
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h>
#include <kernel/task.h>
#include <cmd/cmdline.h>
#include <embox/unit.h>
#include <pwd.h>
#include <util/array.h>

#include <lib/linenoise_1.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <framework/cmd/api.h>

#include <cmd/shell.h>

#define BUF_INP_SIZE OPTION_GET(NUMBER, input_buf_size)

#define PROMPT_FMT OPTION_STRING_GET(prompt)

#define AMP_SUPPORT  OPTION_GET(NUMBER, amp_support)
#define RICH_PROMPT_SUPPORT OPTION_GET(NUMBER, rich_prompt_support)

#define PROMPT_BUF_LEN 32

struct cmd_data {
	int argc;
	char *argv[(BUF_INP_SIZE + 1) / 2];
	char buff[BUF_INP_SIZE];
	const struct cmd *cmd;
};

#if 0
static int cmd_compl(char *buf, char *out_buf) {
	const struct cmd *cmd = NULL;
	int buf_len = strlen(buf);
	int ret = 0;

	cmd_foreach(cmd) {
		if (strlen(cmd_name(cmd)) < buf_len) {
			continue;
		}
		if (strncmp(buf, cmd_name(cmd), buf_len) == 0) {
			strcpy(out_buf, cmd_name(cmd));
			out_buf += strlen(cmd_name(cmd)) + 1;
			ret++;
		}
	}
	return ret;

}
#endif

static void completion_hnd(const char *buf, linenoiseCompletions_t *lc) {
	const struct cmd *cmd = NULL;
	int buf_len = strlen(buf);

	cmd_foreach(cmd) {
		if (strlen(cmd_name(cmd)) < buf_len) {
			continue;
		}
		if (strncmp(buf, cmd_name(cmd), buf_len) == 0) {
			linenoiseAddCompletion(lc, (char *)cmd_name(cmd));
		}
	}
}

static void * process_cmd(void *data) {
	int ret;
	struct cmd_data *cdata;

	cdata = (struct cmd_data *)data;

	tcsetpgrp(STDIN_FILENO, getpid());

	ret = cmd_exec(cdata->cmd, cdata->argc, cdata->argv);
	if (ret != 0) {
		printf("%s: Command returned with code %d: %s\n",
				cmd_name(cdata->cmd), ret, strerror(-ret));
	}

	free(cdata);

	return NULL;
}

static int process_export(struct cmd_data *cdata) {
	int ret, i;
	char *equal;

	ret = 0;

	for (i = 1; i < cdata->argc; ++i) {
		equal = strchr(cdata->argv[i], '=');
		if (equal == NULL) {
			continue;
		}

		if (*(equal + 1) != '\0') {
			if (-1 == putenv(cdata->argv[i])) {
				ret = -errno;
				break;
			}
		}
		else {
			*equal = '\0';
			if (-1 == unsetenv(cdata->argv[i])) {
				ret = -errno;
				break;
			}
		}
	}

	free(cdata);
	return ret;
}

static int process_amp(struct cmd_data *cdata) {
	pid_t pid;

	pid = new_task(cdata->argv[0], process_cmd, cdata);
	if (pid < 0) {
		free(cdata);
		return pid;
	}

	return 0;
}

static int process(struct cmd_data *cdata) {
	pid_t pid;

	assert(cdata != NULL);

	if (0 == strcmp(cdata->argv[0], "exit")) {
		free(cdata);
		return -ENOSYS;
	}
	else if (0 == strcmp(cdata->argv[0], "logout")) {
		free(cdata);
		return -ENOSYS;
	}
	else if (0 == strcmp(cdata->argv[0], "export")) {
		return process_export(cdata);
	}

	cdata->cmd = cmd_lookup(cdata->argv[0]);
	if (cdata->cmd == NULL) {
		printf("%s: Command not found\n", cdata->argv[0]);
		free(cdata);
		return -ENOENT;
	}

	if (0 == strcmp(cdata->argv[cdata->argc - 1], "&")) {
		return process_amp(cdata);
	}

	pid = new_task(cdata->argv[0], process_cmd, cdata);
	if (pid < 0) {
		free(cdata);
		return pid;
	}

	task_waitpid(pid);

	return 0;
}

static int tish_exec(const char *cmdline) {
	struct cmd_data *cdata;

	if (strlen(cmdline) >= BUF_INP_SIZE) {
		return -EINVAL;
	}

	cdata = malloc(sizeof *cdata);
	if (cdata == NULL) {
		return -errno;
	}

	strcpy(&cdata->buff[0], cmdline);

	cdata->argc = cmdline_tokenize(&cdata->buff[0], &cdata->argv[0]);
	if (cdata->argc == 0) {
		return -EINVAL;
	}

	return process(cdata);
}

#define PWDBUF_LEN 64

static int rich_prompt(const char *fmt, char *buf, size_t len) {
	struct passwd pwdbuf, *pwd;
	char cpwdbuf[PWDBUF_LEN];
	const int uid = getuid();
	int state = 0;
	int ret;

	getpwuid_r(uid, &pwdbuf, cpwdbuf, ARRAY_SIZE(cpwdbuf), &pwd);

	for (; *fmt; fmt++) {
		if (len <= 0) {
			return -ENOMEM;
		}

		if (!state) {
			if (*fmt != '%') {
				*buf++ = *fmt;
				len --;
			} else {
				state = 1;
			}
			continue;
		}

		state = 0;

		switch(*fmt) {
		case 'u':
			if (!pwd) {
				ret = snprintf(buf, len, "%d", uid);
				break;
			}

			ret = snprintf(buf, len, "%s", pwd->pw_name);
			break;
		case 'h':
			ret = snprintf(buf, len, "embox");
			break;
		case '$':
			ret = snprintf(buf, len, "%c", uid ? '$' : '#');
			break;
		case 'w':
			ret = snprintf(buf, len, "%s", getenv("PWD"));
			break;
		default:
			ret = snprintf(buf, len, "%c", *fmt);
		}

		if (ret < 0) {
			return -EIO;
		}

		len -= ret;
		buf += ret;
	}

	return 0;
}

static void tish_run(void) {
	char *line;
	char prompt_buf[PROMPT_BUF_LEN];
	const char *prompt;

	/**
	 * Set the completion callback. This will be called every time the
	 * user uses the <tab> key.
	 */
	linenoiseSetCompletionCallback(completion_hnd);

#if 0
	/**
	 * Load history from file. The history file is just a plain text file
	 * where entries are separated by newlines.
	 */
	read_history("history.txt"); /* Load the history at startup */
#endif

	/**
	 * Now this is the main loop of the typical readline-based application.
	 * The call to readline will block as long as the user types something
	 * and presses enter.
	 */
	while (1) {
		if (RICH_PROMPT_SUPPORT) {
			prompt = 0 == rich_prompt(PROMPT_FMT, prompt_buf,
					ARRAY_SIZE(prompt_buf)) ? &prompt_buf[0] : PROMPT_FMT;
		}
		else {
			prompt = PROMPT_FMT;
		}

		line = readline(prompt);
		if (line == NULL) {
			continue;
		}

		/* Do something with the string. */
		if (line[0] != '\0' && line[0] != '/') {
			add_history(line); /* Add to the history. */
			(void)tish_exec(line);
		} else if (!strncmp(line,"/historylen",11)) {
			/* The "/historylen" command will change the history len. */
			int len = atoi(line+11);
			linenoiseHistorySetMaxLen(len);
		} else if (line[0] == '/') {
			printf("Unreconized command: %s\n", line);
		}
		free(line);
	}
}

#if 0
static void tish_run(void) {
	const char *prompt = OPTION_STRING_GET(prompt);
	char inp_buf[BUF_INP_SIZE];
	struct hist h;

	linenoise_history_init(&h);

	printf("\n%s\n\n", OPTION_STRING_GET(welcome_msg));

	while (1) {
		if (linenoise(prompt, inp_buf, BUF_INP_SIZE, &h,
			(compl_callback_t) cmd_compl) < 0) {
			return;
		}

		inp_buf[strlen(inp_buf) - 1] = '\0';
		linenoise_history_add(inp_buf, &h);
		if (0 > tish_exec(inp_buf)) {
			return;
		}
	}
}
#endif

SHELL_DEF({
	.name = "tish",
	.exec = tish_exec,
	.run  = tish_run,
	});

/**
 * @file
 * @brief Tiny Shell
 * @details New shell build around tiny readline impl.
 *	    Supports history and completions and tends to be extremely small.
 *
 * @date 13.09.11
 * @author Anton Kozlov
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

static struct cmdtask_data *get_task_data(int argc, char *argv[]);
static void *cmdtask(void *data);

struct cmdtask_data {
	int argc;
	char buf[BUF_INP_SIZE];
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

void completion_hnd(const char *buf, linenoiseCompletions_t *lc) {
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

static int run_cmd(int argc, char *argv[]) {
	const struct cmd *cmd;
	int code;

	if (NULL == (cmd = cmd_lookup(argv[0]))) {
		printf("%s: Command not found\n", argv[0]);
		return -ENOENT;
	}

	if (0 != (code = cmd_exec(cmd, argc, argv))) {
		printf("%s: Command returned with code %d: %s\n",
				cmd_name(cmd), code, strerror(-code));
	}
	return code;
}

#if AMP_SUPPORT
static int process_amp(int argc, char *argv[]) {
	struct cmdtask_data *m = get_task_data(argc - 1, argv);

	if (!m) {
		return -ENOMEM;
	}

	new_task(argv[0], cmdtask, m);

	return 0;
}

#else
static int process_amp(int argc, char *argv[]) {
	return -EINVAL;
}
#endif

static void *cmdtask(void *data) {
	struct cmdtask_data *m = (struct cmdtask_data *) data;
	char *argv[(BUF_INP_SIZE + 1) / 2], **pp, *p;
	pid_t pid;

	pid = getpid();
	tcsetpgrp(STDIN_FILENO, pid);

	pp = argv;
	p = m->buf;

	while (*p != '\0') {
		*pp++ = p;
		p += strlen(p) + 1;
	}

	run_cmd(m->argc, argv);

	free(m);

	return NULL;

}

static struct cmdtask_data *get_task_data(int argc, char *argv[]) {
	struct cmdtask_data *m = malloc(sizeof(struct cmdtask_data));
	char *p = m->buf;

	if (!m) {
		return m;
	}

	m->argc = argc;
	for (int i = 0; i < argc; i++) {
		strcpy(p, argv[i]);
		p += strlen(p) + 1;
	}

	*p = '\0';

	return m;
}

static int process_new_task_cmd(int argc, char *argv[]) {
	pid_t pid;
	struct cmdtask_data *m = get_task_data(argc, argv);

	if (!m) {
		return -ENOMEM;
	}

	pid = new_task(argv[0], cmdtask, m);
	if (pid < 0) {
		return pid;
	}
	task_waitpid(pid);

	return 0;
}

static int process_export(int argc, char *argv[]) {
	int i;
	char *equal;

	for (i = 1; i < argc; ++i) {
		if (NULL == (equal = strchr(argv[i], '='))) {
			continue;
		}

		if (*(equal + 1) != '\0') {
			if (-1 == putenv(argv[i])) {
				return -errno;
			}
		}
		else {
			*equal = '\0';
			if (-1 == unsetenv(argv[i])) {
				return -errno;
			}
		}
	}

	return 0;
}

static int process(int argc, char *argv[]) {
	assert(argc >= 1);
	assert(argv != NULL);

	if (0 == strcmp(argv[0], "exit")) {
		return -ENOSYS;
	}
	else if (0 == strcmp(argv[0], "logout")) {
		return -ENOSYS;
	}
	else if (0 == strcmp(argv[0], "export")) {
		return process_export(argc, argv);
	}
	else if (0 == strcmp(argv[argc - 1], "&")) {
		return process_amp(argc, argv);
	}

	return process_new_task_cmd(argc, argv);
}

static int tish_exec(const char *cmdline) {
	char cmdl[BUF_INP_SIZE];
	/* In the worst case cmdline looks like "x x x x x x". */
	char *argv[(BUF_INP_SIZE + 1) / 2];
	int argc;

	if (strlen(cmdline) >= BUF_INP_SIZE) {
		return -ERANGE;
	}

	strcpy(cmdl, cmdline);

	argc = cmdline_tokenize(cmdl, argv);
	if (argc == 0) {
		return -EINVAL;
	}

	return process(argc, argv);
}

#define PWDBUF_LEN 64

static int rich_prompt(const char *fmt, char *buf, size_t len) {
	struct passwd pwdbuf, *pwd;
	char cpwdbuf[PWDBUF_LEN];
	const int uid = getuid();
	int state = 0;
	int ret;

	getpwuid_r(uid, &pwdbuf, cpwdbuf, PWDBUF_LEN, &pwd);

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
					PROMPT_BUF_LEN) ? &prompt_buf[0] : PROMPT_FMT;
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
			if (0 != tish_exec(line)) {
				free(line);
				return;
			}
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


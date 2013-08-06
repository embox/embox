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
#include <limits.h>

#include <lib/linenoise_1.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <framework/cmd/api.h>

#include <cmd/shell.h>

#define PROMPT_FMT OPTION_STRING_GET(prompt)

#define RICH_PROMPT_SUPPORT OPTION_GET(NUMBER, rich_prompt_support)

#define BUILTIN_COMMANDS OPTION_STRING_GET(builtin_commands)

#define PROMPT_BUF_LEN 32

#define DEADSHELL_RET 0xDEAD5EE1

struct cmd_data {
	int argc;
	char *argv[(SHELL_INPUT_BUFF_SZ + 1) / 2];
	char buff[SHELL_INPUT_BUFF_SZ];
	const struct cmd *cmd;
};

static char * cmd_generator(const char *text, int state) {
	static int last_ind;
	static size_t text_len;
	int ind;
	const struct cmd *cmd;

	if (state == 0) {
		last_ind = 0;
		text_len = strlen(text);
	}

	ind = 0;
	cmd_foreach(cmd) {
		if (ind++ < last_ind) {
			continue;
		}
		if (strncmp(text, cmd_name(cmd), text_len) == 0) {
			last_ind = ind;
			return strdup(cmd_name(cmd));
		}
	}

	return NULL;
}

static char ** cmd_completion(const char *text, int start,
		int end) {
	char **matches;


	if (start == 0) {
		matches = rl_completion_matches((char *)text,
				&cmd_generator);
	}
	else {
		matches = NULL;
	}

	return matches;
}

static int is_builtin(const char *cname) {
	char *tmp;
	size_t cname_len;

	tmp = BUILTIN_COMMANDS;
	cname_len = strlen(cname);

	while (1) {
		if ((0 == strncmp(cname, tmp, cname_len))
				&& ((tmp[cname_len] == ' ') || (tmp[cname_len] == '\0'))) {
			return 1;
		}

		tmp = strchr(tmp, ' ');
		if (tmp == NULL) {
			break;
		}
		++tmp;
	}

	return 0;
}

static int process_builtin(struct cmd_data *cdata) {
	int ret;

	ret = cmd_exec(cdata->cmd, cdata->argc, cdata->argv);
	if (ret != 0) {
		printf("tish: %s: Command returned with code %d: %s\n",
				cmd_name(cdata->cmd), ret, strerror(-ret));
		free(cdata);
		return ret;
	}

	free(cdata);
	return 0;
}

static void * run_cmd(void *data) {
	int ret;
	struct cmd_data *cdata;

	cdata = (struct cmd_data *)data;

	if (-1 == tcsetpgrp(STDIN_FILENO, getpid())) {
		/* running noninteractive */
	}

	ret = cmd_exec(cdata->cmd, cdata->argc, cdata->argv);
	if (ret != 0) {
		printf("%s: Command returned with code %d: %s\n",
				cmd_name(cdata->cmd), ret, strerror(-ret));
		free(cdata);
		return (void *)ret; /* error: ret */
	}

	free(cdata);
	return NULL; /* ok */
}

static int process_external(struct cmd_data *cdata, int on_fg) {
	pid_t pid;
	int res;

	pid = new_task(cdata->argv[0], run_cmd, cdata);
	if (pid < 0) {
		free(cdata);
		return pid;
	}

	if (on_fg) {
		res = task_waitpid(pid);
	}

	return res;
}

static int process(struct cmd_data *cdata) {
	assert(cdata != NULL);

	/* TODO remove stubs */
	if (!strcmp(cdata->argv[0], "exit")
			|| !strcmp(cdata->argv[0], "logout")) {
		free(cdata);
		return DEADSHELL_RET;
	}

	cdata->cmd = cmd_lookup(cdata->argv[0]);
	if (cdata->cmd == NULL) {
		printf("%s: Command not found\n", cdata->argv[0]);
		free(cdata);
		return -ENOENT;
	}

	if (is_builtin(cmd_name(cdata->cmd))) {
		return process_builtin(cdata);
	}

	if (0 == strcmp(cdata->argv[cdata->argc - 1], "&")) {
		--cdata->argc;
		return process_external(cdata, 0);
	}

	return process_external(cdata, 1);
}

static int tish_exec(const char *cmdline) {
	struct cmd_data *cdata;

	if (strlen(cmdline) >= SHELL_INPUT_BUFF_SZ) {
		return -EINVAL;
	}

	cdata = malloc(sizeof *cdata);
	if (cdata == NULL) {
		return -errno;
	}

	strcpy(&cdata->buff[0], cmdline);

	cdata->argc = cmdline_tokenize(&cdata->buff[0], &cdata->argv[0]);
	if (cdata->argc == 0) {
		free(cdata);
		return -EINVAL;
	}

	return process(cdata);
}

static int rich_prompt(const char *fmt, char *buf, size_t len) {
	struct passwd *pwd;
	uid_t uid;
	int ret, after_percent;

	uid = getuid();
	pwd = getpwuid(uid);
	after_percent = 0;

	for (; *fmt != '\0'; fmt++) {
		if (len == 0) {
			return -ENOMEM;
		}

		if (!after_percent) {
			if (*fmt != '%') {
				*buf++ = *fmt;
				len--;
			} else {
				after_percent = 1;
			}
			continue;
		}

		after_percent = 0;

		switch (*fmt) {
		default:
			ret = snprintf(buf, len, "%c", *fmt);
			break;
		case 'u':
			ret = pwd != NULL ? snprintf(buf, len, "%s", pwd->pw_name)
					: snprintf(buf, len, "%d", uid);
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
	readline_init();
	rl_attempted_completion_function = cmd_completion;
	rl_bind_key('\t', rl_complete);

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
			if (DEADSHELL_RET == tish_exec(line)) {
				break;
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

SHELL_DEF({
	.name = "tish",
	.exec = tish_exec,
	.run  = tish_run,
	});

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
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <cmd/cmdline.h>
#include <cmd/shell.h>
#include <embox/unit.h>
#include <framework/cmd/api.h>
#include <kernel/task.h>
#include <kernel/task/resource/module_ptr.h>
#include <lib/libds/array.h>
#include <mem/sysmalloc.h>
#include <readline/history.h>
#include <readline/readline.h>

#define PROMPT_FMT OPTION_STRING_GET(prompt)

#define RICH_PROMPT_SUPPORT OPTION_GET(NUMBER, rich_prompt_support)

#define UNSET_NODELAY_MODE OPTION_GET(NUMBER, unset_nodelay_mode)

#define BUILTIN_COMMANDS OPTION_STRING_GET(builtin_commands)

#define PROMPT_BUF_LEN 32

struct cmd_data {
	int argc;
	char *argv[(SHELL_INPUT_BUFF_SZ + 1) / 2];
	char cmdline_buf[SHELL_INPUT_BUFF_SZ];
	const struct cmd *cmd;
	bool on_fg;
	volatile int started;
};

static char *cmd_generator(const char *text, int state) {
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

static char **cmd_completion(const char *text, int start, int end) {
	char **matches;

	if (start == 0) {
		matches = rl_completion_matches((char *)text, &cmd_generator);
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

	cdata->started = 1;

	ret = cmd_exec(cdata->cmd, cdata->argc, cdata->argv);
	if (ret != 0) {
		printf("tish: %s: Command returned with code %d: %s\n",
		    cmd_name(cdata->cmd), ret, strerror(-ret));

		return ret;
	}

	return 0;
}

static void cmd_data_copy(struct cmd_data *dst, const struct cmd_data *src) {
	int i_arg;
	char *dst_argv_p;

	dst->argc = src->argc;

	dst_argv_p = dst->cmdline_buf;
	for (i_arg = 0; i_arg < src->argc; i_arg++) {
		strcpy(dst_argv_p, src->argv[i_arg]);
		dst->argv[i_arg] = dst_argv_p;

		dst_argv_p += strlen(src->argv[i_arg]);
		*dst_argv_p++ = '\0';
	}

	dst->argv[dst->argc] = NULL;

	dst->cmd = src->cmd;
	dst->on_fg = src->on_fg;
}

static void *run_cmd(void *data) {
	int ret;
	struct cmd_data cdata;

	cmd_data_copy(&cdata, data);

	((struct cmd_data *)data)->started = 1;

	if (-1 == tcsetpgrp(STDIN_FILENO, getpid())) {
		/* running noninteractive */
	}

	task_self_module_ptr_set(cmd2mod(cdata.cmd));
	ret = cmd_exec(cdata.cmd, cdata.argc, cdata.argv);
	if (ret != 0) {
		printf("%s: Command returned with code %d: %s\n", cmd_name(cdata.cmd),
		    ret, strerror(-ret));
		return (void *)(uintptr_t)ret; /* error: ret */
	}

	return NULL; /* ok */
}

static int process_external(struct cmd_data *cdata) {
	int ret;
	pid_t pid;

	cdata->started = 0;

	pid = new_task(cdata->argv[0], run_cmd, cdata);
	if (pid < 0) {
		return pid;
	}

	if (cdata->on_fg) {
		waitpid(pid, &ret, 0);
		assert(WIFEXITED(ret));
		ret = WEXITSTATUS(ret);
	}
	else {
		while (cdata->started == 0) {
			sleep(0);
		}
		ret = 0;
	}

	return ret; /* TODO use task_errno() */
}

static int tish_cdata_fill(const char *cmdline, struct cmd_data *cdata) {
	if (strlen(cmdline) >= SHELL_INPUT_BUFF_SZ) {
		return -EINVAL;
	}

	strcpy(cdata->cmdline_buf, cmdline);

	cdata->argc = cmdline_tokenize(cdata->cmdline_buf, cdata->argv);
	if (cdata->argc == 0) {
		return -EINVAL;
	}

	cdata->cmd = cmd_lookup(cdata->argv[0]);
	if (cdata->cmd == NULL) {
		return -ENOENT;
	}

	if (0 != strcmp(cdata->argv[cdata->argc - 1], "&")) {
		cdata->on_fg = true;
	}
	else {
		cdata->on_fg = false;
		cdata->argv[cdata->argc--] = NULL;
	}

	return 0;
}

static int tish_exec(const char *cmdline) {
	struct cmd_data cdata;
	int res;

	res = tish_cdata_fill(cmdline, &cdata);
	if (res != 0) {
		switch (res) {
		case -ENOENT:
			printf("%s: Command not found\n", cdata.argv[0]);
			break;
		}
		return res;
	}

	if (is_builtin(cdata.argv[0])) {
		res = process_builtin(&cdata);
	}
	else {
		res = process_external(&cdata);
	}

	if (res < 0) {
		printf("tish: failed to exec %s: %s\n", cdata.argv[0], strerror(-res));
	}
	return res;
}

static void tish_collect_bg_childs(void) {
	int status;
	pid_t pid;

	do {
		pid = waitpid(-1, &status, WNOHANG);
	} while (pid > 0);
}

static int rich_prompt(const char *fmt, char *buf, size_t len) {
	struct passwd *pwd;
	uid_t uid;
	int ret, after_percent;

	uid = getuid();
	after_percent = 0;

	for (; *fmt != '\0'; fmt++) {
		if (len == 0) {
			return -ENOMEM;
		}

		if (!after_percent) {
			if (*fmt != '%') {
				*buf++ = *fmt;
				len--;
			}
			else {
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
			pwd = getpwuid(uid);
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

#if UNSET_NODELAY_MODE
static void sh_unset_nodelay_mode(void) {
	int orig_ifd_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	if (orig_ifd_flags & O_NONBLOCK) {
		if (fcntl(STDIN_FILENO, F_SETFL, orig_ifd_flags & ~O_NONBLOCK) == -1) {
			printf("cant unset_nodelay_mode\n");
		}
	}
}
#endif

static void tish_run(void) {
	char *line;
	char prompt_buf[PROMPT_BUF_LEN];
	const char *prompt;
	int err;

	/**
	 * Set the completion callback. This will be called every time the
	 * user uses the <tab> key.
	 */
	readline_init();
	rl_attempted_completion_function = cmd_completion;
	rl_bind_key('\t', rl_complete);

#if UNSET_NODELAY_MODE
	sh_unset_nodelay_mode();
#endif

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
			prompt =
			    0 == rich_prompt(PROMPT_FMT, prompt_buf, ARRAY_SIZE(prompt_buf))
			        ? &prompt_buf[0]
			        : PROMPT_FMT;
		}
		else {
			prompt = PROMPT_FMT;
		}

		line = readline(prompt);
		if (line == NULL) {
			if (errno == EAGAIN) {
				continue;
			}
			break;
		}

		/* Do something with the string. */
		if (line[0] != '\0' && line[0] != '/') {
			add_history(line); /* Add to the history. */
			err = tish_exec(line);
			if (err)
				printf("tish error: #%d\n", err);
		}
		else if (!strncmp(line, "/historylen", 11)) {
			/* The "/historylen" command will change the history len. */
			int len = atoi(line + 11);
			stifle_history(len);
		}
		else if (line[0] == '/') {
			printf("Unreconized command: %s\n", line);
		}

		readline_free(line);

		tish_collect_bg_childs();

#if UNSET_NODELAY_MODE
		sh_unset_nodelay_mode();
#endif
	}
}

SHELL_DEF({
    .name = "tish",
    .exec = tish_exec,
    .run = tish_run,
});

int main(int argc, char **argv) {
	tish_run();
	return 0;
}

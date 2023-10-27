/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 04.04.13
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmd/shell.h>
#include <lib/linenoise.h>
#include <readline/history.h>
#include <readline/readline.h>

#define DEFAULT_HISTORY_FILE "~/.history"

rl_compentry_func_t *rl_completion_entry_function;
rl_completion_func_t *rl_attempted_completion_function;

static rl_command_func_t *complcb_function;
static const char *complcb_text;
static linenoiseCompletions *complcb_lc;

char *readline(const char *prompt) {
	return linenoise(prompt != NULL ? prompt : "");
}

void readline_free(char *line) {
	if (line) {
		linenoiseFree(line);
	}
}

static void completion_callback(const char *text, linenoiseCompletions *lc) {
	complcb_text = text;
	complcb_lc = lc;
	complcb_function(0, '\t');
}

int rl_bind_key(int key, rl_command_func_t *function) {
	if (key == '\t') {
		complcb_function = function;
		linenoiseSetCompletionCallback(completion_callback);
		return 0;
	}

	return EINVAL;
}

int rl_complete(int ignore, int invoking_key) {
	const char *last_word;
	size_t start, end;
	char **matches, **match, buff[SHELL_INPUT_BUFF_SZ];

	if (invoking_key != '\t') {
		return EINVAL;
	}

	end = strlen(complcb_text);
	last_word = complcb_text + end;
	while ((last_word >= complcb_text) && !isspace(*last_word)) {
		--last_word;
	}
	start = ++last_word - complcb_text;

	if (rl_attempted_completion_function != NULL) {
		matches = rl_attempted_completion_function(last_word, start, end);
	}
	else {
		matches = NULL;
	}

	if (matches == NULL) {
		matches = rl_completion_matches(last_word,
		    (rl_completion_entry_function != NULL)
		        ? rl_completion_entry_function
		        : rl_filename_completion_function);
		if (matches == NULL) {
			return 0;
		}
	}

	strncpy(&buff[0], complcb_text, start);
	for (match = matches + 1; *match != NULL; ++match) {
		strcpy(&buff[start], *match);
		linenoiseAddCompletion(complcb_lc, &buff[0]);
		free(*match);
	}
	free(matches);

	return 0;
}

char **rl_completion_matches(const char *text,
    rl_compentry_func_t *entry_func) {
	int ind;
	char **matches, **tmp, *match;
	size_t matches_sz;

	match = entry_func(text, 0);
	if (match == NULL) {
		return NULL;
	}

	matches_sz = 3 * sizeof *matches; /* +1 for null-term array */
	matches = malloc(matches_sz);
	if (matches == NULL) {
		free(match);
		return NULL; /* error: no memory */
	}

	matches[0] = (char *)text;
	matches[1] = match;

	ind = 0;
	while (NULL != (match = entry_func(text, ++ind))) {
		matches[ind + 1] = match;
		matches_sz += sizeof *matches;
		tmp = realloc(matches, matches_sz);
		if (tmp == NULL) {
			while (ind >= 0) {
				free(matches[ind-- + 1]);
			}
			free(matches);
			return NULL; /* error: no memory */
		}
		matches = tmp;
	}

	matches[ind + 1] = NULL;

	return matches;
}

char *rl_filename_completion_function(const char *text, int state) {
	static DIR *dir = NULL;
	static char path[PATH_MAX], name[NAME_MAX];
	static size_t path_len, name_len;
	struct dirent *dent;
	const char *slash, *tmp;
	char filename[PATH_MAX];

	if (state == 0) {
		/* parse path and name */
		slash = NULL;
		tmp = text;
		while (NULL != (tmp = strchr(tmp, '/'))) {
			slash = tmp;
			++tmp;
		}
		if (slash == NULL) {
			path[0] = '\0';
			strcpy(&name[0], text);
		}
		else {
			strncpy(&path[0], text, (slash - text + 1));
			path[slash - text + 1] = '\0';
			strcpy(&name[0], slash + 1);
		}
		path_len = strlen(&path[0]);
		name_len = strlen(&name[0]);

		/* close old dir */
		if (dir != NULL) {
			closedir(dir);
		}
		/* open path dir */
		dir = opendir(&path[0]);
		if (dir == NULL) {
			return NULL; /* error: see errno */
		}
	}

	while (NULL != (dent = readdir(dir))) {
		if (strncmp(&name[0], &dent->d_name[0], name_len) == 0) {
			strcpy(&filename[0], &path[0]);
			strcpy(&filename[path_len], &dent->d_name[0]);
			return strdup(&filename[0]);
		}
	}

	closedir(dir);
	dir = NULL;

	return NULL;
}

void add_history(const char *line) {
	linenoiseHistoryAdd(line);
}

void stifle_history(int max) {
	linenoiseHistorySetMaxLen(max);
}

int read_history(const char *filename) {
	if (!filename) {
		filename = DEFAULT_HISTORY_FILE;
	}

	if (-1 == linenoiseHistoryLoad(filename)) {
		return errno;
	}

	return 0;
}

int write_history(const char *filename) {
	if (!filename) {
		filename = DEFAULT_HISTORY_FILE;
	}

	if (-1 == linenoiseHistorySave(filename)) {
		return errno;
	}

	return 0;
}

void readline_init(void) {
	/* init global */
	rl_completion_entry_function = NULL;
	rl_attempted_completion_function = NULL;

	/* init static */
	complcb_function = NULL;
	complcb_text = NULL;
	complcb_lc = NULL;
}

/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 04.04.13
 */

#include <errno.h>
#include <lib/linenoise_1.h>
#include <lib/readline.h>
#include <stddef.h>
#include <stdlib.h>

struct rl_compl {
	struct linenoiseCompletions linenoise_completion;
};

int rl_read(const char *prompt, char **out_line) {
	char *result;

	if (out_line == NULL) {
		return -EINVAL;
	}

	result = linenoise(prompt);
	if (result == NULL) {
		return -errno;
	}

	*out_line = result;
	return 0;
}

int rl_free(char *line) {
	if (line == NULL) {
		return -EINVAL;
	}

	free(line);
	return 0;
}

int rl_compl_set_hnd(rl_compl_hnd hnd) {
	if (hnd == NULL) {
		return -EINVAL;
	}

	linenoiseSetCompletionCallback((linenoiseCompletionCallback *)hnd);
	return 0;
}

int rl_compl_add(struct rl_compl *rlc, const char *completion) {
	if ((rlc == NULL) || (completion == NULL)) {
		return -EINVAL;
	}

	linenoiseAddCompletion(&rlc->linenoise_completion, (char *)completion);
	return 0;
}

int rl_hist_set_len(size_t len) {
	if (len == 0) {
		return -EINVAL;
	}

	if (0 == linenoiseHistorySetMaxLen(len)) {
		return -ENOMEM;
	}

	return 0;
}

int rl_hist_add(const char *line) {
	if (line == NULL) {
		return -EINVAL;
	}

	if (0 == linenoiseHistoryAdd(line)) {
		return -ENOMEM;
	}

	return 0;
}

int rl_hist_load(const char *file) {
	if (file == NULL) {
		return -EINVAL;
	}

	if (-1 == linenoiseHistoryLoad((char *)file)) {
		return -errno;
	}

	return 0;
}

int rl_hist_save(const char *file) {
	if (file == NULL) {
		return -EINVAL;
	}

	if (-1 == linenoiseHistorySave((char *)file)) {
		return -ENOMEM;
	}

	return 0;
}

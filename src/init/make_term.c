/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.10.2012
 */

#include <types.h>
#include <unistd.h>
#include <fcntl.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(make_term);

static inline struct idx_desc *data2idx_desc(struct idx_desc *data) {
	return task_idx_desc_data(data);
}

static int term_read(struct idx_desc *idx, void *buf, size_t nbyte) {
	struct idx_desc *raw_idx = data2idx_desc(idx);
	return task_idx_desc_ops(raw_idx)->read(raw_idx, buf, nbyte);
}

static int term_write(struct idx_desc *idx, const void *buf, size_t nbyte) {
	struct idx_desc *raw_idx = data2idx_desc(idx);
	char tbuf[nbyte * 2];
	char *tbufp = tbuf, *bufp = (char *) buf;
	while (nbyte--) {
		if (OPTION_GET(BOOLEAN,lf_crlf_map) &&
				*bufp == '\n' && *(bufp-1) != '\r') {
			*tbufp++ = '\r';
		}

		*tbufp++ = *bufp++;

	}
	return task_idx_desc_ops(raw_idx)->write(raw_idx, (const void *) tbuf, (int) tbufp - (int) tbuf);
}

static int term_close(struct idx_desc *idx) {
	struct idx_desc *raw_idx = data2idx_desc(idx);
	return task_idx_desc_free(raw_idx);
}

static const struct task_idx_ops task_idx_ops_term = {
		.read = term_read,
		.write = term_write,
		.close = term_close,
		.type = TASK_RES_OPS_TTY
};

int term(int fd) {
	int res;
	struct idx_desc *old_idx = task_self_idx_get(fd);
	struct idx_desc *new_idx = task_idx_desc_alloc(old_idx->data);

	*task_idx_desc_flags_ptr(new_idx) = *task_idx_desc_flags_ptr(old_idx);

	if (0 > (res = task_self_idx_alloc(&task_idx_ops_term, new_idx))) {
		task_idx_desc_free(new_idx);

		return res;
	}

	return res;

}

static int make_term(void) {
	int stdn = 2;

	while (stdn >= 0) {
		int new_stdn = term(stdn);
		dup2(new_stdn, stdn);
		close(new_stdn);

		stdn -= 1;
	}

	return 0;
}

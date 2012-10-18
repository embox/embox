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
#include <prom/prom_printf.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(make_term);

#define TRANSLATE_BUF_LEN 16

static inline struct idx_desc *data2idx_desc(struct idx_desc *data) {
	return task_idx_desc_data(data);
}

static int term_read(struct idx_desc *idx, void *buf, size_t nbyte) {
	struct idx_desc *raw_idx = data2idx_desc(idx);
	return task_idx_desc_ops(raw_idx)->read(raw_idx, buf, nbyte);
}

static int term_flush(struct idx_desc *idx, char *tbuf, char **tbufpp) {
	int res = task_idx_desc_ops(idx)->write(idx, (const void *) tbuf, (int) *tbufpp - (int) tbuf);

	if (res <= 0) {
		return res;
	}

	*tbufpp = tbuf;

	return res;

}

static int term_putc(struct idx_desc *idx, char *tbuf, char **tbufpp, char c) {
	int res = 0;

	if ((int) *tbufpp - (int) tbuf == TRANSLATE_BUF_LEN) {
		if ((res = term_flush(idx, tbuf, tbufpp)) <= 0) {
			return res;
		}
	}

	*(*tbufpp)++ = c;

	return 0;
}

static int term_write(struct idx_desc *idx, const void *buf, size_t nbyte) {
	struct idx_desc *raw_idx = data2idx_desc(idx);

	char tbuf[TRANSLATE_BUF_LEN];
	char *tbufp = tbuf, *bufp = (char *) buf;

	int was_r = 0;

	prom_printf("term_write 0x%x %d\n", (unsigned int) buf, nbyte);

	while (nbyte--) {

		if (OPTION_GET(BOOLEAN,lf_crlf_map) &&
				*bufp == '\n' && !was_r) {
			term_putc(raw_idx, tbuf, &tbufp, '\r');
		}

		was_r = *bufp == '\r' ? 1 : 0;

		term_putc(raw_idx, tbuf, &tbufp, *bufp++);

	}

	return term_flush(raw_idx, tbuf, &tbufp);
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

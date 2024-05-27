/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include "curses_priv.h"

#define CTRL_SEQ_MAX_LEN  5
#define CTRL_SEQ_BUF_SIZE CTRL_SEQ_MAX_LEN * 4

static struct ctrl_seq ctrl_seq_buf[CTRL_SEQ_BUF_SIZE];
static size_t ctrl_seq_buf_inuse;

static int add_ctrl_seq(char *str, int value) {
	struct ctrl_seq *ptr = _curs_ctrl_seq;

	if ((str == NULL) || (*str == '\0')) {
		return ERR;
	}

	while (*str) {
		if (ctrl_seq_buf_inuse == CTRL_SEQ_BUF_SIZE) {
			return ERR;
		}

		if (ptr->ch != 0) {
			while ((ptr->ch != *str) && (ptr->sibling != NULL)) {
				ptr = ptr->sibling;
			}
			if (ptr->ch == *str) {
				ptr = ptr->child;
				str++;
				continue;
			}
			ptr->sibling = &ctrl_seq_buf[ctrl_seq_buf_inuse++];
			ptr = ptr->sibling;
		}

		ptr->ch = *str++;
		if (*str) {
			ptr->child = &ctrl_seq_buf[ctrl_seq_buf_inuse++];
			ptr = ptr->child;
		}
	}

	ptr->value = value;

	return OK;
}

int _curs_ctrl_seq_init(void) {
	int rc = OK;

	if (cur_term == NULL) {
		return ERR;
	}

	_curs_ctrl_seq = ctrl_seq_buf;
	ctrl_seq_buf_inuse = 1;

	rc |= add_ctrl_seq(TINFO_KEY_DOWN, KEY_DOWN);
	rc |= add_ctrl_seq(TINFO_KEY_UP, KEY_UP);
	rc |= add_ctrl_seq(TINFO_KEY_LEFT, KEY_LEFT);
	rc |= add_ctrl_seq(TINFO_KEY_RIGHT, KEY_RIGHT);

	return rc;
}

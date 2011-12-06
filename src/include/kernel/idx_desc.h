/**
 * @file
 *
 * @brief
 *
 * @date 06.12.2011
 * @author Anton Bondarev
 */

#ifndef IDX_DESC_H_
#define IDX_DESC_H_

struct idx_desc {
	union {
		struct socket *socket;
		FILE *file;
		void *data;
	};
	int link_count;
	int type;
};

extern int idx_alloc(struct task *task, int type, void *res);

extern void idx_free(struct task *task, int idx);

#endif /* IDX_DESC_H_ */

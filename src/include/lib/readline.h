/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 04.04.13
 */

#ifndef LIB_READLINE_H_
#define LIB_READLINE_H_

#include <stddef.h>

extern int rl_read(const char *prompt, char **out_line);
extern int rl_free(char *line);

struct rl_compl;
typedef void (*rl_compl_hnd)(const char *pattern, struct rl_compl *rlc);

extern int rl_compl_set_hnd(rl_compl_hnd hnd);
extern int rl_compl_add(struct rl_compl *rlc, const char *completion);

extern int rl_hist_set_len(size_t len);
extern int rl_hist_add(const char *line);
extern int rl_hist_load(const char *file);
extern int rl_hist_save(const char *file);

#endif /* LIB_READLINE_H_ */

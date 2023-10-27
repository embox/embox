/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 04.04.13
 */

#ifndef READLINE_READLINE_H_
#define READLINE_READLINE_H_

typedef char *rl_compentry_func_t(const char *, int);
typedef char **rl_completion_func_t(const char *, int, int);
typedef int rl_command_func_t(int, int);

extern rl_compentry_func_t *rl_completion_entry_function;
extern rl_completion_func_t *rl_attempted_completion_function;

extern char *readline(const char *prompt);
extern void readline_free(char *line);
extern int rl_bind_key(int key, rl_command_func_t *function);
extern int rl_complete(int ignore, int invoking_key);
extern char **rl_completion_matches(const char *text,
    rl_compentry_func_t *entry_func);
extern char *rl_filename_completion_function(const char *text, int state);

extern void readline_init(void); /* module init */

#endif /* READLINE_READLINE_H_ */

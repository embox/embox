/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 04.04.13
 */

#ifndef READLINE_HISTORY_H_
#define READLINE_HISTORY_H_

extern void add_history(const char *line);
extern void stifle_history(int max);

extern int read_history(const char *filename);
extern int write_history(const char *filename);

#endif /* READLINE_HISTORY_H_ */

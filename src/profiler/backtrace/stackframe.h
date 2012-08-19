/**
 * @file
 * @brief TODO: brief
 *
 * @date 16 Aug, 2012
 * @author Bulychev Anton
 */

#ifndef STACKFRAME_H_
#define STACKFRAME_H_

struct stackframe;

void stackframe_set_current(struct stackframe*);
int stackframe_set_prev(struct stackframe*);

void stackframe_print(struct stackframe*);

#endif /* FRAME_H_ */

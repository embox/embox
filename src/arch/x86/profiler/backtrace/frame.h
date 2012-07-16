/**
 * @file
 * @brief TODO: brief
 *
 * @date 16 Jul, 2012
 * @author Bulychev Anton
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <stdio.h>

struct frame {
	void *fp;
	void *pc;
};

struct frame get_current_frame(void);
struct frame get_prev_frame(struct frame *f);

#endif /* FRAME_H_ */

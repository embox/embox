/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#ifndef TERM_PRIV_H_
#define TERM_PRIV_H_

#include <term.h>

#undef TRUE
#define TRUE true

#undef FALSE
#define FALSE false

#undef OK
#define OK 0

#undef ERR
#define ERR -1

typedef struct terminal {
	struct termtype type;
	int fildes;
#undef TERMINAL
} TERMINAL;

#define TPARM_1(s, a)    tparm(s, a, 0, 0, 0, 0, 0, 0, 0, 0)
#define TPARM_2(s, a, b) tparm(s, a, b, 0, 0, 0, 0, 0, 0, 0)

#endif /* TERM_PRIV_H_ */

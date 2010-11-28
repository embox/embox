/**
 * @file
 *
 * @date 24.11.2009
 * @author Anton Bondarev
 */
#ifndef TIME_H_
#define TIME_H_

#include <sys/types.h>

typedef __time_t time_t;

extern char *ctime(const time_t *timep, char *buff);

extern clock_t clock(void);

#endif /* TIME_H_ */

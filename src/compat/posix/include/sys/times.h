/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.05.2014
 */

#ifndef SYS_TIMES_H_
#define SYS_TIMES_H_

#include <sys/cdefs.h>
#include <sys/types.h>

/* Structure describing CPU time used by a process and its children.  */
struct tms
{
  clock_t tms_utime;          /* User CPU time.  */
  clock_t tms_stime;          /* System CPU time.  */

  clock_t tms_cutime;         /* User CPU time of dead children.  */
  clock_t tms_cstime;         /* System CPU time of dead children.  */
};

__BEGIN_DECLS
extern clock_t times(struct tms *);
__END_DECLS

#endif /* SYS_TIMES_H_ */


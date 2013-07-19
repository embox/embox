/**
 * @file contains flock definition
 * @brief file header contains flock definition
 * @details at the moment contains only flock related definitions
 *
 * @date 06.04.13
 * @author Ivan Tretyakov
 */

#ifndef _SYS_FILE_H
#define _SYS_FILE_H     1

#ifndef _FCNTL_H
# include <fcntl.h>
#endif

__BEGIN_DECLS


/* Operations for the `flock' call.  */
#define LOCK_SH 1       /* Shared lock.  */
#define LOCK_EX 2       /* Exclusive lock.  */
#define LOCK_UN 8       /* Unlock.  */

/* Can be OR'd in to one of the above.  */
#define LOCK_NB 4       /* Don't block when locking.  */


/* Apply or remove an advisory lock, according to OPERATION,
   on the file FD refers to.  */
extern int flock(int fd, int operation);


__END_DECLS

#endif /* sys/file.h  */

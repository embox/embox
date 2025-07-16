/**
 * @file
 * @brief ISO C99 Standard: 7.5 Errors.
 * @details Integer constants which represent error codes.
 *
 * @date 21.07.2015
 * @author Andrey Baboshin
 * @author Anton Kozlov
 */

#ifndef COMPAT_LIBC_ERRNO_H_
#define COMPAT_LIBC_ERRNO_H_

#include <posix_errno.h>

#define ENOERR           0     /* No error */
#define EPERM            1001  /* Not permitted */ /* FIXME: see issue 519 */
#define ENOENT           2     /* No such entity */
#define ESRCH            3     /* No such process */
#define EINTR            4     /* Operation interrupted */
#define EIO              5     /* I/O error */
#define E2BIG            7     /* Argument list too long. */
#define ENOEXEC          8     /* Exec format error */
#define EBADF            9     /* Bad file handle */
#define EAGAIN           11    /* Try again later */
#define	ECHILD           10    /* No child processes */
#define EWOULDBLOCK      EAGAIN
#define ENOMEM           12    /* Out of memory */
#define EFAULT           14    /* Bad adress posix.1 */
#define ENOTBLK          15    /* Not block device */
#define EBUSY            16    /* Resource busy */
#define EXDEV            18    /* Cross-device link */
#define ENODEV           19    /* No such device */
#define ENOTDIR          20    /* Not a directory */
#define EISDIR           21    /* Is a directory */
#define EINVAL           22    /* Invalid argument */
#define ENFILE           23    /* Too many open files in system */
#define EMFILE           24    /* Too many open files */
#define ETXTBSY          26    /* Text file busy */
#define EFBIG            27    /* File too large */
#define ENOSPC           28    /* No space left on device */
#define ESPIPE           29    /* Illegal seek */
#define EROFS            30    /* Read-only file system */
#define EMLINK           31    /* Too many links */
#define EDOM             33    /* Argument to math function out of domain */
#define ERANGE           34    /* Numerical result out of range */
#define EDEADLK          35    /* Resource deadlock would occur */
#define ENAMETOOLONG     36    /* File name too long */
#define EDEADLOCK        EDEADLK
#define ENOSYS           38    /* Function not implemented */


#define ELOOP            40    /* Too many levels of symbolic links. */
#define ENOMSG           42    /* No message of desired type */
#define EIDRM            43    /* Identifier removed */

#define ENOSTR           60    /* Device not a stream */
#define ENODATA          61    /* No data available */
#define ETIME            62    /* Timer expired */
#define ENOSR            63    /* Out of streams resources */
#define ENOTEMPTY        66    /* Directory not empty */
#define ENOLINK          67    /* Link has been severed */
#define EPROTO           71    /* Protocol error */
#define EOVERFLOW        75    /* Value too large to be stored in data type. */
#define EBADMSG          74    /* Not a data message */
#define EILSEQ           84    /* Illegal byte sequence */
#define ENOTSUP          95    /* Not supported error */

#define ECANCELED        125   /* Operation Canceled */

#define EOWNERDEAD       130   /* Owner died */
#define ENOTRECOVERABLE  131   /* State not recoverable */

#define EEOF             200   /* End of file reached */
#define ENOSUPP          201   /* Operation not supported */
#define EDEVNOSUPP       202   /* Device does not support this operation */
#define ENOLCK           203   /* No locks available. */

/* Additional errors used by networking */
#define ENXIO            300   /* Device not configured */
#define EACCES           301   /* Permission denied */
#define EEXIST           302   /* File exists */
#define ENOTTY           303   /* Inappropriate ioctl for device */
#define EPIPE            304   /* Broken pipe */

/* non-blocking and interrupt i/o */
#define EINPROGRESS      310   /* Operation now in progress */
#define EALREADY         311   /* Operation already in progress */

/* ipc/network software -- argument errors */
#define ENOTSOCK         320   /* Socket operation on non-socket */
#define EDESTADDRREQ     321   /* Destination address required */
#define EMSGSIZE         322   /* Message too long */
#define EPROTOTYPE       323   /* Protocol wrong type for socket */
#define ENOPROTOOPT      324   /* Protocol not available */
#define EPROTONOSUPPORT  325   /* Protocol not supported */
#define ESOCKTNOSUPPORT  326   /* Socket type not supported */
#define EOPNOTSUPP       327   /* Operation not supported */
#define EPFNOSUPPORT     328   /* Protocol family not supported */
#define EAFNOSUPPORT     329   /* Address family not supported by */
                               /* protocol family */
#define EADDRINUSE       330   /* Address already in use */
#define EADDRNOTAVAIL    331   /* Can't assign requested address */

/* ipc/network software -- operational errors */
#define ENETDOWN         350   /* Network is down */
#define ENETUNREACH      351   /* Network is unreachable */
#define ENETRESET        352   /* Network dropped connection on reset */
#define ECONNABORTED     353   /* Software caused connection abort */
#define ECONNRESET       354   /* Connection reset by peer */
#define ENOBUFS          355   /* No buffer space available */
#define EISCONN          356   /* Socket is already connected */
#define ENOTCONN         357   /* Socket is not connected */
#define ESHUTDOWN        358   /* Can't send after socket shutdown */
#define ETOOMANYREFS     359   /* Too many references: can't splice */
#define ETIMEDOUT        360   /* Operation timed out */
#define ECONNREFUSED     361   /* Connection refused */

#define EHOSTDOWN        364   /* Host is down */
#define EHOSTUNREACH     365   /* No route to host */

#endif /* COMPAT_LIBC_ERRNO_H_ */

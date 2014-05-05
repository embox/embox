/**
 * @file
 * @brief Implementation of #strerror() function.
 *
 * @date 13.10.09
 * @author Nikolay Korotky
 */

#include <string.h>
#include <errno.h>

char *strerror(int errnum) {
	switch (errnum) {
	case ENOERR:       return "No error";
	case EPERM:        return "Not permitted";
	case ENOENT:       return "No such entity";
	case ESRCH:        return "No such process";
	case EINTR:        return "Operation interrupted";
	case EIO:          return "I/O error";
	case EBADF:        return "Bad file handle";
	case EAGAIN:       return "Try again later";
	case ENOMEM:       return "Out of memory";
	case EBUSY:        return "Resource busy";
	case EXDEV:        return "Cross-device link";
	case ENODEV:       return "No such device";
	case ENOTDIR:      return "Not a directory";
	case EISDIR:       return "Is a directory";
	case EINVAL:       return "Invalid argument";
	case ENFILE:       return "Too many open files in system";
	case EMFILE:       return "Too many open files";
	case EFBIG:        return "File too large";
	case ENOSPC:       return "No space left on device";
	case ESPIPE:       return "Illegal seek";
	case EROFS:        return "Read-only file system";
	case EDOM:         return "Argument to math function outside valid";
	case ERANGE:       return "Math result cannot be represented";
	case EDEADLK:      return "Resource deadlock would occur";
	case ENOSYS:       return "Function not implemented";
	case ENAMETOOLONG: return "File name too long";
	case ENOTEMPTY:    return "Directory not empty";
	case ENOTSUP:      return "Not supported error";
	case EEOF:         return "End of file reached";
	case ENOSUPP:      return "Operation not supported";
	case EDEVNOSUPP:   return "Device does not support this operation";
	case ETIMEDOUT:    return "Connection timed out";
	case EACCES:       return "Access denied";
	case EEXIST:       return "File exists";
	case EINPROGRESS:  return "Operation now in progress";
	case EALREADY:     return "Operation already in progress";
	case ENOTSOCK:     return "Socket operation on non-socket";
	case EDESTADDRREQ: return "Destination address required";
	case EMSGSIZE:     return "Message too long";
	case EPROTOTYPE:   return "Protocol wrong type for socket";
	case ENOPROTOOPT:  return "Protocol not available";
	case EPROTONOSUPPORT: return "Protocol not supported";
	case ESOCKTNOSUPPORT: return "Socket type not supported";
	case EOPNOTSUPP:   return "Operation not supported";
	case EPFNOSUPPORT: return "Protocol family not supported";
	case EAFNOSUPPORT: return "Address family not supported by protocol family";
	case EADDRINUSE:   return "Address already in use";
	case EADDRNOTAVAIL: return "Can't assign requested address";
	case ENETDOWN:     return "Network is down";
	case ENETUNREACH:  return "Network is unreachable";
	case ENETRESET:    return "Network dropped connection on reset";
	case ECONNABORTED: return "Software caused connection abort";
	case ECONNRESET:   return "Connection reset by peer";
	case ENOBUFS:      return "No buffer space available";
	case EISCONN:      return "Socket is already connected";
	case ENOTCONN:     return "Socket is not connected";
	default:           return "Unknown error code. PLEASE REPORT ISSUE 519!";
	}
}

/**
 * @file
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
	default:           return "Unknown error code";
	}
}

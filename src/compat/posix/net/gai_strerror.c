/**
 * @file
 * @brief
 *
 * @date 29.10.13
 * @author Ilia Vaprol
 */

#include <netdb.h>

const char * gai_strerror(int error_code) {
	switch (error_code) {
	case 0:            return "no error";
	case EAI_AGAIN:    return "temporary failure in name resolution";
	case EAI_BADFLAGS: return "invalid flags value";
	case EAI_FAIL:     return "non-recoverable failure in name resolution";
	case EAI_FAMILY:   return "address family not supported";
	case EAI_MEMORY:   return "memory allocation failure";
	case EAI_NONAME:   return "host nor service provided, or not known";
	case EAI_OVERFLOW: return "argument buffer overflow";
	case EAI_SERVICE:  return "service not supported for socket type";
	case EAI_SOCKTYPE: return "socket type not supported";
	case EAI_SYSTEM:   return "system error";
        case EAI_NODATA:   return "no address associated with nodename";
	default:           return "unknown error";
	}
}

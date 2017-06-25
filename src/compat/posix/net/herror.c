/**
 * @file
 * @brief
 *
 * @date 31.08.12
 * @author Ilia Vaprol
 */

#include <netdb.h>
#include <stdio.h>

int h_errno;

const char * hstrerror(int error_code) {
	switch (error_code) {
	default:             return "unknown error";
	case 0:              return "no error";
	case HOST_NOT_FOUND: return "unknown host";
	case NO_ADDRESS:     return "no address associated with name";
	case NO_RECOVERY:    return "unknown server error";
	case TRY_AGAIN:      return "host name lookup failure";
	}
}

void herror(const char *msg) {
	fprintf(stderr, "%s: %s\n", msg, hstrerror(h_errno));
}

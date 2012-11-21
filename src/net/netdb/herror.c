/**
 * @file
 *
 * @date 31.08.12
 * @author Ilia Vaprol
 */

#include <netdb.h>
#include <stdio.h>

int h_errno;

const char * hstrerror(int err) {
	switch (err) {
	default:             return "unknown error";
	case NETDB_SUCCESS:  return "no error";
	case HOST_NOT_FOUND: return "unknown host";
	case TRY_AGAIN:      return "host name lookup failure";
	case NO_RECOVERY:    return "unknown server error";
	case NO_ADDRESS:     return "no address associated with name";
	}
};

void herror(const char *msg) {
	fprintf(stderr, "%s: %s\n", msg, hstrerror(h_errno));
}

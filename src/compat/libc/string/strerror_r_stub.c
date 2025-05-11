/**
 * @file
 * @brief Stub of #strerror_r() function.
 *
 * @date 11.05.25
 * @author Dmitry Pilyuk
 */

#include <stddef.h>
#include <errno.h>

int __xpg_strerror_r(int errnum, char *strerrbuf, size_t buflen) {
    SET_ERRNO(ENOSYS);
    return -1;
}
int strerror_r(int errnum, char *strerrbuf, size_t buflen) {
    return __xpg_strerror_r(errnum, strerrbuf, buflen);
}
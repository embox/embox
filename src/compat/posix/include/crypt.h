/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.05.2014
 */

#ifndef CRYPT_H_
#define CRYPT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern char *crypt(const char *key, const char *salt);

__END_DECLS

#endif /* CRYPT_H_ */


/**
 * @file
 *
 * @date Sep 9, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_INCLUDE_NETINET_IN_SYSTM_H_
#define SRC_COMPAT_POSIX_INCLUDE_NETINET_IN_SYSTM_H_

#include <sys/cdefs.h>

#include <sys/types.h>
#include <stdint.h>

__BEGIN_DECLS
/*
 * Network order versions of various data types. Unfortunately, BSD
 * assumes specific sizes for shorts (16 bit) and longs (32 bit) which
 * don't hold in general. As a consequence, the network order versions
 * may not reflect the actual size of the native data types.
 */
typedef uint16_t n_short;      /* short as received from the net */
typedef uint32_t n_long;       /* long as received from the net  */
typedef uint32_t n_time;       /* ms since 00:00 GMT, byte rev   */

__END_DECLS

#endif /* SRC_COMPAT_POSIX_INCLUDE_NETINET_IN_SYSTM_H_ */

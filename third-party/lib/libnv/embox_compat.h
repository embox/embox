/**
 * @file
 *
 * @date Sep 3, 2021
 * @author Anton Bondarev
 */

#ifndef THIRD_PARTY_LIB_LIBNV_EMBOX_COMPAT_H_
#define THIRD_PARTY_LIB_LIBNV_EMBOX_COMPAT_H_

#define __FBSDID(x)
#define __linux__
#define WITH_MSGIO   1

#define __packed __attribute__((packed))

#include <strings.h>
#include <endian.h>
#include <stdarg.h>

/* Weird but BIG_ENDIAN definition from endian.h does not appear */
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#define _VA_LIST_DECLARED

#include <nv.h>

#endif /* THIRD_PARTY_LIB_LIBNV_EMBOX_COMPAT_H_ */

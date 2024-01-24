/**
 * @file
 *
 * @date Jul 18, 2022
 * @author: Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_INCLUDE_DEFINES_SIGSET_T_DEFINE_H_
#define SRC_COMPAT_POSIX_INCLUDE_DEFINES_SIGSET_T_DEFINE_H_


#include <defines/_sig_total_define.h>

#include <lib/libds/bitmap.h>

typedef struct {
	BITMAP_DECL(bitmap, _SIG_TOTAL);
} sigset_t;


#endif /* SRC_COMPAT_POSIX_INCLUDE_DEFINES_SIGSET_T_DEFINE_H_ */

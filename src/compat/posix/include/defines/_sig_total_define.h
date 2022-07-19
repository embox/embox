/**
 * @file
 *
 * @date Jul 18, 2022
 * @author: Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_INCLUDE_DEFINES__SIG_TOTAL_DEFINE_H_
#define SRC_COMPAT_POSIX_INCLUDE_DEFINES__SIG_TOTAL_DEFINE_H_


#include <framework/mod/options.h>
#include <module/embox/kernel/task/resource/sig_table.h>

#define _SIG_TOTAL \
	OPTION_MODULE_GET(embox__kernel__task__resource__sig_table, \
		NUMBER, sig_table_size)

#endif /* SRC_COMPAT_POSIX_INCLUDE_DEFINES__SIG_TOTAL_DEFINE_H_ */

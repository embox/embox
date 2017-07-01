/*
 * @file
 *
 * @date 12.12.12
 * @author Anton Bondarev
 */

#ifndef ALLOCA_H_
#define ALLOCA_H_

#include <stddef.h> /* size_t in standard library */

#include <sys/cdefs.h>
__BEGIN_DECLS

extern void * alloca(size_t size);

#ifdef        __GNUC__
# define alloca(size)        __builtin_alloca (size)
#endif /* GCC.  */

__END_DECLS

#endif /* ALLOCA_H_ */

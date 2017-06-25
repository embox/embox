#ifndef GCC_LOCALE_H_
#define GCC_LOCALE_H_

#include_next <locale.h>

struct lconv;

#include <sys/cdefs.h>

__BEGIN_DECLS

struct lconv * localeconv(void);

__END_DECLS

#endif /* GCC_LOCALE_H_ */


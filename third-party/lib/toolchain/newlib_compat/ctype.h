#ifndef NEWLIB_CTYPE_H_
#define NEWLIB_CTYPE_H_

/* This is required because _U, _L, etc. are non-standard definitions,
 * but libstdc++ relies on such namings. */

#include_next "ctype.h"

#define _U  _CTYPE_U
#define _L  _CTYPE_L
#define _N  _CTYPE_D
#define _S  _CTYPE_S
#define _P  _CTYPE_P
#define _C  _CTYPE_C
#define _X  _CTYPE_X
#define _B  _CTYPE_SP

#endif /* NEWLIB_CTYPE_H_ */

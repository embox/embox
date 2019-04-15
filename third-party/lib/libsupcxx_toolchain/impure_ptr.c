/**
 * @file
 *
 * @date 15.04.19
 * @author Alexander Kalmuk
 */

#include <stddef.h>

/* When libsupcxx_toolchain is used, this pointer is required somewhy.
 * It's difficult to find exact reason why it's required, but probably,
 * cross toolchain assumes existance of smth like newlib and so
 * requests this pointer. */
void *_impure_ptr = NULL;

/**
 * @file
 * @brief Exception Handling routines
 * @date 28.12.12
 * @author Felix Sulima
 */

#include <c_compat/cstdlib>

#if defined(__EXCEPTIONS) && __EXCEPTIONS==1
#error Exceptions must be disabled
#endif


extern "C" void __cxa_pure_virtual() {
  std::abort();
  while (1);
}

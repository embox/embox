/**
 * @file
 * @brief Exception Handling routines
 * @date 28.12.12
 * @author Felix Sulima
 */

#include <cstdlib>

extern "C" void __cxa_pure_virtual() {
  std::abort();
  while (1);
}

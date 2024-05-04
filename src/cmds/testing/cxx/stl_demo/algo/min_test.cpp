/* confdefs.h */
#define PACKAGE_NAME "bm"
#define PACKAGE_TARNAME "bm"
#define PACKAGE_VERSION "1.15.0-unknown"
#define PACKAGE_STRING "bm 1.15.0-unknown"
#define PACKAGE_BUGREPORT "antonin@barefootnetworks.com"
#define PACKAGE_URL ""
#define PACKAGE "bm"
#define VERSION "1.15.0-unknown"
#define LOG_DEBUG_ON /**/
#define LOG_TRACE_ON /**/
#define WP4_16_STACKS /**/
#define STDC_HEADERS 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STRINGS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_DLFCN_H 1
#define LT_OBJDIR ".libs/"
#define HAVE_CXX11 1

#include <cstdint>
#include <cstddef>
/* end confdefs.h.  */
#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <algorithm>

#if defined(__GNUC__) && (__GNUC__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif
	#include "algostuff.hpp"
#if defined(__GNUC__) && (__GNUC__ >= 6)
#pragma GCC diagnostic pop
#endif

using namespace std;

// C++ program to demonstrate the use of std::min
#include <iostream>
#include <algorithm>
using namespace std;

// Defining the binary function
bool comp(int a, int b) {
	return (a < b);
}

int main() {
	int a = 5;
	int b = 7;
	cout << std::min(a, b, comp) << "\n";

	// Returns the first one if both the numbers
	// are same
	cout << std::min(7, 7, comp);

	return 0;
}

/*
 * ntfs-3g_embox_compat.h
 *
 *  Created on: 02 juillet 2013
 *      Author: fsulima
 */

#ifndef NTFS_EMBOX_COMPAT_H_
#define NTFS_EMBOX_COMPAT_H_


#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif



static inline void __x86_verificator__(void) {
	// This is to make sure this header only compiles on x86
	asm ("mov %%cr2, %%eax");
}
#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321
#define	__PDP_ENDIAN	3412
// x86 architecture only
#define __BYTE_ORDER __LITTLE_ENDIAN


#define S_ISVTX 01000


#define __timespec_defined


// errno values
#define EOVERFLOW 1
#define EMLINK    2
#define ENODATA   3
#define EILSEQ    4



#ifndef __need_getopt
/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
   of `struct option' terminated by an element containing a name which is
   zero.

   The field `has_arg' is:
   no_argument		(or 0) if the option does not take an argument,
   required_argument	(or 1) if the option requires an argument,
   optional_argument 	(or 2) if the option takes an optional argument.

   If the field `flag' is not NULL, it points to a variable that is set
   to the value given in the field `val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `optarg', set the
   option's `flag' field to zero and its `val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero `flag' field, `getopt'
   returns the contents of the `val' field.  */

struct option
{
  const char *name;
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

# define no_argument		0
# define required_argument	1
# define optional_argument	2
#endif	/* need getopt */

int getopt_long(int argc, char * const argv[],
           const char *optstring,
           const struct option *longopts, int *longindex);


#include <unistd.h>


#endif /* NTFS_EMBOX_COMPAT_H_ */

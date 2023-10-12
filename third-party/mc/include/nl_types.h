/* Copyright (C) 1996-2023 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef _NL_TYPES_H
#define _NL_TYPES_H 1

#include <features.h>

# if __GNUC_PREREQ (4, 6) && !defined _LIBC
#  define __LEAF , __leaf__
#  define __LEAF_ATTR __attribute__ ((__leaf__))
# else
#  define __LEAF
#  define __LEAF_ATTR
# endif

/* GCC can always grok prototypes.  For C++ programs we add throw()
   to help it optimize the function calls.  But this only works with
   gcc 2.8.x and egcs.  For gcc 3.4 and up we even mark C functions
   as non-throwing using a function attribute since programs can use
   the -fexceptions options for C code as well.  */
# if !defined __cplusplus && (__GNUC_PREREQ (3, 4))
#  define __THROW	__attribute__ ((__nothrow__ __LEAF))
#  define __THROWNL	__attribute__ ((__nothrow__))
#  define __NTH(fct)	__attribute__ ((__nothrow__ __LEAF)) fct
#  define __NTHNL(fct)  __attribute__ ((__nothrow__)) fct
# else
#  if defined __cplusplus && (__GNUC_PREREQ (2,8) || __clang_major >= 4)
#   if __cplusplus >= 201103L
#    define __THROW	noexcept (true)
#   else
#    define __THROW	throw ()
#   endif
#   define __THROWNL	__THROW
#   define __NTH(fct)	__LEAF_ATTR fct __THROW
#   define __NTHNL(fct) fct __THROW
#  else
#   define __THROW
#   define __THROWNL
#   define __NTH(fct)	fct
#   define __NTHNL(fct) fct
#  endif
# endif

#ifndef __attribute_nonnull__
# if __GNUC_PREREQ (3,3)
#  define __attribute_nonnull__(params) __attribute__ ((__nonnull__ params))
# else
#  define __attribute_nonnull__(params)
# endif
#endif
#ifndef __nonnull
# define __nonnull(params) __attribute_nonnull__ (params)
#endif


/* The default message set used by the gencat program.  */
#define NL_SETD 1

/* Value for FLAG parameter of `catgets' to say we want XPG4 compliance.  */
#define NL_CAT_LOCALE 1


__BEGIN_DECLS

/* Message catalog descriptor type.  */
typedef void *nl_catd;

/* Type used by `nl_langinfo'.  */
typedef int nl_item;

/* Open message catalog for later use, returning descriptor.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern nl_catd catopen (const char *__cat_name, int __flag) __nonnull ((1));

/* Return translation with NUMBER in SET of CATALOG; if not found
   return STRING.  */
extern char *catgets (nl_catd __catalog, int __set, int __number,
		      const char *__string) __THROW __nonnull ((1));

/* Close message CATALOG.  */
extern int catclose (nl_catd __catalog) __THROW __nonnull ((1));

__END_DECLS

#endif /* nl_types.h  */

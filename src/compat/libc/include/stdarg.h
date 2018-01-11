/*
 * This file is derived from GCC <stdarg.h> header.
 */

/* Copyright (C) 1989, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

/* As a special exception, if you include this header file into source
   files compiled by GCC, this header file does not by itself cause
   the resulting executable to be covered by the GNU General Public
   License.  This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU General
   Public License.  */

/**
 * @file
 * @brief C standard library header.
 * @details Allows functions to accept an indefinite number of arguments.
 *
 * @date 13.02.10
 * @author Eldar Abusalimov
 */

#ifndef STDARG_H_
#define STDARG_H_

#ifdef __LCC__
#define __STDARG_H

typedef void* va_list;
typedef va_list __gnuc_va_list;
#define _VA_LIST_
#define _VA_LIST va_list
#define _VA_LIST_DEFINED
#define _VA_LIST_T_H
#define __va_list__

#define __PAR_SIZE              8
#define __arg_size(x)           ((sizeof(x)+__PAR_SIZE-1) & ~(__PAR_SIZE-1))
#define va_start(ap, parmN)     \
     ((ap) = (va_list)((char *)(&parmN)+__arg_size(parmN)))

#define va_arg(ap, type)        \
        ( ( ap = (char*)ap +    \
            ((~((int)((char*)ap-(char*)0))+1) & (((__PAR_SIZE<<1)-1)>>(sizeof(type) <= __PAR_SIZE))) \
            ),\
        (*(type *)((char*)( ap = (char*)ap + __arg_size(type)) - __arg_size(type))) )

#define va_end(ap)              ((void)0)
#define va_copy(dest, src)      ((dest) = (src))

#else
/* If not LCC, we assume it's GCC */
#ifdef __GNUC__

typedef __builtin_va_list va_list;

#define va_start(v,l) __builtin_va_start(v,l)
#define va_end(v)     __builtin_va_end(v)
#define va_arg(v,l)   __builtin_va_arg(v,l)
#define va_copy(d,s)  __builtin_va_copy(d,s)

#else /* __GNUC__ */

#define va_list char *

#endif /* __GNUC__ */

#endif /* __LCC__ */

#endif /* STDARG_H_ */

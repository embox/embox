/*
 Copyright 2001, 2002 Georges Menie (www.menie.org)
 stdarg version contributed by Christian Ettinger

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 putchar is the only external dependency for this file,
 if you have a working putchar, leave it commented out.
 If not, uncomment the define below and
 replace outbyte(c) by your own function call.

 #define putchar(c) outbyte(c)
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "printf_impl.h"

static void stdout_printchar(struct printchar_handler_data *d, int c) {
	putchar(c);
}

int vprintf(const char *format, va_list args) {
	assert(format != NULL);

	return __print(stdout_printchar, NULL, format, args);
}

int printf(const char *format, ...) {
	int ret;
	va_list args;

	assert(format != NULL);

	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args);

	return ret;
}

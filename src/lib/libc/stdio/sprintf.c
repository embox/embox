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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "printf_impl.h"

struct printchar_handler_data {
	char *str;
};

static void str_printchar(struct printchar_handler_data *d, int c) {
	assert(d != NULL);
	assert(d->str != NULL);

	*d->str++ = c;
}

int vsprintf(char *str, const char *format, va_list args) {
	int ret;
	struct printchar_handler_data data;

	assert(str != NULL);
	assert(format != NULL);

	data.str = str;
	ret = __print(str_printchar, &data, format, args);
	assert(data.str != NULL);
	*data.str = '\0';

	return ret;
}

int sprintf(char *str, const char *format, ...) {
	int ret;
	va_list args;

	assert(str != NULL);
	assert(format != NULL);

	va_start(args, format);
	ret = vsprintf(str, format, args);
	va_end(args);

	return ret;
}

/**
 * @file
 *
 * @date Nov 13, 2020
 * @author Anton Bondarev
 */

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>
#include <inttypes.h>


struct printchar_handler_data {
	wchar_t *str;
	size_t cnt_buff;
};

static wchar_t strn_printchar(struct printchar_handler_data *d, wchar_t c) {
	assert(d != NULL);
	assert((d->str != NULL) || (d->cnt_buff == 0));

	if (d->cnt_buff) {
		*d->str++ = c;
		--d->cnt_buff;
	}

	return c;
}

extern int __wprint(wchar_t (*printchar_handler)(struct printchar_handler_data *d, wchar_t c),
		struct printchar_handler_data *printchar_data,
		const wchar_t *format, va_list args);

int vswprintf(wchar_t *restrict s, size_t n, const wchar_t *restrict fmt,
		va_list ap) {
	int ret;
	struct printchar_handler_data data;

	assert((s != NULL) || (n == 0));
	assert(fmt != NULL);

	data.str = s;
	data.cnt_buff = n ? n - 1 : 0;
	ret = __wprint(strn_printchar, &data, fmt, ap);
	if (n) *data.str = L'\0';

	return ret;
}

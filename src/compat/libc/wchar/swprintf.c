/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 05.07.24
 */

#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>

int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...) {
	int ret;
	va_list args;

	va_start(args, format);
	ret = vswprintf(s, n, format, args);
	va_end(args);

	return ret;
}

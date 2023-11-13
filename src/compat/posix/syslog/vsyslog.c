/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <stdarg.h>
#include <stdio.h>

void vsyslog(int priority, const char *message, va_list args) {
	vprintf(message, args);
	printf("\n");
}

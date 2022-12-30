/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 21.12.22
 */

#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <util/macro.h>
#include <asm/ptrace.h>
#include <framework/mod/options.h>

#include "arch.h"
#include "remote.h"
#include "breakpoint.h"

#define PACKET_SIZE OPTION_GET(NUMBER, packet_size)

static const char hexchars[] = "0123456789abcdef";

static void *stack_top;
static bool is_running = false;

int gdbstub_err;

static char *pack_str(char *dst, const char *src, uint8_t *checksum) {
	while (*src) {
		*checksum += *src;
		*dst++ = *src++;
	}
	return dst;
}

static char *pack_hex(char *dst, const void *src, size_t nbytes,
                      uint8_t *checksum) {
	uint8_t *ptr;
	char hi;
	char lo;

	for (ptr = (uint8_t *)src; nbytes; nbytes--, ptr++) {
		hi = hexchars[*ptr >> 4];
		lo = hexchars[*ptr & 0xf];
		*checksum += hi;
		*checksum += lo;
		*dst++ = hi;
		*dst++ = lo;
	}
	return dst;
}

int gdbstub(uint32_t nr, void *regs) {
	char buf[PACKET_SIZE + 1];
	char *request;
	char *buf_ptr;
	int n;
	uint8_t checksum;

	if (!is_running) {
		remove_breakpoint(*(void **)((uint8_t *)regs + PC_OFFSET));
		stack_top = *(void **)((uint8_t *)regs + SP_OFFSET);
	}

	for (;;) {
		if (is_running) {
			request = "$?";
			is_running = false;
		}
		else {
			n = remote_read(buf, sizeof(buf));
			if (n <= 0) {
				break;
			}

			request = memchr(buf, '$', n);
			if (!request) {
				continue;
			}
		}

		checksum = 0;
		buf_ptr = buf + 2;

		switch (request[1]) {
		case '?':
			buf_ptr = pack_str(buf_ptr, "S05", &checksum);
			break;

		case 'c':
			goto out;

		case 'D':
			remove_all_breakpoints();
			goto out;

		case 'g':
			buf_ptr = pack_hex(buf_ptr, regs, REG_LIST_SZ, &checksum);
			break;

		case 'H':
			buf_ptr = pack_str(buf_ptr, "OK", &checksum);
			break;

		case 'm': {
			void *addr;
			size_t len;

			addr = (void *)strtoul(request + 2, &request, 16);
			len = strtoul(request + 1, NULL, 16);
			if (addr > stack_top) {
				memset(buf_ptr, '0', len * 2);
				buf_ptr[len * 2] = '\0';
				buf_ptr = pack_str(buf_ptr, buf_ptr, &checksum);
			}
			else {
				buf_ptr = pack_hex(buf_ptr, addr, len, &checksum);
			}
			break;
		}

		case 'q':
			switch (request[2]) {
			case 'A':
				if (!memcmp(request + 2, "Attached", 8)) {
					buf_ptr = pack_str(buf_ptr, "1", &checksum);
				}
				break;

			case 'C':
				if (request[3] == '#') {
					buf_ptr = pack_str(buf_ptr, "QC00", &checksum);
				}
				break;

			case 'f':
				if (!memcmp(request + 2, "fThreadInfo", 11)) {
					buf_ptr = pack_str(buf_ptr, "m00", &checksum);
				}
				break;

			case 's':
				if (!memcmp(request + 2, "sThreadInfo", 11)) {
					buf_ptr = pack_str(buf_ptr, "l", &checksum);
				}
				break;

			case 'S':
				if (!memcmp(request + 2, "Symbol", 6)) {
					buf_ptr = pack_str(buf_ptr, "OK", &checksum);
				}
				else if (!memcmp(request + 2, "Supported", 9)) {
					buf_ptr = pack_str(buf_ptr,
					                   "qXfer:features:read+;"
					                   "PacketSize=" MACRO_STRING(PACKET_SIZE),
					                   &checksum);
				}
				break;

			case 'X':
				if (!memcmp(request + 2, "Xfer:features:read", 18)) {
					buf_ptr = pack_str(buf_ptr, FEATURE_STR, &checksum);
				}
				break;
			}
			break;

		case 'z':
			if (request[2] == '0') {
				void *addr;

				addr = (void *)strtoul(request + 4, NULL, 16);
				if (remove_breakpoint((void *)addr)) {
					buf_ptr = pack_str(buf_ptr, "OK", &checksum);
				}
				else {
					buf_ptr = pack_str(buf_ptr, "E01", &checksum);
				}
			}
			break;

		case 'Z':
			if (request[2] == '0') {
				void *addr;

				addr = (void *)strtoul(request + 4, NULL, 16);
				if (insert_breakpoint((void *)addr)) {
					buf_ptr = pack_str(buf_ptr, "OK", &checksum);
				}
				else {
					buf_ptr = pack_str(buf_ptr, "E01", &checksum);
				}
			}
			break;
		}

		*buf_ptr++ = '#';
		*buf_ptr++ = hexchars[checksum >> 4];
		*buf_ptr++ = hexchars[checksum & 0xf];

		buf[0] = '+';
		buf[1] = '$';

		n = remote_write(buf, buf_ptr - buf);
		if (n <= 0) {
			break;
		}
	}

	remove_all_breakpoints();
	gdbstub_err = n;

out:
	is_running = true;
	return 0;
}

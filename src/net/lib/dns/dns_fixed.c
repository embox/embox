/**
 * @file
 * @brief
 *
 * @date 22.08.12
 * @author Ilia Vaprol
 */

#include <net/lib/dns.h>

#include <framework/mod/options.h>

/**
 * DNS nameservers
 */
#define MODOPS_NAMESERVER OPTION_STRING_GET(nameserver)

const char * dns_get_nameserver(void) {
	return MODOPS_NAMESERVER;
}

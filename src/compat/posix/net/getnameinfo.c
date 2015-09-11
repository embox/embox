/**
 * @file
 * @brief
 *
 * @date 29.10.13
 * @author Ilia Vaprol
 */

#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <mem/misc/pool.h>
#include <errno.h>
#include <net/util/servent.h>
#include <stdio.h>

/**
 * FIXME Stub from Dropbear's fake-rfc2553.c
 */
int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host,
                size_t hostlen, char *serv, size_t servlen, int flags)
{
	struct sockaddr_in *sin = (struct sockaddr_in *)sa;
	struct hostent *hp;
	char tmpserv[16];

	if (sa->sa_family != AF_UNSPEC && sa->sa_family != AF_INET)
		return (EAI_FAMILY);
	if (serv != NULL) {
		snprintf(tmpserv, sizeof(tmpserv), "%d", ntohs(sin->sin_port));
		if (strlcpy(serv, tmpserv, servlen) >= servlen)
			return (EAI_MEMORY);
	}

	if (host != NULL) {
		if (flags & NI_NUMERICHOST) {
			if (strlcpy(host, inet_ntoa(sin->sin_addr),
			    hostlen) >= hostlen)
				return (EAI_MEMORY);
			else
				return (0);
		} else {
			hp = gethostbyaddr((char *)&sin->sin_addr,
			    sizeof(struct in_addr), AF_INET);
			if (hp == NULL)
				return (EAI_NODATA);

			if (strlcpy(host, hp->h_name, hostlen) >= hostlen)
				return (EAI_MEMORY);
			else
				return (0);
		}
	}
	return (0);
}

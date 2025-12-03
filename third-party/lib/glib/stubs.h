/**
 * @file
 *
 * @date May 29, 2014
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netinet/in.h>

struct ip_mreq_source {
	struct in_addr imr_multiaddr;  /* IP multicast group
									address */
	struct in_addr imr_interface;  /* IP address of local
									interface */
	struct in_addr imr_sourceaddr; /* IP address of
									multicast source */
};

int dn_expand(unsigned char *msg, unsigned char *eomorig,
              unsigned char *comp_dn, char *exp_dn,
              int length);

int res_query(const char *dname, int class, int type,
              unsigned char *answer, int anslen);

char *mktemp(char *template);


#include <dirent.h>
void seekdir(DIR *dirp, long offset);

long telldir(DIR *dirp);

int fnmatch(const char *pattern, const char *string, int flags);

/**
 * @file
 * @brief
 *
 * @date 17.05.2012
 * @author Andrey Gazukin
 */

#include <fs/nfs_clientstate.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <net/ip.h>
#include <net/socket.h>
#include <embox/cmd.h>
#include <err.h>
#include <errno.h>

#define INADDR_SRV          "192.168.0.1"//((unsigned long int) 0xC0A80001)
/*
 * RPC definitions for the portmapper
 */
#define	PMAPPORT		111
#define	PMAPPROG		100000
#define	PMAPVERS		2
#define	PMAPPROC_NULL		0
#define	PMAPPROC_SET		1
#define	PMAPPROC_UNSET		2
#define	PMAPPROC_GETPORT	3
#define	PMAPPROC_DUMP		4
#define	PMAPPROC_CALLIT		5

char buf[1024];

static int nfs_udp_sock(void) {
	int res, host;
	socklen_t addr_len;
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port= htons(PMAPPORT);
	inet_aton(INADDR_SRV, &addr.sin_addr);

	/* Create listen socket */
	host = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (host < 0) {
		printf("Error.. can't create socket. errno=%d\n", errno);
		return host;

	}
#if 0
	res = bind(host, (struct sockaddr *)&addr, sizeof(addr));
	if (res < 0) {
		printf("Error.. bind() failed. errno=%d\n", errno);
		return res;
	}

	res = listen(host, 1);
	if (res < 0) {
		printf("Error.. listen() failed. errno=%d\n", errno);
		return res;
	}
#endif
	for (int i=0; i<254; i++)
	{
		buf[i] = i;
	}
	//while (1) {
		res = sendto(host, (const void *)buf, 255, 0, (struct sockaddr *)&addr, sizeof(addr));
		usleep(100);
		res = recvfrom(host, buf, 1024, 0, (struct sockaddr *)&addr, &addr_len);
		if (res < 0) {
			/* error code in client, now */
			printf("udp rcv failed. errno=%d\n", errno);
			//close(host);
			//return res;
		}
		sleep(2);
	//}
	close(host);

	return 0;
}


int Mount_NFS_Filesystem(char *dev) {

	NFS_Getport(dev);

	NFS_Authentication(dev);

	NFS_Mount(dev);

	NFS_StatFS(dev);

	return 0;
}

int NFS_Getport(char *dev) {

	nfs_udp_sock();

	return 0;
}

int NFS_Authentication(char *dev) {

	return 0;
}

int NFS_Mount(char *dev) {

	return 0;
}

int NFS_StatFS(char *dev) {

	return 0;
}

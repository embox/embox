/**
 * @file
 * @brief
 *
 * @date 17.05.2012
 * @author Andrey Gazukin
 */

#include <fs/nfs_clientstate.h>
#include <fs/nfs.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <net/ip.h>
#include <net/socket.h>
#include <embox/cmd.h>
#include <err.h>
#include <errno.h>

#define DEBUG_TCP_MNTPORT 49488

char snd_buf[1024];
char rcv_buf[4096];

nfs_fs_description_t *p_fs_fd;
uint32_t xid;

static uint32_t get_xid(void) {
	return htonl(xid++);
}

static void opaque_data(char *point, uint32_t *len) {
	uint32_t opaque;

	opaque = 4 - ((*len) % 4);
	while (opaque--) {
		*point++ = 0;
		(*len)++;
	}
}

static void set_auth_unix(nfs_crdt_unix_t *crdt) {
    memset(crdt, 0, sizeof(*crdt));
	crdt->aux_unix = htonl(AUX_UNIX);
	crdt->len = htonl(EMBOX_STAMPLEN);
	crdt->stamp = htonl(EMBOX_STAMP);
	crdt->namelen = htonl(EMBOX_NAMELEN);
	memcpy(crdt->name, EMBOX_MACHNAME, EMBOX_NAMELEN);
	//crdt->aux_gid_qua = htonl(AUX_UNIX);
}

static int nfs_prepare(char *dev) {
	char *src, *dst;

	xid = 0x0001e242;

	src = dev;
	dst = p_fs_fd->srv_name;

	do {
		if ('\0' == *src) {
			return -1;
		}
		*dst++ = *src++;
	} while (':' != *src);

	src++;
	dst = p_fs_fd->srv_dir;
	do {
		*dst++ = *src++;
	} while ('\0' != *src);
	return 0;
}


static int nfs_getport(uint32_t proc, uint32_t ver,
		nfs_getport_args_t *gp_args) {
	int res;
	size_t len, arg_len;
	char *point;
	nfs_call_head_t call_head;
	nfs_reply_head_t reply_head;
	nfs_crdt_none_t crdt;

	call_head.xid = get_xid();
	call_head.msg_type = htonl(0);
	call_head.rpc_ver = htonl(2);
	call_head.prog_num = htonl(PMAP_PROGNUM);
	call_head.prog_ver = htonl(PMAP_VER);
	call_head.proc = htonl(PMAPPROC_GETPORT);

	point = snd_buf;
	memcpy(point, (const void *)&call_head, len = sizeof(call_head));
	point += len;


	crdt.flvr = crdt.flvr_len =	crdt.vrf_flvr = crdt.vrf_flvr_len = 0;
	memcpy(point, (const void *)&crdt, arg_len = sizeof(crdt));
	point += arg_len;
	len += arg_len;

	gp_args->prog = htonl(proc);
	gp_args->ver = htonl(ver);
	gp_args->proto = htonl(IPPROTO_TCP);
	gp_args->port = 0;

	memcpy(point, (const void *)gp_args, arg_len = sizeof(*gp_args));
	point += arg_len;
	len += arg_len;


	res = sendto(p_fs_fd->pm.sock, (const void *)snd_buf,len, 0,
			(struct sockaddr *)&p_fs_fd->pm.addr,
			sizeof(p_fs_fd->pm.addr));
	for(int i = 0; i < 20; i++) {
		res = recvfrom(p_fs_fd->pm.sock, rcv_buf, 1024, 0, NULL, 0);
		//		(struct sockaddr *)&p_fs_fd->pm.rcv_addr, &addr_len);
		if (res < 0) {
			/* error code in client, now */
			printf("udp rcv failed. errno=%d\n", errno);
			return -1;
		}
		else if (res > 0) {
			break;
		}
		usleep(10);
	}

	if (res > 0) {
		point = rcv_buf;

		memcpy(&reply_head , point, len = sizeof(reply_head));
		point += len;

		/* TODO check reply_head status */
		gp_args->port = ntohl(*(uint32_t *)point);
	}
	return 0;
}

static int nfs_mnt_export(void) {
	int res, dirlen;
	size_t len;
	uint32_t fh;
	uint32_t vf;
	char *point;
	socklen_t addr_len;
	nfs_call_head_t call_head;
	nfs_crdt_none_t crdt;
	nfs_reply_head_t reply_head;
	struct sockaddr_in dst;


	call_head.xid = get_xid();
	call_head.msg_type = htonl(0);
	call_head.rpc_ver = htonl(2);
	call_head.prog_num = htonl(MOUNT_PROGNUM);
	call_head.prog_ver = htonl(MOUNT_VER);
	call_head.proc = htonl(MOUNTPROC3_EXPORT);
	crdt.flvr = crdt.flvr_len =
			crdt.vrf_flvr = crdt.vrf_flvr_len = 0;

	point = snd_buf;
	*(uint16_t *)point = htons(0x8000);
	point += 2;
	*(uint16_t *)point = htons(sizeof(call_head) + sizeof(crdt));
	point += 2;

	memcpy(point, (const void *)&call_head, len = sizeof(call_head));
	point += len;
	len += 4;
	memcpy(point, (const void *)&crdt, sizeof(crdt));
	len += sizeof(crdt);

	res = sendto(p_fs_fd->mnt.sock, (const void *)snd_buf, len , 0,
			(struct sockaddr *)&p_fs_fd->mnt.addr,
			sizeof(p_fs_fd->mnt.addr));

	res = recvfrom(p_fs_fd->mnt.sock, rcv_buf, 1024, 0,
			(struct sockaddr *)&dst, &addr_len);

	if (res < 0) {
		/* error code in client, now */
		printf("mnt export failed. errno=%d\n", errno);
		return -1;
	}
	else if (res > 0) {
		point = rcv_buf;
		fh = ntohl(*(uint32_t *)point);
		point += sizeof(fh);
		/*TODO check fragment header*/
		memcpy(&reply_head , point, len = sizeof(reply_head));
		point += len;

		/* TODO check reply_head status */
		vf = ntohl(*(int *)point);
		point += sizeof(vf);

		if(VALUE_FOLLOWS_YES == vf) {
			dirlen = ntohl(*(int *)point);
			if(0 < dirlen) {
				p_fs_fd->exp_dir.len = htonl(dirlen);
				point += sizeof(p_fs_fd->exp_dir.len);

				memcpy(p_fs_fd->exp_dir.srcname, point, dirlen + 1);

				if(0 != strcmp(p_fs_fd->exp_dir.srcname, p_fs_fd->srv_dir)) {
					return -1;
				}
			}
			return 0;
		}
	}
	return -1;
}


static int nfs_mnt_null(void) {
	int res;
	size_t len, arglen;
	char *point;
	uint16_t *lenpoint;
	uint32_t fh;
	nfs_filehandle_t *p_fh;
	socklen_t addr_len;
	nfs_call_head_t call_head;
	nfs_crdt_unix_t *crdt;
	nfs_reply_head_t reply_head;

	p_fh = &p_fs_fd->fh;

	call_head.xid = get_xid();
	call_head.msg_type = htonl(0);
	call_head.rpc_ver = htonl(2);
	call_head.prog_num = htonl(MOUNT_PROGNUM);
	call_head.prog_ver = htonl(MOUNT_VER);
	call_head.proc = htonl(MOUNTPROC3_NULL);


	point = snd_buf;
	*(uint16_t *)point = htons(0x8000);
	point += 2;
	lenpoint = (uint16_t *)point;
	point += 2;

	memcpy(point, (const void *)&call_head, len = sizeof(call_head));
	point += len;

	crdt = &p_fs_fd->auth_head;
	set_auth_unix(crdt);
	memcpy(point, (const void *)crdt, arglen = sizeof(*crdt));
	point += arglen;
	len += arglen;

	/* add  fragment length */
	*lenpoint = htons(len);
	/* add size of fragment header */
	len += 4;

	res = sendto(p_fs_fd->mnt.sock, (const void *)snd_buf, len , 0,
			(struct sockaddr *)&p_fs_fd->mnt.addr,
			sizeof(p_fs_fd->mnt.addr));

	res = recvfrom(p_fs_fd->mnt.sock, rcv_buf, 1024, 0,
			(struct sockaddr *)&p_fs_fd->mnt.addr, &addr_len);
	if (res < 0) {
		/* error code in client, now */
		printf("mnt export failed. errno=%d\n", errno);
		return -1;
	}
	else if (res > 0) {
		point = rcv_buf;
		fh = ntohl(*(uint32_t *)point);
		point += sizeof(fh);
		memcpy(&reply_head , point, len = sizeof(reply_head));
		point += len;

		if(STATUS_OK == reply_head.accept_state) {
			return 0;
		}
	}
	return -1;
}

static int nfs_mnt_mount(void) {
	int res, dirlen;
	size_t len, arglen;
	char *point;
	uint16_t *lenpoint;
	uint32_t fh, status;
	nfs_filehandle_t *p_fh;
	socklen_t addr_len;
	nfs_call_head_t call_head;
	nfs_crdt_unix_t *crdt;
	nfs_reply_head_t reply_head;

	p_fh = &p_fs_fd->fh;

	call_head.xid = get_xid();
	call_head.msg_type = htonl(0);
	call_head.rpc_ver = htonl(2);
	call_head.prog_num = htonl(MOUNT_PROGNUM);
	call_head.prog_ver = htonl(MOUNT_VER);
	call_head.proc = htonl(MOUNTPROC3_MNT);


	point = snd_buf;
	*(uint16_t *)point = htons(0x8000);
	point += 2;
	lenpoint = (uint16_t *)point;
	point += 2;

	memcpy(point, (const void *)&call_head, len = sizeof(call_head));
	point += len;

	crdt = &p_fs_fd->auth_head;
	set_auth_unix(crdt);
	memcpy(point, (const void *)crdt, arglen = sizeof(*crdt));
	point += arglen;
	len += arglen;


	dirlen = ntohl(p_fs_fd->exp_dir.len);

	memcpy(point, (const void *)&p_fs_fd->exp_dir,
			arglen = (dirlen + sizeof(p_fs_fd->exp_dir.len)));
	len += arglen;
	point += arglen;

	opaque_data(point, &len);

	/* add  fragment length */
	*lenpoint = htons(len);
	/* add size of fragment header */
	len += 4;

	res = sendto(p_fs_fd->mnt.sock, (const void *)snd_buf, len , 0,
			(struct sockaddr *)&p_fs_fd->mnt.addr,
			sizeof(p_fs_fd->mnt.addr));

	res = recvfrom(p_fs_fd->mnt.sock, rcv_buf, 1024, 0,
			(struct sockaddr *)&p_fs_fd->mnt.addr, &addr_len);
	if (res < 0) {
		/* error code in client, now */
		printf("mnt export failed. errno=%d\n", errno);
		return -1;
	}
	else if (res > 0) {
		point = rcv_buf;
		fh = ntohl(*(uint32_t *)point);
		point += sizeof(fh);
		memcpy(&reply_head , point, len = sizeof(reply_head));
		point += len;

		/* TODO check reply_head status */
		status = ntohl(*(int *)point);
		point += sizeof(status);

		if(STATUS_OK == status) {
			memset(p_fh, 0, FHSIZE3);
			p_fh->len = *(int *)point;
			len = ntohl(p_fh->len);
			if(0 < len) {
				point += sizeof(p_fh->len);

				memcpy(p_fh->name, point, len);
				p_fh->count = htonl(DIRCOUNT);
				p_fh->maxcount = htonl(MAXDIRCOUNT);
				return 0;
			}
		}
	}
	return -1;
}

static int nfs_call(uint32_t proc) {

	return 0;
}

static int nfs_portmapper_sockinit(void) {
	int sock, res;
	struct sockaddr_in *addr;

	/* Create listen socket */
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		printf("Error.. can't create socket. errno=%d\n", errno);
		return sock;
	}

	addr = &p_fs_fd->pm.addr;

	addr->sin_family = AF_INET;
	addr->sin_port= htons(PMAP_PORT);
	inet_aton(p_fs_fd->srv_name, &addr->sin_addr);
	res = connect(sock, (struct sockaddr *)addr, sizeof(*addr));
	if (res < 0) {
		printf("Error.. portmapper udp connect() failed. errno=%d\n", errno);
		return res;
	}

	p_fs_fd->pm.sock = sock;
	return 0;
}

static int nfs_mnt_sockinit(uint32_t port) {
	int sock, res;
	struct sockaddr_in *addr;

	/* Create send socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		printf("Error.. can't create socket. errno=%d\n", errno);
		return sock;
	}

	addr = &p_fs_fd->mnt.addr;
	addr->sin_family = AF_INET;
	addr->sin_port= htons(port);
	inet_aton(p_fs_fd->srv_name, &addr->sin_addr);

	res = connect(sock, (struct sockaddr *)addr,  sizeof(*addr));
	if (res < 0) {
		printf("Error.. connect() failed. errno=%d\n", errno);
		return res;
	}

	p_fs_fd->mnt.sock = sock;
	return 0;
}

int mount_nfs_filesystem(void *par) {
	mount_params_t *params;
	node_t *node;
	nfs_file_description_t *fd;

	params = (mount_params_t *) par;
	node = params->dev_node;
	fd = (nfs_file_description_t *)node->attr;
	p_fs_fd = fd->p_fs_dsc;

	if(-1 == nfs_prepare(params->dev)) {
		return -1;
	}
	if(-1 == nfs_getports()) {
		return -1;
	}
	if(-1 == nfs_mount()) {
		return -1;
	}

	nfs_statfs();

	return 0;
}

int nfs_getports(void) {

	if (0 > nfs_portmapper_sockinit()) {
		return -1;
	}
	if (0 > nfs_getport(MOUNT_PROGNUM, MOUNT_VER, &p_fs_fd->mnt_arg)) {
		return -1;
	}
	if (0 > nfs_getport(NFS_PROGNUM, NFS_VER, &p_fs_fd->nfs_arg)) {
		return -1;
	}
	close(p_fs_fd->pm.sock);
	return 0;
}

int nfs_mount(void) {

	if(0 > nfs_mnt_sockinit(p_fs_fd->mnt_arg.port)) {
		return -1;
	}
	if (0 > nfs_mnt_export()) {
		return -1;
	}
	if (0 > nfs_mnt_null()) {
		return -1;
	}
	if (0 > nfs_mnt_mount()) {
		return -1;
	}
	close(p_fs_fd->mnt.sock);
	return 0;
}

int nfs_statfs(void) {

	nfs_call(NFSPROC3_NULL);
	nfs_call(NFSPROC3_FSSTAT);
	nfs_call(NFSPROC3_FSINFO);
	return 0;
}

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
#include <net/rpc/clnt.h>
#include <net/rpc/xdr.h>
#include <framework/example/self.h>
#include <sys/time.h>

#define DEBUG_TCP_MNTPORT 49488

static int xdr_mnt_export(struct xdr *xs, export_dir_t *export);
static int xdr_mnt_service(struct xdr *xs, mount_service_t *mnt_srvc);
static int xdr_nfs_set_fh(struct xdr *xs, nfs_filehandle_t *fh);
static int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh) ;

char snd_buf[1024];
char rcv_buf[4096];

nfs_fs_description_t *p_fs_fd;

int nfs_clnt_destroy (nfs_fs_description_t *p_fsfd) {

	if (NULL != p_fsfd->mnt) {
		clnt_destroy(p_fsfd->mnt);
		p_fsfd->mnt = NULL;
	}
	if (NULL != p_fsfd->nfs) {
		clnt_destroy(p_fsfd->nfs);
		p_fsfd->nfs = NULL;
	}
	return 0;
}

static int nfs_prepare(char *dev) {
	char *src, *dst;

	//xid = 0x0001e242;

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

static int nfs_mnt_export(void) {
	struct timeval timeout = { 25, 0 };

	memset((void *)&p_fs_fd->export, 0, sizeof(p_fs_fd->export));
	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_EXPORT,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_mnt_export, (char *)&p_fs_fd->export,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("mnt export failed. errno=%d\n", errno);
		return -1;
	}
	if(0 == strcmp(p_fs_fd->srv_dir, p_fs_fd->export.dir.data)) {
		return 0;
	}
	return -1;
}


static int nfs_mnt_null(void) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_NULL,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("mnt null failed. errno=%d\n", errno);
		return -1;
	}
	return 0;
}

static int nfs_mnt_mount(void) {
	struct timeval timeout = { 25, 0 };
	char *point;
	nfs_filehandle_t *p_fh;

	struct mount_service mnt_svc;
	memset(&mnt_svc, 0, sizeof(mnt_svc));

	point = p_fs_fd->export.dir.data;
	p_fh = &p_fs_fd->fh;

	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_MNT,
		(xdrproc_t)xdr_wrapstring, (char *)&point,
		(xdrproc_t)xdr_mnt_service, (char *)&mnt_svc,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("mnt mount failed. errno=%d\n", errno);
		return -1;
	}

	memcpy(p_fh, &mnt_svc.fh, sizeof(mnt_svc.fh));
	p_fh->count = DIRCOUNT;
	p_fh->maxcount = MAXDIRCOUNT;

	return 0;
}

static int nfs_nfs_null(nfs_filehandle_t *p_fh) {
	struct timeval timeout = { 25, 0 };
	char *point;

	point = (char *) p_fh;

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_NULL,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsstat(nfs_filehandle_t *p_fh) {
	struct timeval timeout = { 25, 0 };
	char *point;

	point = (char *) p_fh;

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSSTAT,
		(xdrproc_t)xdr_nfs_set_fh, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsinfo(nfs_filehandle_t *p_fh) {
	struct timeval timeout = { 25, 0 };
	char *point;

	point = (char *) p_fh;

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSINFO,
		(xdrproc_t)xdr_nfs_set_fh, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}
static int nfs_nfs_readdirplus(nfs_filehandle_t *p_fh) {
	struct timeval timeout = { 25, 0 };
	char *point;

	point = (char *) p_fh;

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_READDIRPLUS,
		(xdrproc_t)xdr_nfs_readdirplus, point,
		(xdrproc_t)xdr_void, rcv_buf,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("nfs readdirplus failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_call(uint32_t proc) {

	return 0;
}

static int nfs_client_init(void) {

	/* Create nfs programm clients */
	p_fs_fd->mnt = clnt_create(p_fs_fd->srv_name, MOUNT_PROGNUM, MOUNT_VER, "tcp");
	if (p_fs_fd->mnt == NULL) {
		/*
		 * Couldn't establish connection with server.
		 * Print error message and die.
		 */
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}

	p_fs_fd->nfs = clnt_create(p_fs_fd->srv_name, NFS_PROGNUM, NFS_VER, "tcp");
	if (p_fs_fd->nfs == NULL) {
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}
	return 0;
}

static int nfs_mount(void) {
	if (0 > nfs_mnt_export()) {
		return -1;
	}
	if (0 > nfs_mnt_null()) {
		return -1;
	}
	if (0 > nfs_mnt_mount()) {
		return -1;
	}
	clnt_destroy(p_fs_fd->mnt);
	p_fs_fd->mnt = NULL;

	if(0 >  nfs_nfs_null(&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  nfs_nfs_fsstat(&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  nfs_nfs_fsinfo(&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  nfs_nfs_readdirplus(&p_fs_fd->fh)) {
		return -1;
	}
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

	if(0 >  nfs_prepare(params->dev)) {
		return -1;
	}
	if(0 >  nfs_client_init()) {
		return -1;
	}
	if(0 >  nfs_mount()) {
		nfs_clnt_destroy(p_fs_fd);
		return -1;
	}


	//nfs_statfs();
	//nfs_clnt_destroy(p_fs_fd);

	return 0;
}

int nfs_statfs(void) {

	nfs_call(NFSPROC3_NULL);
	nfs_call(NFSPROC3_FSSTAT);
	nfs_call(NFSPROC3_FSINFO);
	return 0;
}

static int xdr_mnt_export(struct xdr *xs, export_dir_t *export) {
	char *point;

	assert(export != NULL);

	if (xdr_u_int(xs, &export->follow)) {
		if (VALUE_FOLLOWS_YES == export->follow) {
			point = export->dir.data;
			if (xdr_bytes(xs, (char **)&point,
					&export->dir.len, sizeof export->dir.data)) {
				return XDR_SUCCESS;
			}
		}
	}

	return XDR_FAILURE;
}

static int xdr_mnt_service(struct xdr *xs, mount_service_t *mnt_srvc) {
	char *point;

	assert(mnt_srvc != NULL);

	if (xdr_u_int(xs, &mnt_srvc->status)) {
		if (STATUS_OK == mnt_srvc->status) {
			point = mnt_srvc->fh.name;
			if (xdr_bytes(xs, (char **)&point,
					&mnt_srvc->fh.len, sizeof mnt_srvc->fh.name)) {
				if (xdr_u_int(xs, &mnt_srvc->flv)) {
					return XDR_SUCCESS;
				}
			}
		}
	}

	return XDR_FAILURE;
}

static int xdr_nfs_set_fh(struct xdr *xs, nfs_filehandle_t *fh) {
	char *point;

	assert(fh != NULL);

	point = fh->name;
	if(xdr_bytes(xs, (char **)&point, &fh->len, sizeof(*fh))) {
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;
}

static int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh) {
	char *point;
	__u32 size;

	assert(fh != NULL);

	point = fh->name;
	if(xdr_bytes(xs, (char **)&point, &fh->len, sizeof(*fh))) {
		point = (char *)&fh->cookie;
		size = sizeof(fh->cookie) + sizeof(fh->cookieverf);
		if(xdr_opaque(xs, point, size)) {
			if (xdr_u_int(xs, &fh->count)) {
				if (xdr_u_int(xs, &fh->maxcount)) {
					return XDR_SUCCESS;
				}
			}
		}
	}

	return XDR_FAILURE;
}

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

static int xdr_mnt_export(struct xdr *xs, export_dir_t *export);
static int xdr_mnt_service(struct xdr *xs, mount_service_t *mnt_srvc);
static int xdr_nfs_namestring(struct xdr *xs, rpc_string_t *fh);
static int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh);
static int xdr_nfs_get_dirlist(struct xdr *xs, char *buff);
static int xdr_nfs_get_dirdesc(struct xdr *xs, char *point);
static int xdr_nfs_get_attr(struct xdr *xs, char *point);

char snd_buf[MAXDIRCOUNT];
char rcv_buf[MAXDIRCOUNT];

nfs_fs_description_t *p_fs_fd;
nfs_file_description_t *p_rootfd;

static int nfs_unix_auth_set(struct client *clnt) {
	if (NULL == clnt)  {
		return -1;
	}
	if (NULL != clnt->ath) {
		auth_destroy(clnt->ath);
	}
	clnt->ath = authunix_create(EMBOX_MACHNAME, 0, 0, 0, NULL);
	if (clnt->ath == NULL) {
		return -1;
	}

	return 0;
}

static int nfs_clnt_destroy (nfs_fs_description_t *p_fsfd) {

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

	/* copy name of server and mount filesystem server directory*/
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
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
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
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
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
#if 0
	char *point;

	point = (char *) p_fh;
#endif
	if (clnt_call(p_fs_fd->nfs, NFSPROC3_NULL,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsstat(nfs_filehandle_t *p_fh) {
	struct timeval timeout = { 25, 0 };
	char *point;

	point = (char *) &p_fh->name_fh;

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSSTAT,
		(xdrproc_t)xdr_nfs_namestring, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsinfo(nfs_filehandle_t *p_fh) {
	struct timeval timeout = { 25, 0 };
	char *point;

	point = (char *) &p_fh->name_fh;

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSINFO,
		(xdrproc_t)xdr_nfs_namestring, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
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
		(xdrproc_t)xdr_nfs_get_dirlist, rcv_buf,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
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
	p_fs_fd->mnt = clnt_create(p_fs_fd->srv_name,
			MOUNT_PROGNUM, MOUNT_VER, "tcp");
	if (p_fs_fd->mnt == NULL) {
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}
	if(0 != nfs_unix_auth_set(p_fs_fd->mnt)) {
		return -1;
	}

	p_fs_fd->nfs = clnt_create(p_fs_fd->srv_name, NFS_PROGNUM, NFS_VER, "tcp");
	if (p_fs_fd->nfs == NULL) {
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}

	return nfs_unix_auth_set(p_fs_fd->nfs);
}

static int nfs_mount(void) {
	char *parent;

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

	parent = p_fs_fd->mnt_point;
	create_dir_entry(parent, rcv_buf);

	if(0 >  nfs_nfs_readdirplus(&p_fs_fd->fh)) {
		return -1;
	}
	//while(create_fs_nodes())

	return 0;
}


int mount_nfs_filesystem(void *par) {
	mount_params_t *params;
	node_t *node;
	nfs_file_description_t *fd;

	params = (mount_params_t *) par;
	node = params->dev_node;
	fd = p_rootfd = (nfs_file_description_t *)node->attr;
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
			point = mnt_srvc->fh.name_fh.data;
			if (xdr_bytes(xs, (char **)&point, &mnt_srvc->fh.name_fh.len,
					sizeof mnt_srvc->fh.name_fh)) {
				if (xdr_u_int(xs, &mnt_srvc->flv)) {
					return XDR_SUCCESS;
				}
			}
		}
	}

	return XDR_FAILURE;
}

static int xdr_nfs_namestring(struct xdr *xs, rpc_string_t *fh) {
	char *point;

	assert(fh != NULL);

	point = fh->data;
	if(xdr_bytes(xs, (char **)&point, &fh->len, sizeof(*fh))) {
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;
}

static int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh) {
	char *point;
	__u32 size;

	assert(fh != NULL);

	point = fh->name_fh.data;
	if(xdr_bytes(xs, (char **)&point, &fh->name_fh.len, sizeof(*fh))) {
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

static int xdr_nfs_get_dirattr(struct xdr *xs, char *point) {
	dir_attribute_rep_t *dir_attr;

	assert(point != NULL);

	dir_attr = (dir_attribute_rep_t *) point;
	if(XDR_SUCCESS == xdr_nfs_get_attr(xs, point)) {
		if (xdr_opaque(xs, (char *)&dir_attr->verifier,
				sizeof(dir_attr->verifier))) {
			return XDR_SUCCESS;
		}
	}
	return XDR_FAILURE;
}

static int xdr_nfs_get_name(struct xdr *xs, char *point) {
	file_name_t *file;

	assert(point != NULL);

	file = (file_name_t *) point;
	return (xdr_opaque(xs, (char *)&file->file_id, sizeof(file->file_id))
			&& xdr_nfs_namestring(xs, &file->name)
			&& xdr_opaque(xs, (char *)&file->cookie, sizeof(file->cookie))
			&& (int)(0 != (p_fs_fd->fh.cookie = file->cookie)));
}

static int xdr_nfs_get_attr(struct xdr *xs, char *point) {
	file_attribute_rep_t *attr;

	assert(point != NULL);

	attr = (file_attribute_rep_t *) point;
	return (xdr_u_int(xs, &attr->type) && xdr_u_int(xs, &attr->mode)
			&& xdr_u_int(xs, &attr->nlink) && xdr_u_int(xs, &attr->uid)
			&& xdr_u_int(xs, &attr->gid) && xdr_u_hyper(xs, &attr->size)
			&& xdr_u_hyper(xs, &attr->used) && xdr_u_int(xs, &attr->specdata1)
			&& xdr_u_int(xs, &attr->specdata2) && xdr_u_hyper(xs, &attr->fsid)
			&& xdr_u_hyper(xs, &attr->file_id)
			&& xdr_u_int(xs, &attr->atime.second)
			&& xdr_u_int(xs, &attr->atime.nano_sec)
			&& xdr_u_int(xs, &attr->mtime.second)
			&& xdr_u_int(xs, &attr->mtime.nano_sec)
			&& xdr_u_int(xs, &attr->ctime.second)
			&& xdr_u_int(xs, &attr->ctime.nano_sec));
}

static int xdr_nfs_get_dirdesc(struct xdr *xs, char *point) {
	readdir_desc_t *desc;
	__u32 *vf;

	assert(point != NULL);

	desc = (readdir_desc_t *) point;
	while(1) {
		if(XDR_SUCCESS != xdr_nfs_get_name(xs,
				(char *) &desc->file_name)) {
			break;
		}

		vf = &desc->vf_attr;
		if (xdr_u_int(xs, vf)) {
			if(VALUE_FOLLOWS_YES == *vf) {
				if(XDR_SUCCESS != xdr_nfs_get_attr(xs,
						(char *) &desc->file_attr)) {
					break;
				}
			}
		}

		vf = &desc->vf_fh;
		if (xdr_u_int(xs, vf)) {
			if(VALUE_FOLLOWS_YES == *vf) {
				if(XDR_SUCCESS != xdr_nfs_namestring(xs,
						&desc->file_handle.name_fh)) {
					break;
				}
			}
		}
		return XDR_SUCCESS;
	}
	return XDR_FAILURE;
}

/*
static size_t get_sizeof_readdirentry(readdir_desc_t *file) {
	size_t len;

	len = sizeof(file->file_name.file_id);
	len += sizeof(file->file_name.cookie);
	len += file->file_name.name.len + sizeof(file->file_name.name.len);

	len += sizeof(file->vf_attr);
	len += sizeof(file->file_attr);

	len += sizeof(file->vf_fh);
	len += file->file_handle.name_fh.len +
			sizeof(file->file_handle.name_fh.len);
	return len;
}
*/

static int xdr_nfs_get_dirlist(struct xdr *xs, char *buff) {
	char *point;
	__u32 vf;
	readdir_desc_t *file;

	assert(buff != NULL);
	point = buff;

	if (xdr_u_int(xs, &vf)) {
		if(STATUS_OK == (*(__u32 *) point = vf)) {
			point += sizeof(vf);
			if (xdr_u_int(xs,  &vf)) {
				if(VALUE_FOLLOWS_YES == (*(__u32 *) point = vf)) {
					point += sizeof(vf);
					if(xdr_nfs_get_dirattr(xs, point)) {
						point += sizeof(dir_attribute_rep_t);
						while(1) {
							if (xdr_u_int(xs, &vf)) {
								if(VALUE_FOLLOWS_YES ==
										(*(__u32 *) point = vf)) {
									point += sizeof(vf);
									file = (readdir_desc_t *)point;
									if(XDR_SUCCESS == xdr_nfs_get_dirdesc(xs,
											point)) {
										//point += get_sizeof_readdirentry(file);
										point += sizeof(readdir_desc_t);
									}
								}
								else {
									point += sizeof(vf);
									break;
								}
							}
						}
						/*read EOF */
						if (xdr_u_int(xs, (__u32 *) point)) {
							return XDR_SUCCESS;
						}
					}
				}
			}
		}
	}
	return XDR_FAILURE;
}

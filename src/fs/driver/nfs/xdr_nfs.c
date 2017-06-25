/**
 * @file
 * @brief
 *
 * @date 21.06.2012
 * @author Andrey Gazukin
 */
#include <sys/types.h>
#include <stddef.h>
#include <string.h>

#include <fs/nfs.h>
#include <fs/xdr_nfs.h>
#include <net/lib/rpc/xdr.h>

static int unaligned_xdr_u_hyper(struct xdr *xs, void *point) {
	int ret;
	uint64_t hyper;

	memcpy(&hyper, point, sizeof hyper);
	ret = xdr_u_hyper(xs, &hyper);
	memcpy(point, &hyper, sizeof hyper);
	return ret;
}

static int unaligned_test_non_zero_hyper(void *hyper) {
	uint64_t zero = 0;
	return 0 != memcmp(hyper, &zero, sizeof zero);
}

int xdr_mnt_export(struct xdr *xs, export_dir_t *export) {
	char *point;

	assert(export != NULL);

	if (xdr_u_int(xs, &export->vf)) {
		if (VALUE_FOLLOWS_YES == export->vf) {
			point = export->dir_name;
			if (xdr_bytes(xs, (char **)&point, (uint32_t *)&export->dir_len,
					(uint32_t) sizeof export->dir_name)) {
				return XDR_SUCCESS;
			}
		}
	}

	return XDR_FAILURE;
}

int xdr_mnt_service(struct xdr *xs, mount_service_t *mnt_srvc) {
	char *point;

	assert(mnt_srvc != NULL);

	if (xdr_u_int(xs, &mnt_srvc->status)) {
		if (STATUS_OK == mnt_srvc->status) {
			point = mnt_srvc->fh.name_fh.data;
			if (xdr_bytes(xs, (char **)&point,
					(uint32_t *)&mnt_srvc->fh.name_fh.len,
					sizeof mnt_srvc->fh.name_fh)) {
				if (xdr_u_int(xs, &mnt_srvc->flv)) {
					return XDR_SUCCESS;
				}
			}
		}
	}

	return XDR_FAILURE;
}

int xdr_nfs_namestring(struct xdr *xs, rpc_string_t *fh) {
	char *point;

	assert(fh != NULL);

	point = fh->data;
	if (xdr_bytes(xs, (char **)&point, (uint32_t *)&fh->len,
			(uint32_t) sizeof(*fh))) {
		point += fh->len;
		*point = 0;
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;
}

int xdr_nfs_name_fh(struct xdr *xs, rpc_fh_string_t *fh) {
	char *point;

	assert(fh != NULL);

	point = fh->data;
	if (xdr_bytes(xs, (char **)&point, (uint32_t *)&fh->len,
			(uint32_t) sizeof(*fh))) {
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;
}

int xdr_nfs_lookup(struct xdr *xs, char *point) {

	lookup_req_t *req;
	lookup_reply_t *reply;

	assert(point != NULL);

	switch (xs->oper) {
		case XDR_DECODE:
			reply = (lookup_reply_t *)point;
			if (xdr_u_int(xs, &reply->status) && (STATUS_OK == reply->status)
				&& xdr_nfs_name_fh(xs, reply->fh)
				&& xdr_u_int(xs, &reply->obj_vf)
				&& (VALUE_FOLLOWS_YES == reply->obj_vf)
				&& xdr_nfs_get_attr(xs, (char *) &reply->attr)
				&& xdr_u_int(xs, &reply->dir_vf)
				&& (VALUE_FOLLOWS_YES == reply->dir_vf)
				&& xdr_nfs_get_attr(xs, (char *) &reply->dir_attr)) {
				return XDR_SUCCESS;
			}
			break;
		case XDR_ENCODE:
			req = (lookup_req_t *)point;

			if (xdr_nfs_name_fh(xs, req->dir_fh)
					&& xdr_nfs_namestring(xs, req->fname)) {
				return XDR_SUCCESS;
			}
			break;
		case XDR_FREE:
			return XDR_SUCCESS;
		}

		return XDR_FAILURE;
}

int xdr_nfs_delete(struct xdr *xs, char *point) {

	lookup_req_t *req;
	delete_reply_t *reply;

	assert(point != NULL);

	switch (xs->oper) {
		case XDR_DECODE:
			reply = (delete_reply_t *)point;
			if (xdr_u_int(xs, &reply->status) && (STATUS_OK == reply->status)
				&& xdr_u_int(xs, &reply->before_vf)) {
				if (VALUE_FOLLOWS_YES == reply->before_vf) {
					if (XDR_SUCCESS !=
						xdr_nfs_get_del_attr(xs, (char *) &reply->before_attr)) {
						break;
					}
				}
				if (xdr_u_int(xs, &reply->dir_vf)
					&& (VALUE_FOLLOWS_YES == reply->dir_vf)
					&& xdr_nfs_get_attr(xs, (char *) reply->dir_attr)) {
					return XDR_SUCCESS;
				}
			}
			break;
		case XDR_ENCODE:
			req = (lookup_req_t *)point;

			if (xdr_nfs_name_fh(xs, req->dir_fh)
					&& xdr_nfs_namestring(xs, req->fname)) {
				return XDR_SUCCESS;
			}
			break;
		case XDR_FREE:
			return XDR_SUCCESS;
		}

		return XDR_FAILURE;
}

int xdr_nfs_create(struct xdr *xs, char *point) {

	create_req_t *req;
	create_reply_t *reply;

	assert(point != NULL);

	switch (xs->oper) {
		case XDR_DECODE:
			reply = (create_reply_t *)point;
			if (xdr_u_int(xs, &reply->status) && (STATUS_OK == reply->status)
				&& xdr_u_int(xs, &reply->fh_vf)
				&& (VALUE_FOLLOWS_YES == reply->fh_vf)
				&& xdr_nfs_name_fh(xs, &reply->obj_fh)
				&& xdr_u_int(xs, &reply->attr_vf)
				&& (VALUE_FOLLOWS_YES == reply->attr_vf)
				&& xdr_nfs_get_attr(xs, (char *) &reply->obj_attr)) {
				return XDR_SUCCESS;
			}
			break;
		case XDR_ENCODE:
			req = (create_req_t *)point;

			if (xdr_nfs_name_fh(xs, req->new.dir_fh)
				&& xdr_nfs_namestring(xs, req->new.fname)) {
				if (NFS_FILE_NODE_TYPE == req->type) {
					if (XDR_SUCCESS != xdr_u_int(xs, &req->create_mode)) {
						break;
					}
				}
				if ((NFS_DIRECTORY_NODE_TYPE == req->type) ||
					(GURDED_MODE == req->create_mode)) {
					if (xdr_u_int(xs, &req->mode_vf)
						&& xdr_u_int(xs, &req->mode)
						&& xdr_u_int(xs, &req->uid_vf)
						&& xdr_u_int(xs, &req->uid)
						&& xdr_u_int(xs, &req->gid_vf)
						&& xdr_u_int(xs, &req->gid)
						&& xdr_u_int(xs, &req->size_vf)
						&& unaligned_xdr_u_hyper(xs, &req->size)
						&& xdr_u_int(xs, &req->set_atime)
						&& xdr_u_int(xs, &req->set_mtime)){
						return XDR_SUCCESS;
					}
					break;
				}
				return XDR_SUCCESS;
			}
			break;
		case XDR_FREE:
			return XDR_SUCCESS;
		}
		return XDR_FAILURE;
}


int xdr_nfs_read_file(struct xdr *xs, char *point) {

	read_req_t *req;
	read_reply_t *reply;
	char *data;

	assert(point != NULL);

	switch (xs->oper) {
		case XDR_DECODE:
			reply = (read_reply_t *)point;
			data = reply->data;
			if (xdr_u_int(xs, &reply->status) && (STATUS_OK == reply->status)
				&& xdr_u_int(xs, &reply->vf)
				&& (VALUE_FOLLOWS_YES == reply->vf)
				&& xdr_nfs_get_attr(xs, (char *) &reply->attr)
				&& xdr_u_int(xs, &reply->count)
				&& xdr_u_int(xs, &reply->eof)
				&& xdr_bytes(xs, (char **)&data, &reply->datalen, XDR_LAST_UINT32)) {
				return XDR_SUCCESS;
			}
			break;
		case XDR_ENCODE:
			req = (read_req_t *)point;
			if (xdr_nfs_name_fh(xs, req->fh)
				&& unaligned_xdr_u_hyper(xs, &req->offset)
				&& xdr_u_int(xs, &req->count)) {
				return XDR_SUCCESS;
			}
			break;
		case XDR_FREE:
			return XDR_SUCCESS;
		}

		return XDR_FAILURE;
}

int xdr_nfs_write_file(struct xdr *xs, char *point) {

	write_req_t *req;
	write_reply_t *reply;
	char *data;

	assert(point != NULL);

	switch (xs->oper) {
		case XDR_DECODE:
			reply = (write_reply_t *)point;
			if (xdr_u_int(xs, &reply->status) && (STATUS_OK == reply->status)
				&& xdr_u_int(xs, &reply->before_vf)) {

				if (VALUE_FOLLOWS_YES == reply->before_vf) {
					if (XDR_SUCCESS != xdr_nfs_get_attr(xs,
						(char *) &reply->before_attr)) {
							break;
						}
					}

				if (xdr_u_int(xs, &reply->vf)
					&& (VALUE_FOLLOWS_YES == reply->vf)
					&& xdr_nfs_get_attr(xs, (char *) reply->attr)
					&& xdr_u_int(xs, &reply->count)
					&& xdr_u_int(xs, &reply->comitted)
					&& unaligned_xdr_u_hyper(xs, &reply->cookie_vrf)) {
						return XDR_SUCCESS;
					}
			}
			break;
		case XDR_ENCODE:
			req = (write_req_t *)point;
			data = req->data;

			if (xdr_nfs_name_fh(xs, req->fh) && unaligned_xdr_u_hyper(xs, &req->offset)
				&& xdr_u_int(xs, &req->count) && xdr_u_int(xs, &req->stable)
				&&  xdr_bytes(xs, (char **)&data, &req->datalen, XDR_LAST_UINT32)) {
				return XDR_SUCCESS;
			}
			break;
		case XDR_FREE:
			return XDR_SUCCESS;
		}

		return XDR_FAILURE;
}

int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh) {
	char *point;
	size_t size;

	assert(fh != NULL);

	point = fh->name_fh.data;
	if (xdr_bytes(xs, (char **)&point, (uint32_t *)&fh->name_fh.len,
			(uint32_t) sizeof(*fh))) {
		point = (char *)&fh->cookie;
		size = sizeof(fh->cookie) + sizeof(fh->cookieverf);
		if (xdr_opaque(xs, point, size)) {
			if (xdr_u_int(xs, &fh->count)) {
				if (xdr_u_int(xs, &fh->maxcount)) {
					return XDR_SUCCESS;
				}
			}
		}
	}

	return XDR_FAILURE;
}

int xdr_nfs_get_dirattr(struct xdr *xs, char *point) {
	dir_attribute_rep_t *dir_attr;

	assert(point != NULL);

	dir_attr = (dir_attribute_rep_t *) point;
	if (XDR_SUCCESS == xdr_nfs_get_attr(xs, point)) {
		if (xdr_opaque(xs, (char *)&dir_attr->verifier,
				sizeof(dir_attr->verifier))) {
			return XDR_SUCCESS;
		}
	}
	return XDR_FAILURE;
}

int xdr_nfs_get_name(struct xdr *xs, char *point) {
	file_name_t *file;

	assert(point != NULL);

	file = (file_name_t *) point;
	return (xdr_opaque(xs, (char *)&file->file_id, sizeof(file->file_id))
			&& xdr_nfs_namestring(xs, &file->name)
			&& xdr_opaque(xs, (char *)&file->cookie, sizeof(file->cookie))
			&& unaligned_test_non_zero_hyper(&file->cookie));
}

int xdr_nfs_get_attr(struct xdr *xs, char *point) {
	file_attribute_rep_t *attr;

	assert(point != NULL);

	attr = (file_attribute_rep_t *) point;
	return (xdr_u_int(xs, &attr->type) && xdr_u_int(xs, &attr->mode)
			&& xdr_u_int(xs, &attr->nlink) && xdr_u_int(xs, &attr->uid)
			&& xdr_u_int(xs, &attr->gid) && unaligned_xdr_u_hyper(xs, &attr->size)
			&& unaligned_xdr_u_hyper(xs, &attr->used) && xdr_u_int(xs, &attr->specdata1)
			&& xdr_u_int(xs, &attr->specdata2) && unaligned_xdr_u_hyper(xs, &attr->fsid)
			&& unaligned_xdr_u_hyper(xs, &attr->file_id)
			&& xdr_u_int(xs, &attr->atime.second)
			&& xdr_u_int(xs, &attr->atime.nano_sec)
			&& xdr_u_int(xs, &attr->mtime.second)
			&& xdr_u_int(xs, &attr->mtime.nano_sec)
			&& xdr_u_int(xs, &attr->ctime.second)
			&& xdr_u_int(xs, &attr->ctime.nano_sec));
}

int xdr_nfs_get_del_attr(struct xdr *xs, char *point) {
	file_del_attribute_rep_t *attr;

	assert(point != NULL);

	attr = (file_del_attribute_rep_t *) point;
	return (unaligned_xdr_u_hyper(xs, &attr->size)
			&& xdr_u_int(xs, &attr->mtime.second)
			&& xdr_u_int(xs, &attr->mtime.nano_sec)
			&& xdr_u_int(xs, &attr->ctime.second)
			&& xdr_u_int(xs, &attr->ctime.nano_sec));
}

int xdr_nfs_get_dirdesc(struct xdr *xs, char *point) {
	readdir_desc_t *desc;
	uint32_t *vf;

	assert(point != NULL);

	desc = (readdir_desc_t *) point;
	while(1) {
		if (XDR_SUCCESS != xdr_nfs_get_name(xs, (char *) &desc->file_name)) {
			break;
		}

		vf = &desc->vf_attr;
		if (xdr_u_int(xs, vf)) {
			if (VALUE_FOLLOWS_YES == *vf) {
				if (XDR_SUCCESS != xdr_nfs_get_attr(xs,
						(char *) &desc->file_attr)) {
					break;
				}
			}
		}

		vf = &desc->vf_fh;
		if (xdr_u_int(xs, vf)) {
			if (VALUE_FOLLOWS_YES == *vf) {
				if (XDR_SUCCESS != xdr_nfs_name_fh(xs,
						&desc->file_handle.name_fh)) {
					break;
				}
			}
		}
		return XDR_SUCCESS;
	}
	return XDR_FAILURE;
}

int xdr_nfs_get_dirlist(struct xdr *xs, char *buff) {
	char *point;
	uint32_t vf;
	/*readdir_desc_t *file;*/

	assert(buff != NULL);
	point = buff;

	if (xdr_u_int(xs, &vf)) {
		if (STATUS_OK == (*(uint32_t *) point = vf)) {
			point += sizeof(vf);
			if (xdr_u_int(xs,  &vf)) {
				if (VALUE_FOLLOWS_YES == (*(uint32_t *) point = vf)) {
					point += sizeof(vf);
					if (xdr_nfs_get_dirattr(xs, point)) {
						point += sizeof(dir_attribute_rep_t);
						while(1) {
							if (xdr_u_int(xs, &vf)) {
								if (VALUE_FOLLOWS_YES ==
										(*(uint32_t *) point = vf)) {
									point += sizeof(vf);
									/*file = (readdir_desc_t *)point;*/
									if (XDR_SUCCESS == xdr_nfs_get_dirdesc(xs,
											point)) {
										point += sizeof(readdir_desc_t);
									}
								} else {
									point += sizeof(vf);
									break;
								}
							}
						}
						/*read EOF */
						if (xdr_u_int(xs, (uint32_t *) point)) {
							return XDR_SUCCESS;
						}
					}
				}
			}
		}
	}
	return XDR_FAILURE;
}

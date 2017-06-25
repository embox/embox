/**
 * @file
 * @brief
 *
 * @date 21.06.2012
 * @author Andrey Gazukin
 */


#ifndef XDR_NFS_H_
#define XDR_NFS_H_

#include <net/lib/rpc/xdr.h>

extern int xdr_mnt_export(struct xdr *xs, export_dir_t *export);

extern int xdr_mnt_service(struct xdr *xs, mount_service_t *mnt_srvc);

extern int xdr_nfs_namestring(struct xdr *xs, rpc_string_t *str);

extern int xdr_nfs_name_fh(struct xdr *xs, rpc_fh_string_t *fh);

extern int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh);

extern int xdr_nfs_get_dirlist(struct xdr *xs, char *buff);

extern int xdr_nfs_get_dirdesc(struct xdr *xs, char *point);

extern int xdr_nfs_get_attr(struct xdr *xs, char *point);

extern int xdr_nfs_get_del_attr(struct xdr *xs, char *point);

extern int xdr_nfs_read_file(struct xdr *xs, char *point);

extern int xdr_nfs_lookup(struct xdr *xs, char *point);

extern int xdr_nfs_write_file(struct xdr *xs, char *point);

extern int xdr_nfs_create(struct xdr *xs, char *point);

extern int xdr_nfs_delete(struct xdr *xs, char *point);

#endif /* XDR_NFS_H_ */

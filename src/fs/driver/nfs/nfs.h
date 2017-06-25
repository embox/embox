/**
 * @file
 * @brief
 *
 * @date 14.05.2012
 * @author Andrey Gazukin
 */


#ifndef NFS_H_
#define NFS_H_

#include <stdint.h>
#include <fs/node.h>
#include <net/l3/ipv4/ip.h>
#include <sys/socket.h>
#include <net/lib/rpc/rpc.h>
#include <limits.h>

/*
 * RPC definitions for the portmapper
 */
#define PMAP_PROGNUM 100000
#define PMAP_VER 2
#define PMAP_PORT 111

#define	PMAPPROC_NULL		0
#define	PMAPPROC_SET		1
#define	PMAPPROC_UNSET		2
#define	PMAPPROC_GETPORT	3
#define	PMAPPROC_DUMP		4
#define PMAPPROC_CALLIT     5

/*
 * RPC definitions for the status manager NSM
 */
#define NSM_PROGNUM 100024
#define NSM_VER 1

/*
 * RPC definitions for the lock manager NLM
 */
#define NLM_PROGNUM 100021
#define NLM_VER 4

#define NLMPROC4_NULL            0
#define NLMPROC4_TEST            1
#define NLMPROC4_LOCK            2
#define NLMPROC4_CANCEL          3
#define NLMPROC4_UNLOCK          4
#define NLMPROC4_GRANTED         5
#define NLMPROC4_TEST_MSG        6
#define NLMPROC4_LOCK_MSG        7
#define NLMPROC4_CANCEL_MSG      8
#define NLMPROC4_UNLOCK_MSG      9
#define NLMPROC4_GRANTED_MSG    10
#define NLMPROC4_TEST_RES       11
#define NLMPROC4_LOCK_RES       12
#define NLMPROC4_CANCEL_RES     13
#define NLMPROC4_UNLOCK_RES     14
#define NLMPROC4_GRANTED_RES    15
#define NLMPROC4_SHARE          20
#define NLMPROC4_UNSHARE        21
#define NLMPROC4_NM_LOCK        22
#define NLMPROC4_FREE_ALL       23

/*
 * RPC definitions for the nfs deamon
 */
#define NFS_PROGNUM 100003
#define NFS_VER 3

#define NFSPROC3_NULL            0
#define NFSPROC3_GETATTR         1
#define NFSPROC3_SETATTR         2
#define NFSPROC3_LOOKUP          3
#define NFSPROC3_ACCESS          4
#define NFSPROC3_READLINK        5
#define NFSPROC3_READ            6
#define NFSPROC3_WRITE           7
#define NFSPROC3_CREATE          8
#define NFSPROC3_MKDIR           9
#define NFSPROC3_SYMLINK        10
#define NFSPROC3_MKNOD          11
#define NFSPROC3_REMOVE         12
#define NFSPROC3_RMDIR          13
#define NFSPROC3_RENAME         14
#define NFSPROC3_LINK           15
#define NFSPROC3_READDIR        16
#define NFSPROC3_READDIRPLUS    17
#define NFSPROC3_FSSTAT         18
#define NFSPROC3_FSINFO         19
#define NFSPROC3_PATHCONF       20
#define NFSPROC3_COMMIT         21

#define FILE_SYNC      2

#define UNCHECKED_MODE  0
#define GURDED_MODE     0x01
#define EXCLUSIVE_MODE  0x02

/*
 * RPC definitions for the mount deamon
 */
#define MOUNT_PROGNUM 100005
#define MOUNT_VER 3

#define MOUNTPROC3_NULL       0
#define MOUNTPROC3_MNT        1
#define MOUNTPROC3_DUMP       2
#define MOUNTPROC3_UMNT       3
#define MOUNTPROC3_UMNTALL    4
#define MOUNTPROC3_EXPORT     5

#define MNTPATHLEN   1024  /* Maximum bytes in a path name */
#define MNTNAMLEN     255  /* Maximum bytes in a name */
#define FHSIZE3        64  /* Maximum bytes in a V3 file handle */

#define	STATUS_OK		        0x00000000
#define	VALUE_FOLLOWS_YES		0x00000001
#define	NFS_EOF         		0x00000001
#define	NFS_FILE_NODE_TYPE      0x1
#define	NFS_DIRECTORY_NODE_TYPE 0x2

#define	DIRCOUNT     1024
#define	MAXDIRCOUNT  2048

#define	EMBOX_MACHNAME  "embox"

typedef struct create_params {
	const char      path[PATH_MAX];
	const char      fs_name[NAME_MAX];
	unsigned int    fs_type;
} create_params_t;

/* RPC string */
typedef struct rpc_string {
	size_t len;
	char data[NAME_MAX];
} rpc_string_t;

typedef struct rpc_fh_string {
	size_t len;
	char data[FHSIZE3];
} rpc_fh_string_t;


typedef struct nfs_filehandle {
	rpc_fh_string_t name_fh;
	__u64 cookie;
	__u64 cookieverf;
	__u32 count;
	__u32 maxcount;
} nfs_filehandle_t;

/* Body of a RPC replay to MOUNT Export command */
typedef struct export_dir {
	__u32 vf;
	size_t dir_len;
	char dir_name[PATH_MAX];
} export_dir_t;


/* Body of a RPC MOUNT service replay */
typedef struct mount_service {
	__u32 status;
	nfs_filehandle_t fh;
	__u32 flv;
} mount_service_t;

/* name of file */
typedef struct file_name {
	__u64 file_id;
	rpc_string_t name;
	__u64 cookie;
} file_name_t;

/* time of create file */
typedef struct time_sec {
	__u32 second;
	__u32 nano_sec;
} time_sec_t;

/* attribute of file, reply in READDIRPLUS command */
typedef struct file_attribute_rep {
	__u32 type;
	__u32 mode;
	__u32 nlink;
	__u32 uid;
	__u32 gid;
	__u64 size;
	__u64 used;
	__u32 specdata1;
	__u32 specdata2;
	__u64 fsid;
	__u64 file_id;
	time_sec_t atime;
	time_sec_t mtime;
	time_sec_t ctime;
} file_attribute_rep_t;

/* attribute of file, reply in READDIRPLUS command */
typedef struct file_del_attribute_rep {
	__u64 size;
	time_sec_t mtime;
	time_sec_t ctime;
} file_del_attribute_rep_t;

typedef struct dir_attribute_rep {
	file_attribute_rep_t dir_attr;
	__u64 verifier;
} dir_attribute_rep_t;

/* READDIRPLUS command reply*/
typedef struct readdir_desc {
	file_name_t file_name;
	__u32 vf_attr;
	file_attribute_rep_t file_attr;
	__u32 vf_fh;
	nfs_filehandle_t file_handle;
} readdir_desc_t;

/* LOOKUP file request*/
typedef struct lookup_req {
	rpc_fh_string_t *dir_fh;
	rpc_string_t *fname;
}lookup_req_t;

/* LOOKUP file reply*/
typedef struct lookup_reply {
	__u32 status;
	rpc_fh_string_t *fh;
	__u32 obj_vf;
	file_attribute_rep_t attr;
	__u32 dir_vf;
	file_attribute_rep_t dir_attr;
} lookup_reply_t;

/* create file/dir request*/
typedef struct create_req {
	lookup_req_t new;
	__u32 type;
	__u32 create_mode;
	__u32 mode_vf;
	__u32 mode;
	__u32 uid_vf;
	__u32 uid;
	__u32 gid_vf;
	__u32 gid;
	__u32 size_vf;
	__u64 size;
	__u32 set_atime;
	__u32 set_mtime;
} create_req_t;

typedef struct create_reply {
	__u32 status;
	__u32 fh_vf;
	rpc_fh_string_t obj_fh;
	__u32 attr_vf;
	file_attribute_rep_t obj_attr;
}create_reply_t;

/* DELETE file reply*/
typedef struct delete_reply {
	__u32 status;
	__u32 before_vf;
	file_del_attribute_rep_t before_attr;
	__u32 dir_vf;
	file_attribute_rep_t *dir_attr;
} delete_reply_t;

/* READ file request*/
typedef struct read_req {
	rpc_fh_string_t *fh;
	__u64 offset;
	__u32 count;
} read_req_t;

/* READ file reply*/
typedef struct read_reply {
	__u32 status;
	__u32 vf;
	file_attribute_rep_t attr;
	__u32 count;
	__u32 eof;
	__u32 datalen;
	char *data;
} read_reply_t;

/* WRITE file request*/
typedef struct write_req {
	rpc_fh_string_t *fh;
	__u64 offset;
	__u32 count;
	__u32 stable;
	__u32 datalen;
	char *data;
} write_req_t;

/* WRITE file reply*/
typedef struct write_reply {
	__u32 status;
	__u32 before_vf;
	file_attribute_rep_t before_attr;
	__u32 vf;
	file_attribute_rep_t *attr;
	__u32 count;
	__u32 comitted;
	__u64 cookie_vrf;
} write_reply_t;

typedef struct nfs_fs_info {
	char srv_name[PATH_MAX];
	char srv_dir[PATH_MAX];
	nfs_filehandle_t fh;
	struct client *mnt;
	struct client *nfs;
} nfs_fs_info_t;

typedef struct nfs_file_info {
	file_name_t name_dsc;
	file_attribute_rep_t attr;
	nfs_filehandle_t fh;
	int mode;				/* mode in which this file was opened */
	__u64 offset;			/* current (BYTE) pointer */
} nfs_file_info_t;

#endif /* NFS_H_ */

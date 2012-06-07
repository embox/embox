/**
 * @file nfs.h
 * @brief
 *
 * @date 14.05.2012
 * @author Andrey Gazukin
 */


#ifndef NFS_H_
#define NFS_H_

#include <stdint.h>
#include <fs/fat.h>
#include <fs/ramdisk.h>
#include <net/ip.h>
#include <net/socket.h>
#include <net/rpc/rpc.h>

#define MAX_FILE_QUANTITY  64


/*
 *    "rpcinfo -p" example:
 *    прог-ма верс прото   порт
 *    100000    2   tcp    111  portmapper
 *    100000    2   udp    111  portmapper
 *    100024    1   udp  44725  status
 *    100024    1   tcp  40865  status
 *    100021    1   udp  60786  nlockmgr
 *    100021    3   udp  60786  nlockmgr
 *    100021    4   udp  60786  nlockmgr
 *    100021    1   tcp  41219  nlockmgr
 *    100021    3   tcp  41219  nlockmgr
 *    100021    4   tcp  41219  nlockmgr
 *    100003    2   udp   2049  nfs
 *    100003    3   udp   2049  nfs
 *    100003    4   udp   2049  nfs
 *    100003    2   tcp   2049  nfs
 *    100003    3   tcp   2049  nfs
 *    100003    4   tcp   2049  nfs
 *    100005    1   udp  35933  mountd
 *    100005    1   tcp  56887  mountd
 *    100005    2   udp  35933  mountd
 *    100005    2   tcp  56887  mountd
 *    100005    3   udp  35933  mountd
 *    100005    3   tcp  56887  mountd
 */

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

#define	VALUE_FOLLOWS_YES		0x00000001
#define	STATUS_OK		        0x00000000
#define	DIRCOUNT     2048
#define	MAXDIRCOUNT  4064

#define AUX_UNIX        0x00000001
#define	EMBOX_MACHNAME  "embox"
#define	EMBOX_NAMELEN   sizeof(EMBOX_MACHNAME) - 1
#define	EMBOX_STAMP     0x00fd15b7

typedef struct nfs_filehandle {
	uint32_t len;
	char name[64];
	uint64_t cookie;
	uint64_t cookieverf;
	uint32_t count;
	uint32_t maxcount;
} nfs_filehandle_t;

/* RPC string */
typedef struct rpc_string {
	char data[CONFIG_MAX_LENGTH_PATH_NAME];
	size_t len;
} rpc_string_t;

/* Body of a RPC replay to MOUNT Export command */
typedef struct export_dir {
	__u32 follow;
	rpc_string_t dir;
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

/* READDIRPLUS command reply*/
typedef struct readdir_reply {
	__u32 vf_name;
	file_name_t file_name;
	__u32 vf_attr;
	file_attribute_rep_t file_attr;
	__u32 vf_fh;
	nfs_filehandle_t file_handle;
} readdir_reply_t;

typedef struct nfs_fs_description {
	char srv_name[CONFIG_MAX_LENGTH_PATH_NAME];
	char srv_dir[CONFIG_MAX_LENGTH_PATH_NAME];
	export_dir_t export;
	nfs_filehandle_t fh;
	//nfs_crdt_unix_t auth_head;
	char mnt_point[CONFIG_MAX_LENGTH_PATH_NAME];
	struct client *mnt;
	struct client *nfs;
} nfs_fs_description_t;

typedef struct nfs_file_description {
	file_name_t name;
	file_attribute_rep_t attr;
	nfs_filehandle_t fh;
	nfs_fs_description_t *p_fs_dsc;
} nfs_file_description_t;

#endif /* NFS_H_ */

/**
 * @file
 * @brief
 *
 * @date 17.05.2012
 * @author Andrey Gazukin
 */

#include <cmd/mount.h>

#ifndef NFS_CLIENTSTATE_H_
#define NFS_CLIENTSTATE_H_

int mount_nfs_filesystem(void *par);

int nfs_getports(void);

int nfs_mount(void);

int nfs_statfs(void);



#endif /* NFS_CLIENTSTATE_H_ */

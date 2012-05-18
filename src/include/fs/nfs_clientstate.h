/**
 * @file
 * @brief
 *
 * @date 17.05.2012
 * @author Andrey Gazukin
 */


#ifndef NFS_CLIENTSTATE_H_
#define NFS_CLIENTSTATE_H_

int Mount_NFS_Filesystem(char *dev);

int NFS_Getport(char *dev);

int NFS_Authentication(char *dev);

int NFS_Mount(char *dev);

int NFS_StatFS(char *dev);



#endif /* NFS_CLIENTSTATE_H_ */

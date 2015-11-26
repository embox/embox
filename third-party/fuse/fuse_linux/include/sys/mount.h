/**
 * @file
 *
 * @data 26.11.2015
 * @author: Anton Bondarev
 */

#ifndef THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_SYS_MOUNT_H_
#define THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_SYS_MOUNT_H_


int mount(const char *source, const char *target,
                 const char *filesystemtype, unsigned long mountflags,
                 const void *data);

#define MS_RDONLY       0x1
#define MS_NOSUID       0x2
#define MS_NODEV        0x3
#define MS_NOEXEC       0x4
#define MS_SYNCHRONOUS  0x5
#define MS_NOATIME      0x6



#endif /* THIRD_PARTY_FUSE_FUSE_LINUX_INCLUDE_SYS_MOUNT_H_ */

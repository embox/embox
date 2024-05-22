/**
 * @file
 * @brief Filesystem types
 *
 * @date 09.11.15
 * @author: Anton Bondarev
 */

#ifndef SRC_COMPAT_LINUX_INCLUDE_LINUX_LINUX_MAGIC_H_
#define SRC_COMPAT_LINUX_INCLUDE_LINUX_LINUX_MAGIC_H_

/* Most of these MAGIC constants are defined in /usr/include/linux/magic.h,
 *  and some are hardcoded in kernel sources.
 */

#define ADFS_SUPER_MAGIC      0xadf5
#define AFFS_SUPER_MAGIC      0xADFF
#define BDEVFS_MAGIC          0x62646576
#define BEFS_SUPER_MAGIC      0x42465331
#define BFS_MAGIC             0x1BADFACE
#define BINFMTFS_MAGIC        0x42494e4d
#define BTRFS_SUPER_MAGIC     0x9123683E
#define CGROUP_SUPER_MAGIC    0x27e0eb
#define CIFS_MAGIC_NUMBER     0xFF534D42
#define CODA_SUPER_MAGIC      0x73757245
#define COH_SUPER_MAGIC       0x012FF7B7
#define CRAMFS_MAGIC          0x28cd3d45
#define DEBUGFS_MAGIC         0x64626720
#define DEVFS_SUPER_MAGIC     0x1373
#define DEVPTS_SUPER_MAGIC    0x1cd1
#define EFIVARFS_MAGIC        0xde5e81e4
#define EFS_SUPER_MAGIC       0x00414A53
#define EXT_SUPER_MAGIC       0x137D
#define EXT2_OLD_SUPER_MAGIC  0xEF51
#define EXT2_SUPER_MAGIC      0xEF53
#define EXT3_SUPER_MAGIC      0xEF53
#define EXT4_SUPER_MAGIC      0xEF53
#define FUSE_SUPER_MAGIC      0x65735546
#define FUTEXFS_SUPER_MAGIC   0xBAD1DEA
#define HFS_SUPER_MAGIC       0x4244
#define HOSTFS_SUPER_MAGIC    0x00c0ffee
#define HPFS_SUPER_MAGIC      0xF995E849
#define HUGETLBFS_MAGIC       0x958458f6
#define ISOFS_SUPER_MAGIC     0x9660
#define JFFS2_SUPER_MAGIC     0x72b6
#define JFS_SUPER_MAGIC       0x3153464a
#define MINIX_SUPER_MAGIC     0x137F /* orig. minix */
#define MINIX_SUPER_MAGIC2    0x138F /* 30 char minix */
#define MINIX2_SUPER_MAGIC    0x2468 /* minix V2 */
#define MINIX2_SUPER_MAGIC2   0x2478 /* minix V2, 30 char names */
#define MINIX3_SUPER_MAGIC    0x4d5a /* minix V3 fs, 60 char names */
#define MQUEUE_MAGIC          0x19800202
#define MSDOS_SUPER_MAGIC     0x4d44
#define NCP_SUPER_MAGIC       0x564c
#define NFS_SUPER_MAGIC       0x6969
#define NILFS_SUPER_MAGIC     0x3434
#define NTFS_SB_MAGIC         0x5346544e
#define OCFS2_SUPER_MAGIC     0x7461636f
#define OPENPROM_SUPER_MAGIC  0x9fa1
#define PIPEFS_MAGIC          0x50495045
#define PROC_SUPER_MAGIC      0x9fa0
#define PSTOREFS_MAGIC        0x6165676C
#define QNX4_SUPER_MAGIC      0x002f
#define QNX6_SUPER_MAGIC      0x68191122
#define RAMFS_MAGIC           0x858458f6
#define REISERFS_SUPER_MAGIC  0x52654973
#define ROMFS_MAGIC           0x7275
#define SELINUX_MAGIC         0xf97cff8c
#define SMACK_MAGIC           0x43415d53
#define SMB_SUPER_MAGIC       0x517B
#define SOCKFS_MAGIC          0x534F434B
#define SQUASHFS_MAGIC        0x73717368
#define SYSFS_MAGIC           0x62656572
#define SYSV2_SUPER_MAGIC     0x012FF7B6
#define SYSV4_SUPER_MAGIC     0x012FF7B5
#define TMPFS_MAGIC           0x01021994
#define UDF_SUPER_MAGIC       0x15013346
#define UFS_MAGIC             0x00011954
#define USBDEVICE_SUPER_MAGIC 0x9fa2
#define V9FS_MAGIC            0x01021997
#define VXFS_SUPER_MAGIC      0xa501FCF5
#define XENFS_SUPER_MAGIC     0xabba1974
#define XENIX_SUPER_MAGIC     0x012FF7B4
#define XFS_SUPER_MAGIC       0x58465342
#define _XIAFS_SUPER_MAGIC    0x012FD16D


#endif /* SRC_COMPAT_LINUX_INCLUDE_LINUX_MAGIC_H_ */

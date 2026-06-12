/**
 * @file
 * @brief mman - XSI shared memory facility
 *
 * @author Aleksey Zhmulin
 * @date 28.02.26
 */

#ifndef COMPAT_POSIX_SYS_SHM_H_
#define COMPAT_POSIX_SYS_SHM_H_

#include <sys/cdefs.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>

/* Values of the 'shmflg' arg of shmat() */
#define SHM_RDONLY 0x01 /* Attach read-only (else read-write) */
#define SHM_RND    0x02 /* Round attach address to SHMLBA */

/* Segment low boundary address multiple */
#define SHMLBA 0x0

#define SHM_R       0x03 /* or S_IRUGO */
#define SHM_W       0x04 /* or S_IWUGO */
#define SHM_HUGETLB 0x05 /* segment will use huge TLB pages */

#define SHMLBA (__getpagesize())

/* Permission flag for shmget.  */
#define SHM_R 0400 /* or S_IRUGO from <linux/stat.h> */
#define SHM_W 0200 /* or S_IWUGO from <linux/stat.h> */

/* Flags for `shmat'.  */
#define SHM_RDONLY 010000  /* attach read-only else read-write */
#define SHM_RND    020000  /* round attach address to SHMLBA */
#define SHM_REMAP  040000  /* take-over region on attach */
#define SHM_EXEC   0100000 /* execution access */

/* Commands for `shmctl'.  */
#define SHM_LOCK   11 /* lock segment (root only) */
#define SHM_UNLOCK 12 /* unlock segment (root only) */

#ifdef CONFIG_SHM_SHMLBA
#define SHMLBA CONFIG_SHM_SHMLBA
#else
#endif

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

/* Unsigned integer used for the number of current attaches that must be
 * able to store values at least as large as a type unsigned short.
 */

typedef unsigned short shmatt_t;

struct shmid_ds {
	struct ipc_perm shm_perm; /* Operation permission structure */
	size_t shm_segsz;         /* Size of segment in bytes */
	pid_t shm_lpid;           /* Process ID of last shared memory operation */
	pid_t shm_cpid;           /* Process ID of creator */
	shmatt_t shm_nattch;      /* Number of current attaches */
	time_t shm_atime;         /* Time of last shmat() */
	time_t shm_dtime;         /* Time of last shmdt() */
	time_t shm_ctime;         /* Time of last change by shmctl() */
};

__BEGIN_DECLS

void *shmat(int shmid, const void *shmaddr, int shmflg);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
int shmdt(const void *shmaddr);
int shmget(key_t key, size_t size, int shmflg);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_SHM_H_ */

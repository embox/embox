/**
 * @file
 * @brief
 *
 * @date 20.08.2012
 * @author Andrey Gazukin
 */

//
// cdfs.c
//
// ISO-9660 CD-ROM Filesystem
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

#include <fs/iso9660.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <types.h>
//#include <fs/fs_drv.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <embox/block_dev.h>
#include <mem/kmalloc.h>
#include <net/in.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>

#define CDFS_DEFAULT_CACHESIZE 128
#define CDFS_BLOCKSIZE         2048

#define PS1                     '/'     // Primary path separator
#define PS2                     '\\'    // Alternate path separator

#define NODEV (-1)

#define S_IFMT         0170000   /* File type mask */
#define S_IFPKT        0160000   /* Packet device */
#define S_IFSOCK       0140000   /* Socket */
#define S_IFLNK        0120000   /* Symbolic link */
#define S_IFREG        0100000   /* Regular file */
#define S_IFBLK        0060000   /* Block device */
#define S_IFDIR        0040000   /* Directory */
#define S_IFCHR        0020000   /* Character device */
#define S_IFIFO        0010000   /* Pipe */

#define S_IREAD        0000400   /* Read permission, owner */
#define S_IWRITE       0000200   /* Write permission, owner */
#define S_IEXEC        0000100   /* Execute/search permission, owner */

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)
#define S_ISPKT(m)      (((m) & S_IFMT) == S_IFPKT)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#define F_MODIFIED     0x100000  /* File has been modified since it was opened */
#define F_DIR          0x200000  /* File is a directory */
#define F_CLOSED       0x400000  /* File is closed */

typedef unsigned short wchar_t;

#define __int64 long long
typedef __int64 off64_t;

struct file {
  //struct ioobject iob;

  struct fs *fs;
  int flags;
  int mode;
  uid_t owner;
  gid_t group;
  off64_t pos;
  void *data;
  char *path;
  char chbuf;
};

struct cdfs {
  dev_t devno;
  int blks;
  int volblks;
  int vdblk;
  int joliet;
  struct bufpool *cache;
  unsigned char *path_table_buffer;
  struct iso_pathtable_record **path_table;
  int path_table_records;
};

struct cdfs_file {
  int extent;
  int size;
  time_t date;
};

#define VFS_LOCK_TIMEOUT        60000     // Timeout for file system locks

#define F_MODIFIED              0x100000  // File has been modified since it was opened
#define F_DIR                   0x200000  // File is a directory
#define F_CLOSED                0x400000  // File is closed

#define FSOP_MKFS       0x00000001
#define FSOP_MOUNT      0x00000002
#define FSOP_UMOUNT     0x00000004
#define FSOP_STATFS     0x00000008
#define FSOP_OPEN       0x00000010
#define FSOP_CLOSE      0x00000020
#define FSOP_FSYNC      0x00000040
#define FSOP_READ       0x00000080
#define FSOP_WRITE      0x00000100
#define FSOP_IOCTL      0x00000200
#define FSOP_TELL       0x00000400
#define FSOP_LSEEK      0x00000800
#define FSOP_FTRUNCATE  0x00001000
#define FSOP_FUTIME     0x00002000
#define FSOP_UTIME      0x00004000
#define FSOP_FSTAT      0x00008000
#define FSOP_STAT       0x00010000
#define FSOP_ACCESS     0x00020000
#define FSOP_FCHMOD     0x00040000
#define FSOP_CHMOD      0x00080000
#define FSOP_FCHOWN     0x00100000
#define FSOP_CHOWN      0x00200000
#define FSOP_MKDIR      0x00400000
#define FSOP_RMDIR      0x00800000
#define FSOP_RENAME     0x01000000
#define FSOP_LINK       0x02000000
#define FSOP_UNLINK     0x04000000
#define FSOP_OPENDIR    0x08000000
#define FSOP_READDIR    0x10000000

struct filesystem {
  char *name;
  struct fsops *ops;
  struct filesystem *next;
};

struct fs {
  int locks;
  //struct mutex exclusive;
  char mntfrom[MAX_LENGTH_PATH_NAME];
  char mntto[MAX_LENGTH_PATH_NAME];
  struct fsops *ops;
  struct fs *next;
  struct fs *prev;
  mode_t mode;
  uid_t uid;
  gid_t gid;
  void *data;
  struct filesystem *fsys;
};

struct buf;

struct buflist {
  struct buf *head;
  struct buf *tail;
};

struct buflink {
  struct buf *next;
  struct buf *prev;
};

struct buf {
  struct buflink bucket;
  struct buflink chain;
  unsigned short state;
  unsigned short locks;
  //struct thread *waiters;
  blkno_t blkno;
  char *data;
};

struct _file {
  //struct ioobject iob;

  struct fs *fs;
  int flags;
  int mode;
  uid_t owner;
  gid_t group;
  off64_t pos;
  void *data;
  char *path;
  char chbuf;
};

struct direntry {
  ino_t ino;
  unsigned int reclen;
  unsigned int namelen;
  char name[MAX_LENGTH_PATH_NAME];
};

struct statfs  {
  unsigned int bsize;        // Fundamental file system block size
  unsigned int iosize;       // Optimal transfer block size
  unsigned int blocks;       // Total data blocks in file system
  unsigned int bfree;        // Free blocks in fs
  unsigned int files;        // Total file nodes in file system
  unsigned int ffree;        // Free file nodes in fs
  unsigned int cachesize;    // Cache buffers
  char fstype[MAX_LENGTH_FILE_NAME];   // File system type name
  char mntto[MAX_LENGTH_PATH_NAME];       // Directory on which mounted
  char mntfrom[MAX_LENGTH_PATH_NAME];     // Mounted file system
};

struct fsops {
  unsigned long reentrant;

  int (*lockfs)(struct fs *fs);
  void (*unlockfs)(struct fs *fs);

  int (*mkfs)(char *devname, char *opts);
  int (*mount)(struct fs *fs, char *opts);
  int (*umount)(struct fs *fs);

  int (*statfs)(struct fs *fs, struct statfs *buf);

  int (*open)(struct file *filp, char *name);
  int (*close)(struct file *filp);
  int (*destroy)(struct file *filp);
  int (*fsync)(struct file *filp);

  int (*read)(struct file *filp, void *data, size_t size, off64_t pos);
  int (*write)(struct file *filp, void *data, size_t size, off64_t pos);
  int (*ioctl)(struct file *filp, int cmd, void *data, size_t size);

  off64_t (*tell)(struct file *filp);
  off64_t (*lseek)(struct file *filp, off64_t offset, int origin);
  int (*ftruncate)(struct file *filp, off64_t size);

  //int (*futime)(struct file *filp, struct utimbuf *times);
  //int (*utime)(struct fs *fs, char *name, struct utimbuf *times);

  //int (*fstat)(struct file *filp, struct stat64 *buffer);
  //int (*stat)(struct fs *fs, char *name, struct stat64 *buffer);

  int (*access)(struct fs *fs, char *name, int mode);

  int (*fchmod)(struct file *filp, int mode);
  int (*chmod)(struct fs *fs, char *name, int mode);
  int (*fchown)(struct file *filp, int owner, int group);
  int (*chown)(struct fs *fs, char *name, int owner, int group);

  int (*mkdir)(struct fs *fs, char *name, int mode);
  int (*rmdir)(struct fs *fs, char *name);

  int (*rename)(struct fs *fs, char *oldname, char *newname);
  int (*link)(struct fs *fs, char *oldname, char *newname);
  int (*unlink)(struct fs *fs, char *name);

  int (*opendir)(struct file *filp, char *name);
  int (*readdir)(struct file *filp, struct direntry *dirp, int count);
};
/*
struct tm {
  int tm_sec;                   // Seconds after the minute [0, 59]
  int tm_min;                   // Minutes after the hour [0, 59]
  int tm_hour;                  // Hours since midnight [0, 23]
  int tm_mday;                  // Day of the month [1, 31]
  int tm_mon;                   // Months since January [0, 11]
  int tm_year;                  // Years since 1900
  int tm_wday;                  // Days since Sunday [0, 6]
  int tm_yday;                  // Days since January 1 [0, 365]
  int tm_isdst;                 // Daylight Saving Time flag
  int tm_gmtoff;                // Seconds east of UTC
  char *tm_zone;                // Timezone abbreviation
};
*/
static int cdfs_fnmatch(struct cdfs *cdfs, char *fn1, int len1, char *fn2, int len2) {
  if (cdfs->joliet) {
    wchar_t *wfn2 = (wchar_t *) fn2;
    int wlen2 = len2 / 2;
    if (wlen2 > 1 && ntohs(wfn2[wlen2 - 2]) == ';') wlen2 -= 2;
    if (wlen2 > 0 && ntohs(wfn2[wlen2 - 1]) == '.') wlen2 -= 1;
    if (len1 != wlen2) return 0;
    while (len1--) if (*fn1++ != ntohs(*wfn2++)) return 0;
    return 1;
  } else {
    if (len2 > 1 && fn2[len2 - 2] == ';') len2 -= 2;
    if (len2 > 0 && fn2[len2 - 1] == '.') len2 -= 1;
    if (len1 != len2) return 0;
    while (len1--) if (*fn1++ != *fn2++) return 0;
    return 1;
  }
}

static int cdfs_read_path_table(struct cdfs *cdfs, struct iso_volume_descriptor *vd) {
  struct buf *buf;
  unsigned char *pt;
  int ptblk;
  int ptlen;
  int ptpos;
  int n;

  // Determine size and location of path table and allocate buffer
  ptlen = isonum_733(vd->path_table_size);
  ptblk = isonum_731(vd->type_l_path_table);
  cdfs->path_table_buffer = malloc(ptlen);
  if (!cdfs->path_table_buffer) return -ENOMEM;

  // Read L path table into buffer
  ptpos = 0;
  while (ptpos < ptlen) {
    buf = get_buffer(cdfs->cache, ptblk++);
    if (!buf) return -EIO;

    if (ptlen - ptpos > CDFS_BLOCKSIZE) {
      memcpy(cdfs->path_table_buffer + ptpos, buf->data, CDFS_BLOCKSIZE);
      ptpos += CDFS_BLOCKSIZE;
    } else {
      memcpy(cdfs->path_table_buffer + ptpos, buf->data, ptlen - ptpos);
      ptpos = ptlen;
    }

    release_buffer(cdfs->cache, buf);
  }

  // Determine number of records in pathtable
  // Path table records are indexed from 1 (first entry not used)
  pt = cdfs->path_table_buffer;
  n = 1;
  while (pt < cdfs->path_table_buffer + ptlen) {
    struct iso_pathtable_record *pathrec = (struct iso_pathtable_record *) pt;
    int namelen = pathrec->length;
    int reclen = sizeof(struct iso_pathtable_record) + namelen + (namelen & 1);

    n++;
    pt += reclen;
  }

  cdfs->path_table_records = n;

  // Allocate path table
  cdfs->path_table = (struct iso_pathtable_record **) kmalloc(cdfs->path_table_records * sizeof(struct iso_pathtable_record **));
  if (!cdfs->path_table) return -ENOMEM;
  cdfs->path_table[0] = NULL;

  // Setup pointers into path table buffer
  pt = cdfs->path_table_buffer;
  for (n = 1; n < cdfs->path_table_records; n++) {
    struct iso_pathtable_record *pathrec = (struct iso_pathtable_record *) pt;
    int namelen = pathrec->length;
    int reclen = sizeof(struct iso_pathtable_record) + namelen + (namelen & 1);

    cdfs->path_table[n] = pathrec;
    pt += reclen;
  }

  return 0;
}

static int cdfs_find_dir(struct cdfs *cdfs, char *name, int len) {
  char *p;
  int l;
  int dir = 2;
  int parent = 1;

  while (1) {
    // Skip path separator
    if (*name == PS1 || *name == PS2) {
      name++;
      len--;
    }
    if (len == 0) return parent;

    // Find next part of name
    p = name;
    l = 0;
    while (l < len && *p != PS1 && *p != PS2) {
      l++;
      p++;
    }

    // Find directory for next name part
    while (dir < cdfs->path_table_records) {
      if (cdfs->path_table[dir]->parent != parent) return -ENOENT;
      if (cdfs_fnmatch(cdfs, name, l, cdfs->path_table[dir]->name, cdfs->path_table[dir]->length)) break;
      dir++;
    }

    // If we reached the end of the path table the directory does not exist
    if (dir == cdfs->path_table_records) return -ENOENT;

    // If we have parsed the whole name return the directory number
    if (l == len) return dir;

    // Go forward in path table until first entry for directory found
    parent = dir;
    while (dir < cdfs->path_table_records) {
      if (cdfs->path_table[dir]->parent == parent) break;
      dir++;
    }

    // Prepare for next name part
    name = p;
    len -= l;
  }
}

static int cdfs_find_in_dir(struct cdfs *cdfs, int dir, char *name, int len, struct buf **dirbuf, struct iso_directory_record **dirrec) {
  struct buf *buf;
  char *p;
  struct iso_directory_record *rec;
  int blk;
  int left;
  int reclen;
  int namelen;

  // The first two directory records are . (current) and .. (parent)
  blk = cdfs->path_table[dir]->extent;
  buf = get_buffer(cdfs->cache, blk++);
  if (!buf) return -EIO;

  // Get length of directory from the first record
  p = buf->data;
  rec = (struct iso_directory_record *) p;
  left = isonum_733(rec->size);

  // Find named entry in directory
  while (left > 0) {
    // Read next block if all records in current block has been read
    // Directory records never cross block boundaries
    if (p >= buf->data + CDFS_BLOCKSIZE) {
      release_buffer(cdfs->cache, buf);
      if (p > buf->data + CDFS_BLOCKSIZE) return -EIO;
      buf = get_buffer(cdfs->cache, blk++);
      if (!buf) return -EIO;
      p = buf->data;
    }

    // Check for match
    rec = (struct iso_directory_record *) p;
    reclen = isonum_711(rec->length);
    namelen = isonum_711(rec->name_len);

    if (reclen > 0) {
      if (cdfs_fnmatch(cdfs, name, len, (char *) rec->name, namelen)) {
        *dirrec = rec;
        *dirbuf = buf;
        return 0;
      }

      // Skip to next record
      p += reclen;
      left -= reclen;
    } else {
      // Skip to next block
      left -= (buf->data + CDFS_BLOCKSIZE) - p;
      p = buf->data + CDFS_BLOCKSIZE;
    }
  }

  release_buffer(cdfs->cache, buf);
  return -ENOENT;
}

static int cdfs_find_file(struct cdfs *cdfs, char *name, int len, struct buf **buf, struct iso_directory_record **rec) {
  int dir;
  int split;
  int n;

  // If root get directory record from volume descriptor
  if (len == 0) {
    struct iso_volume_descriptor *vd;

    *buf = get_buffer(cdfs->cache, cdfs->vdblk);
    if (!*buf) return -EIO;
    vd = (struct iso_volume_descriptor *) (*buf)->data;
    *rec = (struct iso_directory_record *) vd->root_directory_record;
    return 0;
  }

  // Split path into directory part and file name part
  split = -1;
  for (n = 0; n < len; n++) if (name[n] == PS1 || name[n] == PS2) split = n;

  // Find directly if file located in root directory
  if (split == -1) return cdfs_find_in_dir(cdfs, 1, name, len, buf, rec);

  // Locate directory
  dir = cdfs_find_dir(cdfs, name, split + 1);
  if (dir < 0) return dir;

  // Find filename in directory
  return cdfs_find_in_dir(cdfs, dir, name + split + 1, len - split - 1, buf, rec);
}

static time_t cdfs_isodate(unsigned char *date)
{
  struct tm tm;

  memset(&tm, 0, sizeof tm);
  tm.tm_year = date[0];
  tm.tm_mon = date[1] - 1;
  tm.tm_mday = date[2];
  tm.tm_hour = date[3];
  tm.tm_min = date[4];
  tm.tm_sec = date[5];
  tm.tm_min += (*(char *) &date[6]) * 15;

  return &tm; //mktime(&tm);
}

int cdfs_mount(struct fs *fs, char *opts)
{
  struct cdfs *cdfs;
  dev_t devno;
  int cachebufs;
  int rc;
  int blk;
  struct buf *buf;
  struct iso_volume_descriptor *vd;

  // Check device
  devno = dev_open(fs->mntfrom);
  if (devno == NODEV) return -NODEV;
  //if (device(devno)->driver->type != DEV_TYPE_BLOCK) return -ENOTBLK;

  // Revalidate device and check block size
  if (get_option(opts, "revalidate", NULL, 0, NULL)) {
    rc = dev_ioctl(devno, IOCTL_REVALIDATE, NULL, 0);
    if (rc < 0) return rc;
  }

  if (dev_ioctl(devno, IOCTL_GETBLKSIZE, NULL, 0) != CDFS_BLOCKSIZE) return -ENXIO;

  // Allocate file system
  cdfs = (struct cdfs *) kmalloc(sizeof(struct cdfs));
  memset(cdfs, 0, sizeof(struct cdfs));
  cdfs->devno = devno;
  cdfs->blks = dev_ioctl(devno, IOCTL_GETDEVSIZE, NULL, 0);
  if (cdfs->blks < 0) return cdfs->blks;

  // Allocate cache
  cachebufs = get_num_option(opts, "cache", CDFS_DEFAULT_CACHESIZE);
  cdfs->cache = init_buffer_pool(devno, cachebufs, CDFS_BLOCKSIZE, NULL, cdfs);
  if (!cdfs->cache) return -ENOMEM;

  // Read volume descriptors
  cdfs->vdblk = 0;
  blk = 16;
  while (1) {
    int type;
    unsigned char *esc;

    buf  = get_buffer(cdfs->cache, blk);
    if (!buf) return -EIO;
    vd = (struct iso_volume_descriptor *) buf->data;

    type = isonum_711(vd->type);
    esc = vd->escape_sequences;

    if (memcmp(vd->id, "CD001", 5) != 0) {
      free_buffer_pool(cdfs->cache);
      dev_close(cdfs->devno);
      kfree(cdfs);
      return -EIO;
    }

    if (cdfs->vdblk == 0 && type == ISO_VD_PRIMARY) {
      cdfs->vdblk = blk;
    } else if (type == ISO_VD_SUPPLEMENTAL &&
               esc[0] == 0x25 && esc[1] == 0x2F &&
               (esc[2] == 0x40 || esc[2] == 0x43 || esc[2] == 0x45)) {
      cdfs->vdblk = blk;
      cdfs->joliet = 1;
    }

    release_buffer(cdfs->cache, buf);
    if (type == ISO_VD_END) break;
    blk++;
  }
  if (cdfs->vdblk == 0) return -EIO;

  // Initialize filesystem from selected volume descriptor and read path table
  buf  = get_buffer(cdfs->cache, cdfs->vdblk);
  if (!buf) return -EIO;
  vd = (struct iso_volume_descriptor *) buf->data;

  cdfs->volblks = isonum_733(vd->volume_space_size);

  rc = cdfs_read_path_table(cdfs, vd);
  if (rc < 0) return rc;

  release_buffer(cdfs->cache, buf);

  // Device mounted successfully
  fs->data = cdfs;
  return 0;
}

int cdfs_umount(struct fs *fs) {
  struct cdfs *cdfs = (struct cdfs *) fs->data;

  // Free cache
  if (cdfs->cache) free_buffer_pool(cdfs->cache);

  // Close device
  dev_close(cdfs->devno);

  // Deallocate file system
  if (cdfs->path_table_buffer) kfree(cdfs->path_table_buffer);
  if (cdfs->path_table) kfree(cdfs->path_table);
  kfree(cdfs);

  return 0;
}

int cdfs_statfs(struct fs *fs, struct statfs *buf) {
  struct cdfs *cdfs = (struct cdfs *) fs->data;

  buf->bsize = CDFS_BLOCKSIZE;
  buf->iosize = CDFS_BLOCKSIZE;
  buf->blocks = cdfs->volblks;
  buf->bfree = 0;
  buf->files = -1;
  buf->ffree = 0;
  //buf->cachesize = cdfs->cache->poolsize * CDFS_BLOCKSIZE;

  return 0;
}

int cdfs_open(struct file *filp, char *name) {
  struct cdfs *cdfs = (struct cdfs *) filp->fs->data;
  struct iso_directory_record *rec;
  struct cdfs_file *cdfile;
  struct buf *buf;
  time_t date;
  int size;
  int extent;
  int flags;
  int rc;

  // Check open mode
  if (filp->flags & (O_CREAT | O_TRUNC | O_APPEND)) return -EROFS;

  // Locate file in file system
  rc = cdfs_find_file(cdfs, name, strlen(name), &buf, &rec);
  if (rc < 0) return rc;

  flags = isonum_711(rec->flags);
  extent = isonum_733(rec->extent);
  date = cdfs_isodate(rec->date);
  size = isonum_733(rec->size);
  release_buffer(cdfs->cache, buf);

  // Allocate and initialize file block
  cdfile = (struct cdfs_file *) kmalloc(sizeof(struct cdfs_file));
  if (!cdfile) return -ENOMEM;
  cdfile->extent = extent;
  cdfile->date = date;
  cdfile->size = size;
  if (flags & 2) filp->flags |= F_DIR;

  filp->data = cdfile;
  filp->mode = S_IFREG | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;;
  return 0;
}

int cdfs_close(struct file *filp) {
  struct cdfs_file *cdfile = (struct cdfs_file *) filp->data;

  if (cdfile) kfree(cdfile);
  return 0;
}

int cdfs_fsync(struct file *filp) {
  return 0;
}

int cdfs_read(struct file *filp, void *data, size_t size, off64_t pos) {
  struct cdfs_file *cdfile = (struct cdfs_file *) filp->data;
  struct cdfs *cdfs = (struct cdfs *) filp->fs->data;
  size_t read;
  size_t count;
  size_t left;
  char *p;
  int iblock;
  int start;
  int blk;
  struct buf *buf;

  read = 0;
  p = (char *) data;
  while (pos < cdfile->size && size > 0) {
    iblock = (int) pos / CDFS_BLOCKSIZE;
    start = (int) pos % CDFS_BLOCKSIZE;

    count = CDFS_BLOCKSIZE - start;
    if (count > size) count = size;

    left = cdfile->size - (int) pos;
    if (count > left) count = left;
    if (count <= 0) break;

    blk = cdfile->extent + iblock;

    if (filp->flags & O_DIRECT) {
      if (start != 0 || count != CDFS_BLOCKSIZE) return read;
      if (dev_read(cdfs->devno, p, count, blk, 0) != (int) count) return read;
    } else {
      buf = get_buffer(cdfs->cache, blk);
      if (!buf) return -EIO;
      memcpy(p, buf->data + start, count);
      release_buffer(cdfs->cache, buf);
    }

    pos += count;
    p += count;
    read += count;
    size -= count;
  }

  return read;
}

off64_t cdfs_tell(struct file *filp) {
  return filp->pos;
}

off64_t cdfs_lseek(struct file *filp, off64_t offset, int origin) {
  struct cdfs_file *cdfile = (struct cdfs_file *) filp->data;

  switch (origin) {
    case SEEK_END:
      offset += cdfile->size;
      break;

    case SEEK_CUR:
      offset += filp->pos;
  }

  if (offset < 0) return -EINVAL;

  filp->pos = offset;
  return offset;
}

int cdfs_fstat(struct file *filp, struct stat *buffer) {
  struct cdfs_file *cdfile = (struct cdfs_file *) filp->data;
  struct cdfs *cdfs = (struct cdfs *) filp->fs->data;

  if (buffer) {
    memset(buffer, 0, sizeof(struct stat));

    if (filp->flags & F_DIR) {
      buffer->st_mode = S_IFDIR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    } else {
      buffer->st_mode = S_IFREG | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    }

    buffer->st_ino = cdfile->extent;
    buffer->st_nlink = 1;
    buffer->st_dev = cdfs->devno;
    buffer->st_atime = buffer->st_mtime = buffer->st_ctime = cdfile->date;
    buffer->st_size = cdfile->size;
  }

  return cdfile->size;
}

int cdfs_stat(struct fs *fs, char *name, struct stat *buffer) {
  struct cdfs *cdfs = (struct cdfs *) fs->data;
  struct iso_directory_record *rec;
  struct buf *buf;
  int rc;
  int size;

  rc = cdfs_find_file(cdfs, name, strlen(name), &buf, &rec);
  if (rc < 0) return rc;

  size = isonum_733(rec->size);

  if (buffer) {
    memset(buffer, 0, sizeof(struct stat));

    if (rec->flags[0] & 2) {
      buffer->st_mode = S_IFDIR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    } else {
      buffer->st_mode = S_IFREG | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    }

    buffer->st_ino = isonum_733(rec->extent);
    buffer->st_nlink = 1;
    buffer->st_dev = cdfs->devno;
    buffer->st_atime = buffer->st_mtime = buffer->st_ctime = cdfs_isodate(rec->date);
    buffer->st_size = size;
  };

  release_buffer(cdfs->cache, buf);
  return size;
}

int cdfs_opendir(struct file *filp, char *name) {
  struct cdfs *cdfs = (struct cdfs *) filp->fs->data;
  struct iso_directory_record *rec;
  struct cdfs_file *cdfile;
  struct buf *buf;
  time_t date;
  int size;
  int extent;
  int flags;
  int rc;

  // Locate directory
  rc = cdfs_find_file(cdfs, name, strlen(name), &buf, &rec);
  if (rc < 0) return rc;

  flags = isonum_711(rec->flags);
  extent = isonum_733(rec->extent);
  date = cdfs_isodate(rec->date);
  size = isonum_733(rec->size);
  release_buffer(cdfs->cache, buf);

  if (!(flags & 2)) return -ENOTDIR;

  // Allocate and initialize file block
  cdfile = (struct cdfs_file *) kmalloc(sizeof(struct cdfs_file));
  if (!cdfile) return -ENOMEM;
  cdfile->extent = extent;
  cdfile->date = date;
  cdfile->size = size;

  filp->data = cdfile;
  filp->mode = S_IFDIR | S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
  return 0;
}

int cdfs_readdir(struct file *filp, struct direntry *dirp, int count) {
  struct cdfs_file *cdfile = (struct cdfs_file *) filp->data;
  struct cdfs *cdfs = (struct cdfs *) filp->fs->data;
  struct iso_directory_record *rec;
  struct buf *buf;
  int namelen;
  int reclen;
  char *name;
  wchar_t *wname;

blkagain:
  if (count != 1) return -EINVAL;
  if (filp->pos >= cdfile->size) return 0;

  // Get directory block
  buf = get_buffer(cdfs->cache, cdfile->extent + (int) filp->pos / CDFS_BLOCKSIZE);
  if (!buf) return -EIO;

  // Locate directory record
recagain:
  rec = (struct iso_directory_record *) (buf->data + (int) filp->pos % CDFS_BLOCKSIZE);
  reclen = isonum_711(rec->length);
  namelen = isonum_711(rec->name_len);

  // Check for no more records in block
  if (reclen == 0) {
    int blkleft = CDFS_BLOCKSIZE - ((int) filp->pos % CDFS_BLOCKSIZE);
    release_buffer(cdfs->cache, buf);
    filp->pos += blkleft;
    goto blkagain;
  }

  // Check for . and .. entries
  if (namelen == 1 && (rec->name[0] == 0 || rec->name[0] == 1)) {
    filp->pos += reclen;
    goto recagain;
  }

  // Get info from directory record
  dirp->ino = isonum_733(rec->extent);
  dirp->reclen = sizeof(struct direntry) - MAX_LENGTH_PATH_NAME + namelen + 1;
  if (cdfs->joliet) {
    int n;

    namelen /= 2;
    wname = (wchar_t *) rec->name;
    if (namelen > 1 && ntohs(wname[namelen - 2]) == ';') namelen -= 2;
    if (namelen > 0 && ntohs(wname[namelen - 1]) == '.') namelen -= 1;

    dirp->namelen = namelen;
    for (n = 0; n < namelen; n++) dirp->name[n] = (char) ntohs(wname[n]);
    dirp->name[namelen] = 0;
  } else {
    name = (char *) rec->name;
    if (namelen > 1 && name[namelen - 2] == ';') namelen -= 2;
    if (namelen > 0 && name[namelen - 1] == '.') namelen -= 1;

    dirp->namelen = namelen;
    memcpy(dirp->name, name, namelen);
    dirp->name[namelen] = 0;
  }

  filp->pos += reclen;
  release_buffer(cdfs->cache, buf);
  return 1;
}

#define FSOP_MKFS       0x00000001
#define FSOP_MOUNT      0x00000002
#define FSOP_UMOUNT     0x00000004
#define FSOP_STATFS     0x00000008
#define FSOP_OPEN       0x00000010
#define FSOP_CLOSE      0x00000020
#define FSOP_FSYNC      0x00000040
#define FSOP_READ       0x00000080
#define FSOP_WRITE      0x00000100
#define FSOP_IOCTL      0x00000200
#define FSOP_TELL       0x00000400
#define FSOP_LSEEK      0x00000800
#define FSOP_FTRUNCATE  0x00001000
#define FSOP_FUTIME     0x00002000
#define FSOP_UTIME      0x00004000
#define FSOP_FSTAT      0x00008000
#define FSOP_STAT       0x00010000
#define FSOP_ACCESS     0x00020000
#define FSOP_FCHMOD     0x00040000
#define FSOP_CHMOD      0x00080000
#define FSOP_FCHOWN     0x00100000
#define FSOP_CHOWN      0x00200000
#define FSOP_MKDIR      0x00400000
#define FSOP_RMDIR      0x00800000
#define FSOP_RENAME     0x01000000
#define FSOP_LINK       0x02000000
#define FSOP_UNLINK     0x04000000
#define FSOP_OPENDIR    0x08000000
#define FSOP_READDIR    0x10000000

struct fsops cdfsops = {
  FSOP_OPEN | FSOP_CLOSE | FSOP_FSYNC | FSOP_READ |
  FSOP_TELL | FSOP_LSEEK | FSOP_STAT | FSOP_FSTAT |
  FSOP_OPENDIR | FSOP_READDIR,

  NULL,
  NULL,

  NULL,
  cdfs_mount,
  cdfs_umount,

  cdfs_statfs,

  cdfs_open,
  cdfs_close,
  NULL,
  cdfs_fsync,

  cdfs_read,
  NULL,
  NULL,

  cdfs_tell,
  cdfs_lseek,
  NULL,

  NULL,
  NULL,

  cdfs_fstat,
  cdfs_stat,

  NULL,

  NULL,
  NULL,
  NULL,
  NULL,

  NULL,
  NULL,

  NULL,
  NULL,
  NULL,

  cdfs_opendir,
  cdfs_readdir
};

void init_cdfs() {
  //register_filesystem("cdfs", &cdfsops);
	return;
}


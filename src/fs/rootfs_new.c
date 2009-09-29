/**
 * \file rootfs.c
 * \date Jun 29, 2009
 * \author Anton Bondarev
 * \details
 */
#include "common.h"
#include "string.h"
#include "fs/rootfs_new.h"
#include "file/fpath_common.h"

extern FSOP memseg_fsop;

static MNT_FSOP const mnt_fops[] = {
	{ "/mm",    &memseg_fsop},
};

static OPENED_FILE opened_files[MAX_OPENED_FILES];

/*
 * @param out_flist - pointer to the array of FLIST_ITEMs. Caller should alloc memory for it before
 * @param max_nitems - maximum number of FLIST_ITEMs, that can be placed in the out_flist
 * @return actual number of flist_items, that can be returned (it can be more than max_nitems)
 * @return -1 on error (invalid path for ex.) */
size_t list_dir (const char *path, FLIST_ITEM* out_flist, int max_nitems)
{
	short fs_idx;

	if (out_flist == NULL) {
		printf("invalid flist array pointer!\n");
		return -1;
	}

	// if path given is empty, or '/' - list mounted filesystems' paths
	if ( strlen(path)==0 || strcmp(path,"/")==0 ) {
		for (fs_idx=0; (fs_idx<FS_LIST_SIZE)&&(fs_idx<max_nitems); fs_idx++) {
			strcpy (out_flist[fs_idx].name, mnt_fops[fs_idx].mountpath);
			out_flist[fs_idx].size = 0;
		}
		return FS_LIST_SIZE;
	}

	fs_idx = get_fs_idx_by_path(path);

	if (fs_idx == IDX_INVALID)
		return -1;

	return mnt_fops[fs_idx].fsop->list_files(out_flist, max_nitems);
}

int rootfs_init()
{
	int i;
	for (i = 0; i < FS_LIST_SIZE; i++)
		mnt_fops[i].fsop->init();

	for (i=0;i<MAX_OPENED_FILES;i++)
		RELEASE_OFILE(opened_files[i]);

	return 0;
}

static int find_free_ofile_idx()
{
	int i;
	for (i=0;i<MAX_OPENED_FILES;i++) {
		if (!IS_OFILE_USED(opened_files[i]))
			return i;
	}
	return IDX_INVALID;
}

static short get_fs_idx_by_path(const char *path)
{
	short fs_idx;

	for (fs_idx=0; fs_idx<FS_LIST_SIZE; fs_idx++)
		if ( strncmp(path, mnt_fops[fs_idx].mountpath, strlen(mnt_fops[fs_idx].mountpath)) == 0 )
			return fs_idx;

	return IDX_INVALID;
}

FDESC open (const char *file_path, int flags)
{
	TRACE("opening %s..\n", file_path);
	int ofile_idx=find_free_ofile_idx();
	if (ofile_idx == IDX_INVALID) {
		TRACE("can't open more files - maximum reached\n");
		return FDESC_INVALID;
	}
	TRACE("found free idx %d\n", ofile_idx);

	const char* filename = get_file_name(file_path);
	if (filename == NULL) {
		TRACE("can't parse file path %s\n (may be wrong format)\n", file_path);
		return FDESC_INVALID;
	}

	short fs_idx, fs_file_idx;

	if ((fs_idx=get_fs_idx_by_path(file_path)) == IDX_INVALID) {
		TRACE("can't find filesystem description for file %s\n", file_path);
		return FDESC_INVALID;
	}

	const FSOP* fsop = mnt_fops[fs_idx].fsop;

	if ((fs_file_idx=fsop->get_file_idx(filename)) == IDX_INVALID) {
		if (flags&O_CREAT) {
			if (fsop->create_file == NULL) {
				TRACE("this fs does not support creating files\n");
				return FDESC_INVALID;
			}
			fs_file_idx=fsop->create_file(filename);
		}
		else {
			TRACE("can't find file %s\n", file_path);
			return FDESC_INVALID;
		}
	}

	opened_files[ofile_idx].fs_idx = fs_idx;
	opened_files[ofile_idx].fs_file_idx = fs_file_idx;
	opened_files[ofile_idx].cur_pos = 0;
	return (FDESC)ofile_idx;
}


size_t write (FDESC file_desc, const void *buf, size_t nbytes)
{
	if (!IS_FDESC_VALID(file_desc)){
		TRACE("invalid file_desc! %d\n", file_desc);
		return -1;
	}

	OPENED_FILE* ofile = &(opened_files[file_desc]);
	if (!IS_OFILE_USED((*ofile))) {
		TRACE("invalid file_desc! %d\n", file_desc);
		return -1;
	}

	const FSOP* fsop = mnt_fops[ofile->fs_idx].fsop;

	size_t byteswritten, totalbyteswritten=0, nbytesleft;
	nbytesleft=nbytes;
	while (totalbyteswritten<nbytes) {
		byteswritten = fsop->file_write(ofile->fs_file_idx, ofile->cur_pos, buf+totalbyteswritten, nbytesleft);
		if (byteswritten<0) {
			TRACE("fs_write_error!\n");
			break;
		}
		TRACE("written: %d\n",byteswritten);
		nbytesleft -= byteswritten;
		ofile->cur_pos += byteswritten;
		totalbyteswritten += byteswritten;
	}

	return totalbyteswritten;
}

size_t read (FDESC file_desc, void *buf, size_t nbytes)
{
	if (!IS_FDESC_VALID(file_desc)){
		TRACE("invalid file_desc! %d\n", file_desc);
		return -1;
	}

	OPENED_FILE* ofile = &(opened_files[file_desc]);
	if (!IS_OFILE_USED((*ofile))) {
		TRACE("invalid file_desc! %d\n", file_desc);
		return -1;
	}

	const FSOP* fsop = mnt_fops[ofile->fs_idx].fsop;

	size_t bytesread, totalbytesread=0, nbytesleft=nbytes;
	while (totalbytesread<nbytes) {
		bytesread = fsop->file_read(ofile->fs_file_idx, ofile->cur_pos, buf+totalbytesread, nbytesleft);
		if (bytesread<0) {
			TRACE("EOF!\n");
			break;
		}
		TRACE("read: %d\n",bytesread);
		nbytesleft -= bytesread;
		ofile->cur_pos += bytesread;
		totalbytesread += bytesread;
	}

	return totalbytesread;
}

BOOL fclose (FDESC file_desc)
{
	if (!IS_FDESC_VALID(file_desc))
		return FALSE;
	OPENED_FILE* ofile = &(opened_files[file_desc]);
	if (!IS_OFILE_USED((*ofile))) {
		TRACE("invalid file_desc! %d\n", file_desc);
		return FALSE;
	}

	RELEASE_OFILE((*ofile));
	return TRUE;
}

BOOL remove (const char* file_path)
{
	const char* filename = get_file_name(file_path);
	if (filename == NULL) {
		TRACE("can't parse file path %s\n (may be wrong format)\n", file_path);
		return FDESC_INVALID;
	}

	short fs_idx;
	if ((fs_idx=get_fs_idx_by_path(file_path)) == IDX_INVALID) {
		TRACE("can't find filesystem description for file %s\n", file_path);
		return FALSE;
	}

	const FSOP* fsop = mnt_fops[fs_idx].fsop;

	return fsop->delete_file(filename);
}

int lseek(FDESC file_desc, long offset, int whence)
{
   //TODO
	return 0;
}


BOOL fsync(const char* file_path)
{
	short fs_idx;
	if ((fs_idx=get_fs_idx_by_path(file_path)) == IDX_INVALID) {
		TRACE("can't find filesystem description for file %s\n", file_path);
		return FALSE;
	}

	const FSOP* fsop = mnt_fops[fs_idx].fsop;

	return fsop->sync();
}

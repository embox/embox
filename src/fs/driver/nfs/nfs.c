/**
 * @file
 * @brief
 *
 * @date 12.05.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmd/mount.h>
#include <embox/cmd.h>
#include <embox/device.h>
#include <fs/vfs.h>
#include <fs/ramdisk.h>
#include <fs/fs_drv.h>
#include <fs/node.h>
#include <fs/nfs.h>
#include <fs/nfs_clientstate.h>
#include <framework/example/self.h>
#include <mem/page.h>
#include <net/rpc/clnt.h>
#include <net/rpc/xdr.h>
#include <net/ip.h>
#include <net/socket.h>
#include <sys/time.h>
#include <util/array.h>

static int xdr_mnt_export(struct xdr *xs, export_dir_t *export);
static int xdr_mnt_service(struct xdr *xs, mount_service_t *mnt_srvc);
static int xdr_nfs_namestring(struct xdr *xs, rpc_string_t *str);
static int xdr_nfs_name_fh(struct xdr *xs, rpc_fh_string_t *fh);
static int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh);
static int xdr_nfs_get_dirlist(struct xdr *xs, char *buff);
static int xdr_nfs_get_dirdesc(struct xdr *xs, char *point);
static int xdr_nfs_get_attr(struct xdr *xs, char *point);

static int nfs_nfs_readdirplus(char *point, char *buff);

char snd_buf[MAXDIRCOUNT];
char rcv_buf[MAXDIRCOUNT];

nfs_fs_description_t *p_fs_fd;
nfs_file_description_t *p_rootfd;

/* nfs filesystem description pool */

typedef struct nfs_fs_description_head {
	struct list_head *next;
	struct list_head *prev;
	nfs_fs_description_t desc;
} nfs_fs_description_head_t;

static nfs_fs_description_head_t nfs_fs_pool[QUANTITY_RAMDISK];
static LIST_HEAD(nfs_free_fs);

#define param_to_head_fs(fs_param) \
	(uint32_t)(fs_param - offsetof(nfs_fs_description_head_t, desc))

static void init_nfs_fsinfo_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(nfs_fs_pool); i++) {
		list_add((struct list_head *) &nfs_fs_pool[i], &nfs_free_fs);
	}
}

static nfs_fs_description_t *nfs_fsinfo_alloc(void) {
	nfs_fs_description_head_t *head;
	nfs_fs_description_t *desc;

	if (list_empty(&nfs_free_fs)) {
		return NULL;
	}
	head = (nfs_fs_description_head_t *) (&nfs_free_fs)->next;
	list_del((&nfs_free_fs)->next);
	desc = &(head->desc);
	return desc;
}

static void nfs_fsinfo_free(nfs_fs_description_t *desc) {
	if (NULL == desc) {
		return;
	}
	list_add((struct list_head*) param_to_head_fs(desc), &nfs_free_fs);
}

/* nfs file description pool */

typedef struct nfs_file_description_head {
	struct list_head *next;
	struct list_head *prev;
	nfs_file_description_t desc;
} nfs_file_description_head_t;

static nfs_file_description_head_t nfs_files_pool[MAX_FILE_QUANTITY];
static LIST_HEAD(nfs_free_file);

#define param_to_head_file(file_param) \
	(uint32_t)(file_param - offsetof(nfs_file_description_head_t, desc))

static void init_nfs_fileinfo_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(nfs_files_pool); i++) {
		list_add((struct list_head *) &nfs_files_pool[i], &nfs_free_file);
	}
}

static nfs_file_description_t *nfs_fileinfo_alloc(void) {
	nfs_file_description_head_t *head;
	nfs_file_description_t *desc;

	if (list_empty(&nfs_free_file)) {
		return NULL;
	}
	head = (nfs_file_description_head_t *) (&nfs_free_file)->next;
	list_del((&nfs_free_file)->next);
	desc = &(head->desc);
	return desc;
}

static void nfs_fileinfo_free(nfs_file_description_t *desc) {
	if (NULL == desc) {
		return;
	}
	list_add((struct list_head*) param_to_head_file(desc), &nfs_free_file);
}

/* File operations */

static void *nfsfs_fopen(struct file_desc *desc,  const char *mode);
static int nfsfs_fclose(struct file_desc *desc);
static size_t nfsfs_fread(void *buf, size_t size, size_t count, void *file);
static size_t nfsfs_fwrite(const void *buf, size_t size, size_t count,
		void *file);
static int nfsfs_fseek(void *file, long offset, int whence);
static int nfsfs_ioctl(void *file, int request, va_list args);

static file_operations_t nfsfs_fop = { nfsfs_fopen, nfsfs_fclose, nfsfs_fread,
		nfsfs_fwrite, nfsfs_fseek, nfsfs_ioctl };
/*
 * file_operation
 */
static void *nfsfs_fopen(struct file_desc *desc, const char *mode) {
#if 0
	node_t *nod;
	uint8_t _mode;
	//char path [CONFIG_MAX_LENGTH_PATH_NAME];
	nfs_file_description_t *fd;

	nod = desc->node;
	fd = (nfs_file_description_t *)nod->attr;

	if ('r' == *mode) {
		_mode = O_RDONLY;
	}
	else if ('w' == *mode) {
		_mode = O_WRONLY;
	}
	else {
		_mode = O_RDONLY;
	}

	/*nfsfs_set_path ((uint8_t *) path, nod);
	cut_mount_dir((char *) path, fd->p_fs_dsc->root_name);

	if(0 == nfs_open_file(fd, (uint8_t *)path, _mode, sector_buff)) {
		return desc;
	}*/
#endif
	return NULL;
}

static int nfsfs_fseek(void *file, long offset, int whence) {
#if 0
	struct file_desc *desc;
	nfs_file_description_t *fd;
	uint32_t curr_offset;

	curr_offset = offset;

	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->attr;

	switch (whence) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		//curr_offset += fd->fi.pointer;
		break;
	case SEEK_END:
		//curr_offset = fd->fi.filelen;
		break;
	default:
		return -1;
	}

	/*nfs_fseek(fd, curr_offset, sector_buff);*/
#endif
	return 0;
}

static int nfsfs_fclose(struct file_desc *desc) {
	return 0;
}

static size_t nfsfs_fread(void *buf, size_t size, size_t count, void *file) {
	size_t rezult;
#if 0
	size_t size_to_read;
	struct file_desc *desc;
	size_t rezult;
	nfs_file_description_t *fd;

	size_to_read = size * count;
	desc = (struct file_desc *) file;
	fd = (nfs_file_description_t *)desc->node->attr;
#endif
	rezult = 0; /*nfs_read_file(fd, sector_buff, buf, &bytecount, size_to_read);
	if (0 == rezult) {
		return bytecount;
	}*/
	return rezult;
}

static size_t nfsfs_fwrite(const void *buf, size_t size,
	size_t count, void *file) {
	size_t rezult;
#if 0
	size_t size_to_write;
	struct file_desc *desc;
	size_t rezult;
	nfs_file_description_t *fd;

	size_to_write = size * count;
	desc = (struct file_desc *) file;

	fd = (nfs_file_description_t *)desc->node->attr;
#endif
	rezult = 0;/*nfs_write_file(fd, sector_buff, (uint8_t *)buf,
			&bytecount, size_to_write);
	if (0 == rezult) {
		return bytecount;
	}*/
	return rezult;
}

static int nfsfs_ioctl(void *file, int request, va_list args) {
	return 0;
}

/* File system operations */

static int nfsfs_init(void * par);
static int nfsfs_format(void * par);
static int nfsfs_mount(void * par);
static int nfsfs_create(void *par);
static int nfsfs_delete(const char *fname);

static fsop_desc_t nfsfs_fsop = { nfsfs_init, nfsfs_format, nfsfs_mount,
		nfsfs_create, nfsfs_delete };

static fs_drv_t nfsfs_drv = { "nfs", &nfsfs_fop, &nfsfs_fsop };

static int nfsfs_init(void * par) {
	init_nfs_fsinfo_pool();
	init_nfs_fileinfo_pool();
	return 0;
}

static int nfsfs_format(void *par) {
	ramdisk_params_t *params;
	node_t *nod;

	params = (ramdisk_params_t *) par;
	if (NULL == (nod = vfs_find_node(params->path, NULL))) {
		return -ENODEV;/*device not found*/
	}

	return 0;
}

static int nfsfs_mount(void *par) {
	mount_params_t *params;
	node_t *dir_node;
	nfs_fs_description_t *fs_des;
	nfs_file_description_t *fd;
	int rezult;

	params = (mount_params_t *) par;

	if (NULL == (dir_node = vfs_find_node(params->dir, NULL))) {
		/*TODO usually mount doesn't create a directory*/
		if (NULL == (dir_node = vfs_add_path (params->dir, NULL))) {
			return -ENODEV;/*device not found*/
		}
		dir_node->properties = DIRECTORY_NODE_TYPE;
	}

	fs_des = nfs_fsinfo_alloc();
	fd = nfs_fileinfo_alloc();
	fd->p_fs_dsc = fs_des;

	dir_node->fs_type = &nfsfs_drv;
	dir_node->file_info = (void *) &nfsfs_fop;
	dir_node->attr = (void *) fd;
	params->dev_node = dir_node;

	rezult = mount_nfs_filesystem(params);
	if(0 == rezult) {
		memcpy(&fd->fh, &fd->p_fs_dsc->fh, sizeof(fd->fh));
	}

	return rezult;
}

static int create_dir_entry(char *parent) {
	node_t *parent_node, *node;
	__u32 vf;
	char *point;
	nfs_file_description_t *parent_fd, *fd;
	nfs_filehandle_t *fh;
	readdir_desc_t *predesc;

	char full_path[CONFIG_MAX_LENGTH_PATH_NAME];
	char *rcv_buf;

	if (NULL == (parent_node = vfs_find_node(parent, NULL))) {
		return -1;/*device not found*/
	}

	parent_fd = (nfs_file_description_t *) parent_node->attr;
	fh = &parent_fd->fh;
	fh->count = fh->maxcount = DIRCOUNT;
	fh->cookie = 0;

	if(NULL == (rcv_buf = malloc(MAXDIRCOUNT * 2))) {
		return -1;
	}

	while (1) {

		if(0 >  nfs_nfs_readdirplus((char *)fh, rcv_buf)) {
			free(rcv_buf);
			return -1;
		}

		point = rcv_buf;
		vf = *(__u32 *)point;
		if(STATUS_OK != vf) {
			free(rcv_buf);
			return -1;
		}
		point += sizeof(vf);
		vf = *(__u32 *)point;

		if(VALUE_FOLLOWS_YES != vf) {
			break;
		}
		point += sizeof(vf);

		/*TODO copy root dir attr*/
		point += sizeof(dir_attribute_rep_t);
		vf = *(__u32 *)point;
		if(VALUE_FOLLOWS_YES != vf) {
			break;
		}

		while (VALUE_FOLLOWS_YES == (vf = *(__u32 *)point)) {
			point += sizeof(vf);
			predesc = (readdir_desc_t *) point;

			memset(full_path, 0, sizeof(full_path));

			strcpy(full_path, parent);
			strcat(full_path, "/");
			strcat(full_path, (const char *) predesc->file_name.name.data);
			if (NULL == (node = vfs_find_node(full_path, NULL))) {
				/*TODO usually mount doesn't create a directory*/
				if (NULL == (node = vfs_add_path (full_path, NULL))) {
					free(rcv_buf);
					return -1;/*device not found*/
				}
				fd = nfs_fileinfo_alloc();
			}
			else {
				fd = (nfs_file_description_t *) node->attr;
			}

			memcpy(&fd->name_dsc, &predesc->file_name,
					sizeof(predesc->file_name));

			if(VALUE_FOLLOWS_YES == predesc->vf_attr) {
				memcpy(&fd->attr, &predesc->file_attr,
						sizeof(predesc->file_attr));
			}
			if(VALUE_FOLLOWS_YES == predesc->vf_fh) {
				memcpy(&fd->fh, &predesc->file_handle,
						sizeof(predesc->file_handle));
				fd->fh.count = fd->fh.maxcount = DIRCOUNT;
				fd->fh.cookie = 0;
			}

			fd->p_fs_dsc = p_fs_fd;
			node->fs_type = &nfsfs_drv;
			node->file_info = (void *) &nfsfs_fop;
			node->attr = (void *)fd;

			if (NFS_DIRECTORY_NODE_TYPE ==
					(node->properties = (int) fd->attr.type)){
				if((0 != strcmp(fd->name_dsc.name.data, "."))
						&& (0 != strcmp(fd->name_dsc.name.data, ".."))) {
					create_dir_entry(full_path);
				}

			}

			point += sizeof(*predesc);

		}
		point += sizeof(vf);
		if(NFS_EOF == *(__u32 *)point) {
			fh->cookie = 0;
			break;
		}
		else {
			fh->cookie = predesc->file_name.cookie;
		}
	}
	free(rcv_buf);
	return 0;
}

static int nfsfs_create(void *par) {
	file_create_param_t *param;
	nfs_file_description_t *fd;
	node_t *node, *parents_node;
	int node_quantity;

	param = (file_create_param_t *) par;

	node = (node_t *)param->node;
	parents_node = (node_t *)param->parents_node;

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
		node_quantity = 3; /* need create . and .. directory */
	}
	else {
		node_quantity = 1;
	}

	for (int count = 0; count < node_quantity; count ++) {
		if(0 < count) {
			if(1 == count) {
				strcat(param->path, "/.");
			}
			else if(2 == count) {
				strcat(param->path, ".");
			}
			node = vfs_add_path (param->path, NULL);
		}

		fd = nfs_fileinfo_alloc();
		fd->p_fs_dsc = ((nfs_file_description_t *)
				parents_node->attr)->p_fs_dsc;
		node->fs_type = &nfsfs_drv;
		node->file_info = (void *) &nfsfs_fop;
		node->attr = (void *)fd;

		/*
		 * nfsfs_create_file called only once for the newly created directory.
		 * Creation of dir . and .. occurs into the function nfsfs_create_file.
		 */
		/*if(0 >= count) {
			nfsfs_create_file(par);
		}*/
	}
	/* cut /.. from end of PATH, if need */
	if (1 < node_quantity) {
		param->path[strlen(param->path) - 3] = '\0';
	}

	return 0;
}

static int nfsfs_delete(const char *fname) {
	nfs_file_description_t *fd;
	node_t *nod;
	//char path [CONFIG_MAX_LENGTH_PATH_NAME];

	nod = vfs_find_node(fname, NULL);
	fd = nod->attr;

	/*nfsfs_set_path ((uint8_t *) path, nod);
	cut_mount_dir((char *) path, fd->p_fs_dsc->root_name);

	if(nfs_unlike_file(fd, (uint8_t *) path, (uint8_t *) sector_buff)) {
		return -1;
	}*/

	nfs_fileinfo_free(fd);
	if(0)nfs_fsinfo_free(fd->p_fs_dsc);
	vfs_del_leaf(nod);
	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(nfsfs_drv);

static int nfs_unix_auth_set(struct client *clnt) {
	if (NULL == clnt)  {
		return -1;
	}
	if (NULL != clnt->ath) {
		auth_destroy(clnt->ath);
	}
	clnt->ath = authunix_create(EMBOX_MACHNAME, 0, 0, 0, NULL);
	if (clnt->ath == NULL) {
		return -1;
	}

	return 0;
}

static int nfs_clnt_destroy (nfs_fs_description_t *p_fsfd) {

	if (NULL != p_fsfd->mnt) {
		clnt_destroy(p_fsfd->mnt);
		p_fsfd->mnt = NULL;
	}
	if (NULL != p_fsfd->nfs) {
		clnt_destroy(p_fsfd->nfs);
		p_fsfd->nfs = NULL;
	}
	return 0;
}

static int nfs_prepare(char *dev) {
	char *src, *dst;

	/* copy name of server and mount filesystem server directory*/
	src = dev;
	dst = p_fs_fd->srv_name;

	do {
		if ('\0' == *src) {
			return -1;
		}
		*dst++ = *src++;
	} while (':' != *src);

	src++;
	dst = p_fs_fd->srv_dir;
	do {
		*dst++ = *src++;
	} while ('\0' != *src);
	return 0;
}

static int nfs_mnt_export(void) {
	struct timeval timeout = { 25, 0 };

	memset((void *)&p_fs_fd->export, 0, sizeof(p_fs_fd->export));
	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_EXPORT,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_mnt_export, (char *)&p_fs_fd->export,
		timeout) != RPC_SUCCESS) {
		/*
		 * An error occurred while calling the server.
		 * Print error message and die
		 */
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		/* error code in client, now */
		printf("mnt export failed. errno=%d\n", errno);
		return -1;
	}
	if(0 == strcmp(p_fs_fd->srv_dir, p_fs_fd->export.dir.data)) {
		return 0;
	}
	return -1;
}


static int nfs_mnt_null(void) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_NULL,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		printf("mnt null failed. errno=%d\n", errno);
		return -1;
	}
	return 0;
}

static int nfs_mnt_mount(void) {
	struct timeval timeout = { 25, 0 };
	char *point;
	nfs_filehandle_t *p_fh;

	struct mount_service mnt_svc;
	memset(&mnt_svc, 0, sizeof(mnt_svc));

	point = p_fs_fd->export.dir.data;
	p_fh = &p_fs_fd->fh;

	if (clnt_call(p_fs_fd->mnt, MOUNTPROC3_MNT,
		(xdrproc_t)xdr_wrapstring, (char *)&point,
		(xdrproc_t)xdr_mnt_service, (char *)&mnt_svc,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->mnt, p_fs_fd->srv_name);
		printf("mnt mount failed. errno=%d\n", errno);
		return -1;
	}

	mnt_svc.fh.count = DIRCOUNT;
	mnt_svc.fh.maxcount = DIRCOUNT;
	memcpy(p_fh, &mnt_svc.fh, sizeof(mnt_svc.fh));


	return 0;
}

static int nfs_nfs_null(char *point) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_NULL,
		(xdrproc_t)xdr_void, 0,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsstat(char *point) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSSTAT,
		(xdrproc_t)xdr_nfs_name_fh, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_nfs_fsinfo(char *point) {
	struct timeval timeout = { 25, 0 };

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_FSINFO,
		(xdrproc_t)xdr_nfs_name_fh, point,
		(xdrproc_t)xdr_void, 0,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs null failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}
static int nfs_nfs_readdirplus(char *point, char *buff) {
	struct timeval timeout = { 25, 0 };

	memset(buff, 0, sizeof(buff));

	if (clnt_call(p_fs_fd->nfs, NFSPROC3_READDIRPLUS,
		(xdrproc_t)xdr_nfs_readdirplus, point,
		(xdrproc_t)xdr_nfs_get_dirlist, buff,
		timeout) != RPC_SUCCESS) {
		clnt_perror(p_fs_fd->nfs, p_fs_fd->srv_name);
		printf("nfs readdirplus failed. errno=%d\n", errno);
		return -1;
	}

	return 0;
}

static int nfs_call(uint32_t proc) {

	return 0;

}

static int nfs_client_init(void) {

	nfs_clnt_destroy(p_fs_fd);
	/* Create nfs programm clients */
	p_fs_fd->mnt = clnt_create(p_fs_fd->srv_name,
			MOUNT_PROGNUM, MOUNT_VER, "tcp");
	if (p_fs_fd->mnt == NULL) {
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}
	if(0 != nfs_unix_auth_set(p_fs_fd->mnt)) {
		return -1;
	}

	p_fs_fd->nfs = clnt_create(p_fs_fd->srv_name, NFS_PROGNUM, NFS_VER, "tcp");
	if (p_fs_fd->nfs == NULL) {
		clnt_pcreateerror(p_fs_fd->srv_name);
		return -1;
	}

	return nfs_unix_auth_set(p_fs_fd->nfs);
}

static int create_filechain (void) {
	char *parent;
	node_t *node;
	nfs_file_description_t *parent_fd;

	parent = p_fs_fd->mnt_point;
	if (NULL == (node = vfs_find_node(parent, NULL))) {
		return -1;/*device not found*/
	}

	parent_fd = (nfs_file_description_t *) node->attr;
	memcpy(&parent_fd->fh, &p_fs_fd->fh, sizeof(p_fs_fd->fh));

	if(0 >  create_dir_entry(parent)) {
		return -1;
	}
	return 0;
}


static int nfs_mount(void) {

	if (0 > nfs_mnt_export()) {
		return -1;
	}
	if (0 > nfs_mnt_null()) {
		return -1;
	}
	if (0 > nfs_mnt_mount()) {
		return -1;
	}
	if(0 >  nfs_nfs_null((char *)&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  nfs_nfs_fsstat((char *)&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  nfs_nfs_fsinfo((char *)&p_fs_fd->fh)) {
		return -1;
	}
	if(0 >  create_filechain()) {
		return -1;
	}

	return 0;
}


int mount_nfs_filesystem(void *par) {
	mount_params_t *params;
	node_t *node;
	nfs_file_description_t *fd;

	params = (mount_params_t *) par;
	node = params->dev_node;
	fd = p_rootfd = (nfs_file_description_t *)node->attr;
	p_fs_fd = fd->p_fs_dsc;
	strcpy(p_fs_fd->mnt_point, params->dir);

	if(0 >  nfs_prepare(params->dev)) {
		return -1;
	}
	if(0 >  nfs_client_init()) {
		return -1;
	}
	if(0 >  nfs_mount()) {
		nfs_clnt_destroy(p_fs_fd);
		return -1;
	}
	return 0;
}

int nfs_statfs(void) {

	nfs_call(NFSPROC3_NULL);
	nfs_call(NFSPROC3_FSSTAT);
	nfs_call(NFSPROC3_FSINFO);
	return 0;
}

static int xdr_mnt_export(struct xdr *xs, export_dir_t *export) {
	char *point;

	assert(export != NULL);

	if (xdr_u_int(xs, &export->follow)) {
		if (VALUE_FOLLOWS_YES == export->follow) {
			point = export->dir.data;
			if (xdr_bytes(xs, (char **)&point,
					&export->dir.len, sizeof export->dir.data)) {
				return XDR_SUCCESS;
			}
		}
	}

	return XDR_FAILURE;
}

static int xdr_mnt_service(struct xdr *xs, mount_service_t *mnt_srvc) {
	char *point;

	assert(mnt_srvc != NULL);

	if (xdr_u_int(xs, &mnt_srvc->status)) {
		if (STATUS_OK == mnt_srvc->status) {
			point = mnt_srvc->fh.name_fh.data;
			if (xdr_bytes(xs, (char **)&point, &mnt_srvc->fh.name_fh.len,
					sizeof mnt_srvc->fh.name_fh)) {
				if (xdr_u_int(xs, &mnt_srvc->flv)) {
					return XDR_SUCCESS;
				}
			}
		}
	}

	return XDR_FAILURE;
}

static int xdr_nfs_namestring(struct xdr *xs, rpc_string_t *fh) {
	char *point;

	assert(fh != NULL);

	point = fh->data;
	if(xdr_bytes(xs, (char **)&point, &fh->len, sizeof(*fh))) {
		point += fh->len;
		*point = 0;
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;
}

static int xdr_nfs_name_fh(struct xdr *xs, rpc_fh_string_t *fh) {
	char *point;

	assert(fh != NULL);

	point = fh->data;
	if(xdr_bytes(xs, (char **)&point, &fh->len, sizeof(*fh))) {
		return XDR_SUCCESS;
	}

	return XDR_FAILURE;
}

static int xdr_nfs_readdirplus(struct xdr *xs, nfs_filehandle_t *fh) {
	char *point;
	__u32 size;

	assert(fh != NULL);

	point = fh->name_fh.data;
	if(xdr_bytes(xs, (char **)&point, &fh->name_fh.len, sizeof(*fh))) {
		point = (char *)&fh->cookie;
		size = sizeof(fh->cookie) + sizeof(fh->cookieverf);
		if(xdr_opaque(xs, point, size)) {
			if (xdr_u_int(xs, &fh->count)) {
				if (xdr_u_int(xs, &fh->maxcount)) {
					return XDR_SUCCESS;
				}
			}
		}
	}

	return XDR_FAILURE;
}

static int xdr_nfs_get_dirattr(struct xdr *xs, char *point) {
	dir_attribute_rep_t *dir_attr;

	assert(point != NULL);

	dir_attr = (dir_attribute_rep_t *) point;
	if(XDR_SUCCESS == xdr_nfs_get_attr(xs, point)) {
		if (xdr_opaque(xs, (char *)&dir_attr->verifier,
				sizeof(dir_attr->verifier))) {
			return XDR_SUCCESS;
		}
	}
	return XDR_FAILURE;
}

static int xdr_nfs_get_name(struct xdr *xs, char *point) {
	file_name_t *file;

	assert(point != NULL);

	file = (file_name_t *) point;
	return (xdr_opaque(xs, (char *)&file->file_id, sizeof(file->file_id))
			&& xdr_nfs_namestring(xs, &file->name)
			&& xdr_opaque(xs, (char *)&file->cookie, sizeof(file->cookie))
			&& (int)(0 != (p_fs_fd->fh.cookie = file->cookie)));
}

static int xdr_nfs_get_attr(struct xdr *xs, char *point) {
	file_attribute_rep_t *attr;

	assert(point != NULL);

	attr = (file_attribute_rep_t *) point;
	return (xdr_u_int(xs, &attr->type) && xdr_u_int(xs, &attr->mode)
			&& xdr_u_int(xs, &attr->nlink) && xdr_u_int(xs, &attr->uid)
			&& xdr_u_int(xs, &attr->gid) && xdr_u_hyper(xs, &attr->size)
			&& xdr_u_hyper(xs, &attr->used) && xdr_u_int(xs, &attr->specdata1)
			&& xdr_u_int(xs, &attr->specdata2) && xdr_u_hyper(xs, &attr->fsid)
			&& xdr_u_hyper(xs, &attr->file_id)
			&& xdr_u_int(xs, &attr->atime.second)
			&& xdr_u_int(xs, &attr->atime.nano_sec)
			&& xdr_u_int(xs, &attr->mtime.second)
			&& xdr_u_int(xs, &attr->mtime.nano_sec)
			&& xdr_u_int(xs, &attr->ctime.second)
			&& xdr_u_int(xs, &attr->ctime.nano_sec));
}

static int xdr_nfs_get_dirdesc(struct xdr *xs, char *point) {
	readdir_desc_t *desc;
	__u32 *vf;

	assert(point != NULL);

	desc = (readdir_desc_t *) point;
	while(1) {
		if(XDR_SUCCESS != xdr_nfs_get_name(xs,
				(char *) &desc->file_name)) {
			break;
		}

		vf = &desc->vf_attr;
		if (xdr_u_int(xs, vf)) {
			if(VALUE_FOLLOWS_YES == *vf) {
				if(XDR_SUCCESS != xdr_nfs_get_attr(xs,
						(char *) &desc->file_attr)) {
					break;
				}
			}
		}

		vf = &desc->vf_fh;
		if (xdr_u_int(xs, vf)) {
			if(VALUE_FOLLOWS_YES == *vf) {
				if(XDR_SUCCESS != xdr_nfs_name_fh(xs,
						&desc->file_handle.name_fh)) {
					break;
				}
			}
		}
		return XDR_SUCCESS;
	}
	return XDR_FAILURE;
}

/*
static size_t get_sizeof_readdirentry(readdir_desc_t *file) {
	size_t len;

	len = sizeof(file->file_name.file_id);
	len += sizeof(file->file_name.cookie);
	len += file->file_name.name.len + sizeof(file->file_name.name.len);

	len += sizeof(file->vf_attr);
	len += sizeof(file->file_attr);

	len += sizeof(file->vf_fh);
	len += file->file_handle.name_fh.len +
			sizeof(file->file_handle.name_fh.len);
	return len;
}
*/

static int xdr_nfs_get_dirlist(struct xdr *xs, char *buff) {
	char *point;
	__u32 vf;
	readdir_desc_t *file;

	assert(buff != NULL);
	point = buff;

	if (xdr_u_int(xs, &vf)) {
		if(STATUS_OK == (*(__u32 *) point = vf)) {
			point += sizeof(vf);
			if (xdr_u_int(xs,  &vf)) {
				if(VALUE_FOLLOWS_YES == (*(__u32 *) point = vf)) {
					point += sizeof(vf);
					if(xdr_nfs_get_dirattr(xs, point)) {
						point += sizeof(dir_attribute_rep_t);
						while(1) {
							if (xdr_u_int(xs, &vf)) {
								if(VALUE_FOLLOWS_YES ==
										(*(__u32 *) point = vf)) {
									point += sizeof(vf);
									file = (readdir_desc_t *)point;
									if(XDR_SUCCESS == xdr_nfs_get_dirdesc(xs,
											point)) {
										point += sizeof(readdir_desc_t);
									}
								}
								else {
									point += sizeof(vf);
									break;
								}
							}
						}
						/*read EOF */
						if (xdr_u_int(xs, (__u32 *) point)) {
							return XDR_SUCCESS;
						}
					}
				}
			}
		}
	}
	return XDR_FAILURE;
}



/**
 * @file inode.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.01.2020
 */

#include <assert.h>

#include <fs/inode.h>
#include <fs/dentry.h>

void *inode_priv(const struct inode *node) {
	return node->i_data;
}

void inode_priv_set(struct inode *node, void *priv) {
	node->i_data = priv;
}

size_t inode_size(const struct inode *node) {
	return node->length;
}

void inode_size_set(struct inode *node, size_t sz) {
	node->length = sz;
}

unsigned inode_ctime(const struct inode *node) {
	return node->i_ctime;
}

void inode_ctime_set(struct inode *node, unsigned ctime) {
	node->i_ctime = ctime;
}

unsigned inode_mtime(const struct inode *node) {
	return node->i_mtime;
}

void inode_mtime_set(struct inode *node, unsigned mtime) {
	node->i_mtime = mtime;
}

char *inode_name(struct inode *node) {
	return node->i_dentry->name;
}

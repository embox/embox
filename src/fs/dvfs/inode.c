/**
 * @file inode.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.01.2020
 */

#include <assert.h>
#include <string.h>

#include <util/math.h>

#include <fs/inode.h>
#include <fs/dentry.h>

void *inode_priv(const struct inode *node) {
	return node->i_privdata;
}

void inode_priv_set(struct inode *node, void *priv) {
	node->i_privdata = priv;
}

size_t inode_size(const struct inode *node) {
	return node->i_size;
}

void inode_size_set(struct inode *node, size_t sz) {
	node->i_size = sz;
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

char *inode_name_set(struct inode *node, const char *name) {
	char *node_name;
	size_t name_len;

	node_name = node->i_dentry->name;
	name_len = sizeof(node->i_dentry->name) - 1;
	
	strncpy(node_name, name, name_len);
	node_name[name_len] = '\0';

	return node_name;
}

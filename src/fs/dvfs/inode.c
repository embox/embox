/**
 * @file inode.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.01.2020
 */

#include <assert.h>

#include <fs/inode.h>

void inode_priv_set(struct inode *node, void *priv) {
	node->i_data = priv;
}

void *inode_priv(struct inode *node) {
	return node->i_data;
}

/**
 * @file
 * @brief ext2fuse support
 *
 * @date 16.11.2015
 * @author Alexander Kalmuk
 */

#include <fs/fuse_module.h>

//FUSE_MODULE_DEF("ext2fuse", "ext2fuse");

struct fuse_module fm = {"ext2fuse", "ext2fuse"};
ARRAY_SPREAD_DECLARE(struct fuse_module *, fuse_module_repo);
ARRAY_SPREAD_ADD(fuse_module_repo, &fm);

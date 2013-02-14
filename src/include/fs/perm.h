/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.02.2013
 */

#ifndef SRC_FS_PERM_H_
#define SRC_FS_PERM_H_

struct node;

/**
 * @brief permision mask of mode in least signficant bits (in S_RWXO range)
 *
 * @param node
 *
 * @return mask
 */
extern int fs_perm_mask(struct node *node);

/**
 * @brief Check node mask for desired permision mask @fd_flags
 *
 * @param node
 * @param fd_flags
 *
 * @return
 *	-EACCES on permission violation
 *	0 on success
 */
extern int fs_perm_check(struct node *node, int fd_flags);

/**
 * @brief Perform node lookup with respect for directory rights.
 *
 * @param root Lookup starting from @root
 * @param path Path to lookup
 * @param pathlast pointer to path, which is not resolved, starting from *@node
 * @param node pointer to last finded node
 *
 * @return
 *	-EACCESS on permission violation
 *	-ENOENT on not all path resolved
 *	0 on success
 */
extern int fs_perm_lookup(struct node *root, const char *path,
		const char **pathlast, struct node **node);

#endif /* SRC_FS_PERM_H_ */


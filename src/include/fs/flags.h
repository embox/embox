/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    19.02.2013
 */

#ifndef FS_FLAGS_H_
#define FS_FLAGS_H_

#define FS_MAY_EXEC    (0x1 << 0) /* must equal to S_IXOTH */
#define FS_MAY_WRITE   (0x1 << 1) /* must equal to S_IWOTH */
#define FS_MAY_READ    (0x1 << 2) /* must equal to S_IROTH */
#define FS_MAY_APPEND  (0x1 << 3)

#if FS_MAY_READ  != S_IROTH || \
	FS_MAY_WRITE != S_IWOTH || \
	FS_MAY_EXEC  != S_IXOTH
# error "Please arrange FS_MAY_XXX to match S_I[RWX]OTH values"
#endif

#endif /* FS_FLAGS_H_ */


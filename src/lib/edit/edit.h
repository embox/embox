/**
 * @file
 * @brief Tools for file edition
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-01-15
 */

/**
 * @brief Delete given text segment from file
 *
 * @param fd File descriptor
 * @param length Length of the segment
 *
 * @return Negative error code or zero if succeeded
 */
extern int fdelete_from_file(int fd, size_t length);

/**
 * @brief Insert given text segment into file file
 *
 * @param fd File descriptor
 * @param buf Pointer to given text segment
 * @param length Length of the segment
 *
 * @return Negative error code or zero if succeeded
 */
extern int finsert_into_file(int fd, char *buf, size_t length);

/**
 * @brief Libc-style wrapper for fdelete_from_file
 *
 * @return Negative error code or zero if succeeded
 */
extern int delete_from_file(FILE *file, size_t length);

/**
 * @brief Libc-style wrapper for finsert_into_file
 *
 * @return Negative error code or zero if succeeded
 */
extern int insert_into_file(FILE *file, char *buf, size_t length);

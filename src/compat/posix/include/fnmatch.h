/**
 * @file
 * @brief fnmatch.h
 *
 * @date 20.12.18
 * @author Filipp Chubukov
 */
#ifndef POSIX_FNMATCH_H_
#define POSIX_FNMATCH_H_

#define FNM_NOMATCH 3
#define FNM_PATHNAME 1

extern int fnmatch(const char *pattern, const char *string, int flags);
#endif /* POSIX_FNMATCH_H_ */

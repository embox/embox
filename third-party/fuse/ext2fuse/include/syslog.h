/**
 * @file
 *
 * @date Jun 3, 2014
 * @author: Anton Bondarev
 */

#ifndef SYSLOG_H_
#define SYSLOG_H_

#define LOG_CRIT    2   /* critical conditions */
#define LOG_ERR     3   /* error conditions */
#define LOG_WARNING 4   /* warning conditions */
#define LOG_NOTICE  5   /* normal but significant condition */
#define LOG_INFO    6   /* informational */
#define LOG_DEBUG   7   /* debug-level messages */

static inline void syslog(int prio, const char *format, ...) {
}

#endif /* SYSLOG_H_ */

/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#ifndef POSIX_SYSLOG_H_
#define POSIX_SYSLOG_H_

#include <stdarg.h>
#include <sys/cdefs.h>

#define LOG_NONE    0 /* To disable logging */

/* Priorities for syslog() */
#define LOG_EMERG   1 /* A panic condition */
#define LOG_ALERT   2 /* A condition that should be corrected immediately */
#define LOG_CRIT    3 /* Critical conditions, such as hard device errors */
#define LOG_ERR     4 /* Errors */
#define LOG_WARNING 5 /* Warning messages */
#define LOG_NOTICE  6 /* Not errors, but may require special handling */
#define LOG_INFO    7 /* Informational messages */
#define LOG_DEBUG   8 /* Messages for debugging a program */

/* Facility codes */
#define LOG_USER    (1U << 0) /* This is the default facility identifier */
#define LOG_LOCAL0  (1U << 1) /* Reserved for local use */
#define LOG_LOCAL1  (1U << 2) /* Reserved for local use */
#define LOG_LOCAL2  (1U << 3) /* Reserved for local use */
#define LOG_LOCAL3  (1U << 4) /* Reserved for local use */
#define LOG_LOCAL4  (1U << 5) /* Reserved for local use */
#define LOG_LOCAL5  (1U << 6) /* Reserved for local use */
#define LOG_LOCAL6  (1U << 7) /* Reserved for local use */
#define LOG_LOCAL7  (1U << 8) /* Reserved for local use */

/* Non-POSIX facility codes */
#define LOG_KERN    (1U << 9)  /* Kernel */
#define LOG_MAIL    (1U << 10) /* Mail system */
#define LOG_NEWS    (1U << 11) /* News system */
#define LOG_UUCP    (1U << 12) /* UUCP system */
#define LOG_DAEMON  (1U << 13) /* System daemon */
#define LOG_AUTH    (1U << 14) /* Authorization daemon */
#define LOG_CRON    (1U << 15) /* Clock daemon */
#define LOG_LPR     (1U << 16) /* Printer system */

/* Option flags for openlog() */
#define LOG_PID     (1U << 0) /* Log the process ID with each message */
#define LOG_CONS    (1U << 1) /* Write messages to the system console */
#define LOG_NDELAY  (1U << 2) /* Open the connection immediately */
#define LOG_ODELAY  (1U << 3) /* Delay open until syslog() is called */
#define LOG_NOWAIT  (1U << 4) /* Do not wait for child processes */

__BEGIN_DECLS

extern void closelog(void);
extern void openlog(const char *ident, int logopt, int facility);
extern void syslog(int priority, const char *message, ...);
extern void vsyslog(int priority, const char *message, va_list args);
extern int setlogmask(int maskpri);

__END_DECLS

#endif /* POSIX_SYSLOG_H_ */

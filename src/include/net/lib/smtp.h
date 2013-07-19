/**
 * @file
 * @brief Simple Mail Transfer Protocol (RFC 2821)
 *
 * @date 21.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_SMTP_H_
#define NET_LIB_SMTP_H_

#define SMTP_PORT 25

#define SMTP_STATUS_LEN  512
#define SMTP_COMMAND_LEN 512
#define SMTP_TEXT_LEN    1000
#define SMTP_PATH_LEN    256

struct smtp_session {
	int sock;
	int code;
	char status[SMTP_STATUS_LEN + 1];
};

extern int smtp_open(struct smtp_session *ss, const char *host,
		unsigned short port);
extern int smtp_close(struct smtp_session *ss);
extern int smtp_ok(struct smtp_session *ss);
extern int smtp_code(struct smtp_session *ss);
extern const char * smtp_status(struct smtp_session *ss);

extern int smtp_mail_from(struct smtp_session *ss,
		const char *addr); /* FIXME addr with <> brackets */
extern int smtp_rcpt_to(struct smtp_session *ss,
		const char *addr); /* FIXME addr with <> brackets */
extern int smtp_helo(struct smtp_session *ss,
		const char *domain);
extern int smtp_ehlo(struct smtp_session *ss,
		const char *domain);
extern int smtp_data(struct smtp_session *ss);
extern int smtp_data_add(struct smtp_session *ss,
		const char *data);
extern int smtp_data_end(struct smtp_session *ss);
extern int smtp_rset(struct smtp_session *ss);
extern int smtp_vrfy(struct smtp_session *ss,
		const char *str);
extern int smtp_expn(struct smtp_session *ss,
		const char *str);
extern int smtp_help(struct smtp_session *ss);
extern int smtp_noop(struct smtp_session *ss);
extern int smtp_quit(struct smtp_session *ss);
extern int smtp_auth_plain(struct smtp_session *ss,
		const char *name, const char *secret);

#endif /* NET_LIB_SMTP_H_ */

/**
 * @file
 * @brief Post Office Protocol - Version 3 (RFC 1939)
 *
 * @date 17.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_LIB_POP3_H_
#define NET_LIB_POP3_H_

#define POP3_PORT       110
#define POP3_MSG_ANY     -1

#define POP3_STATUS_LEN 512

struct pop3_session {
	int sock;
	int ok;
	char status[POP3_STATUS_LEN + 1];
	char *data;
};

extern int pop3_open(struct pop3_session *p3s, const char *host,
		unsigned short port);
extern int pop3_close(struct pop3_session *p3s);
extern int pop3_ok(struct pop3_session *p3s);
extern const char * pop3_status(struct pop3_session *p3s);
extern const char * pop3_data(struct pop3_session *p3s);

extern int pop3_stat(struct pop3_session *p3s);
extern int pop3_list(struct pop3_session *p3s,
		int msg_or_any);
extern int pop3_retr(struct pop3_session *p3s, int msg);
extern int pop3_dele(struct pop3_session *p3s, int msg);
extern int pop3_noop(struct pop3_session *p3s);
extern int pop3_rset(struct pop3_session *p3s);
extern int pop3_quit(struct pop3_session *p3s);
extern int pop3_top(struct pop3_session *p3s, int msg,
		unsigned int n);
extern int pop3_uidl(struct pop3_session *p3s,
		int msg_or_any);
extern int pop3_user(struct pop3_session *p3s,
		const char *name);
extern int pop3_pass(struct pop3_session *p3s,
		const char *secret);
extern int pop3_apop(struct pop3_session *p3s,
		const char *name, const char *secret, const char *salt);

#endif /* NET_LIB_POP3_H_ */

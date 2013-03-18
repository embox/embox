/**
 * @file
 * @brief Post Office Protocol - Version 3 (RFC 1939)
 *
 * @date 17.03.13
 * @author Ilia Vaprol
 */

#ifndef NET_POP3_H_
#define NET_POP3_H_

#define POP3_PORT     110
#define POP3_BUFF_SZ  512
#define POP3_MSG_NONE -1

struct pop3_session {
	int sock;
	char buff[POP3_BUFF_SZ];
	char *status;
};

extern int pop3_session_create(struct pop3_session *p3s,
		const char *server, unsigned short int port);
extern int pop3_session_destroy(struct pop3_session *p3s);

extern int pop3_stat(struct pop3_session *p3s);
extern int pop3_list(struct pop3_session *p3s,
		int msg_or_none);
extern int pop3_retr(struct pop3_session *p3s, int msg);
extern int pop3_dele(struct pop3_session *p3s, int msg);
extern int pop3_noop(struct pop3_session *p3s);
extern int pop3_rset(struct pop3_session *p3s);
extern int pop3_quit(struct pop3_session *p3s);
extern int pop3_top(struct pop3_session *p3s, int msg,
		unsigned int n);
extern int pop3_uidl(struct pop3_session *p3s,
		int msg_or_none);
extern int pop3_user(struct pop3_session *p3s,
		const char *name);
extern int pop3_pass(struct pop3_session *p3s,
		const char *str);
extern int pop3_apop(struct pop3_session *p3s,
		const char *name, const char *str, const char *salt);

#endif /* NET_POP3_H_ */

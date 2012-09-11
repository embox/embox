/**
 * @file
 * @brief Tiny telnetd server
 * @date 18.04.2012
 * @author Vladimir Sokolov
 */

#include <unistd.h>
#include <string.h>
#include <net/ip.h>
#include <net/socket.h>
#include <net/inetdevice.h>
#include <embox/cmd.h>
#include <cmd/shell.h>
#include <err.h>
#include <errno.h>
#include <kernel/task.h>
EMBOX_CMD(exec);

	/* Upper limit of concurent telnet connections.
	 * ToDo: move those into config files
	 */
#define TELNETD_MAX_CONNECTIONS 10
static int clients[TELNETD_MAX_CONNECTIONS];
	/* Telnetd address bind to */
#define RLOGIN_ADDR INADDR_ANY
	/* Telnetd port bind to */
#define RLOGIN_PORT 23

	/* Allow to turn off/on extra debugging information */
#if 0
#	define RLOGIN_DEBUG(x) do {\
		x;\
	} while (0);
#else
#	define RLOGIN_DEBUG(x) do{\
	} while (0);
#endif

static int telnetd_retval = -1;
static void fatal_error(const char *msg, int code) {
#if 0	/* LOG_ERROR might be off now */
	LOG_ERROR("%s, code=%d, last errno=%d\n", msg, code, errno);
#else
	RLOGIN_DEBUG(printf("%s, code=%d, last errno=%d\n", msg, code, errno));
#endif
	telnetd_retval = -1;
	thread_exit(&telnetd_retval);
	assert(0);
}

	/* Out a bunch of different error messages to the output and to the socket */
static void out_msgs(const char *msg, const char *msg2, const char *msg3,
					int client_descr, struct sockaddr_in *client_socket) {
	const int m_len = strlen(msg) + 1;
	RLOGIN_DEBUG(printf("%s", msg2));
	if(m_len != sendto(client_descr, msg, m_len, 0,
					   (struct sockaddr *)client_socket, sizeof(*client_socket))) {
		RLOGIN_DEBUG(printf("Can't write to the socket (%s)\n", msg3));
	}
}

	/* Executes real shell or it's emulator */
static void run(void) {
#if 0
	while (1) {
		char ch;
		read(0, &ch, 1);				/* Not working. Treats socket as a file */
		printf("telnet!%c\n", ch);		/* Gives something reasonable */
	}
#else
		/* Run tish.
		 * Is it possible to overwrite its promt here?
		 */
	shell_run();
#endif
}


#define T_WILL		251
#define T_WONT		252
#define T_DO		253
#define T_DONT		254
#define T_IAC		255

#define O_ECHO		1		/* Manage ECHO, RFC 857 */
#define O_GO_AHEAD	3		/* Disable GO AHEAD, RFC 858 */

	/* Skip management session */
static void ignore_telnet_options(void) {
	unsigned char ch = getchar();
	while ((ch & (1 << 7)) && (ch != T_IAC))
		ch = getchar();		/* Something related with management, probably in string mode */

	while (1) {
		if (ch == T_IAC) {
			unsigned char op_type = getchar();
			unsigned char param = getchar();
			if (op_type == T_WILL) {
				if (param == O_GO_AHEAD) {
						/* Agree */
					printf("%c%c%c", T_IAC, T_DO, param);
				} else {
						/* Decline */
					printf("%c%c%c", T_IAC, T_DONT, param);
				}
			} else if (op_type == T_DO) {
				if ( (param == O_GO_AHEAD) || (param == O_ECHO) ) {
						/* Agree */
					printf("%c%c%c", T_IAC, T_WILL, param);
				} else {
						/* Decline */
					printf("%c%c%c", T_IAC, T_WONT, param);
				}
			} else {
				/* Currently do nothing, probably it's an answer for our request */
			}
		}
		else {
			ungetchar(ch);	/* Return this symbol, it belongs to usual traffic */
			return;
		}
		ch = getchar();
	}
}

	/* Identify our wishes for the session */
static void set_our_term_parameters(void) {
	printf("%c%c%c", T_IAC, T_WILL, O_GO_AHEAD);
	printf("%c%c%c", T_IAC, T_WILL, O_ECHO);
}

#undef T_WILL
#undef T_WONT
#undef T_DO
#undef T_DONT
#undef T_IAC

#undef O_ECHO
#undef O_GO_AHEAD

	/* Shell thread for telnet */
static void *telnet_thread_handler(void* args) {
	int *client_descr_p = (int *)args;

	close(0);
	close(1);
	close(2);

	dup(*client_descr_p);
	dup(*client_descr_p);
	dup(*client_descr_p);

		/* Hack. Emulate future output, we need a char from user to exit from
		 * parameters mode
		 */
	{
		static const char* prompt = "embox>"; /* OPTION_STRING_GET(prompt); */
		printf("Welcome to telnet!\n%s", prompt);
	}

		/* Operate with settings */
	set_our_term_parameters();
	ignore_telnet_options();

		/* Run shell */
	run();

	close(*client_descr_p);
	*client_descr_p = -1;

	return NULL;
}


static int exec(int argc, char **argv) {
	int res;

	struct sockaddr_in listening_socket;
	int listening_descr;

	clients[0] = -1;
	for (res = 1; res < TELNETD_MAX_CONNECTIONS; res++) {
		memcpy(&clients[res], &clients[0], sizeof(clients[0]));
	}

	listening_socket.sin_family = AF_INET;
	listening_socket.sin_port= htons(RLOGIN_PORT);
	listening_socket.sin_addr.s_addr = htonl(RLOGIN_ADDR);

	if (!((RLOGIN_ADDR == INADDR_ANY) || ip_is_local(RLOGIN_ADDR, false, false) )) {
		fatal_error("telnetd address is incorrect", RLOGIN_ADDR);
	}

	if ((listening_descr = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fatal_error("can't create socket", listening_descr);
	}

	if ((res = bind(listening_descr, (struct sockaddr *)&listening_socket,
					sizeof(listening_socket))) < 0) {
		fatal_error("bind() failed", res);
	}

	if ((res = listen(listening_descr, TELNETD_MAX_CONNECTIONS)) < 0) {
		fatal_error("listen() failed", res);
	}

	RLOGIN_DEBUG(printf("telnetd is ready to accept connections\n"));
	while (1) {
		struct sockaddr_in client_socket;
		int client_socket_len = sizeof(client_socket);
		int client_descr = accept(listening_descr, (struct sockaddr *)&client_socket,
								  &client_socket_len);

		if (client_descr < 0) {
			RLOGIN_DEBUG(printf("accept() failed. code=%d\n", client_descr));
		} else {
			uint i;

			RLOGIN_DEBUG(printf("Attempt to connect from address %s:%d",
					inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port)) );

			for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
				if (clients[i] == -1) {
					clients[i] = client_descr;

					if ((res = new_task(telnet_thread_handler, &clients[i]))) {
						out_msgs("Internal error with shell creation\n", " failed. Can't create shell\n",
								 "shell_create", client_descr, &client_socket);
						RLOGIN_DEBUG(printf("thread_create() returned with code=%d\n", res));
						clients[i] = -1;
					} else {
						RLOGIN_DEBUG(printf(" success\n"));
					}
					close(client_descr);
					client_descr = -1;
					break;
				} /* if - if we have space for this connection */
			} /* for */
			if (client_descr != -1) {
					/* No space for this connection */
				out_msgs("Limit of concurrent connections was reached\n",
					 " failed. Too many existent connections\n",
					 "too_many", client_descr, &client_socket);
				close(client_descr);
			}
		} /* if - accept() is sucessful */
	} /* endless cycle */

	assert(0);
	{
		/* Free resources, don't touch threads, but close socket decriptor
		 * So it'll influence to them
		 */
		uint i;
		for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
			if (clients[i] != -1) {
				close(clients[i]);
			}
		}
		close(listening_descr);
	}

	return ENOERR;
}

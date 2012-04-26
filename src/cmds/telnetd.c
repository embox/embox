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

EMBOX_CMD(exec);

	/* Upper limit of concurent telnet connections.
	 * ToDo: move those into config files
	 */
#define TELNETD_MAX_CONNECTIONS 10
static int clients[TELNETD_MAX_CONNECTIONS];
	/* Telnetd address bind to */
#define TELNETD_ADDR INADDR_ANY
	/* Telnetd port bind to */
#define TELNETD_PORT 23

	/* Allow to turn off/on extra debugging information */
#if 1
#	define MD(x) do {\
		x;\
	} while (0);
#else
#	define MD(x) do{\
	} while (0);
#endif

static int telnetd_retval = -1;
static void fatal_error(const char *msg, int code) {
#if 0	/* LOG_ERROR might be off now */
	LOG_ERROR("%s, code=%d, last errno=%d\n", msg, code, errno);
#else
	MD(printf("%s, code=%d, last errno=%d\n", msg, code, errno));
#endif
	telnetd_retval = -1;
	thread_exit(&telnetd_retval);
	assert(0);
}

	/* Out a bunch of different error messages to the output and to the socket */
static void out_msgs(const char *msg, const char *msg2, const char *msg3,
					int client_descr, struct sockaddr_in *client_socket) {
	const int m_len = strlen(msg) + 1;
	MD(printf("%s", msg2));
	if(m_len != sendto(client_descr, msg, m_len, 0,
					   (struct sockaddr *)client_socket, sizeof(*client_socket))) {
		MD(printf("Can't write to the socket (%s)\n", msg3));
	}
}

	/* Executes real shell or it's emulator */
static void run(void) {
	printf("Welcome to telnet!\n");
#if 0
	while (1) {
		char ch;
		read(0, &ch, 1);				/* Not working. Treats socket as a file */
		printf("telnet!%c\n", ch);		/* Gives something reasonable */
	}
#else
		/* Run tish */
	shell_run();
#endif
}

#if 1
	/* Shell thread for telnet */
static void *telnet_thread_handler(void* args) {
	int *client_descr_p = (int *)args;
	struct task_resources *t_r = task_self_res();
	struct idx_desc *i_d = task_res_idx_get(t_r, *client_descr_p);

		/* Redirect stdin, stdout, stderr to our socket
		 * Unfortunately it's not working. We try to treat stdout as
		 * a socket, but we can't map 1 into a socket descriptor
		 * ToDo:
		 * 	It's unsertain what we should do with the original descriptors
		 *	We can close them, but it may corrupt the original task
		 */
	close(0);
	close(1);
	close(2);
	task_res_idx_set(t_r, 0, i_d);
	task_res_idx_set(t_r, 1, i_d);
	task_res_idx_set(t_r, 2, i_d);

		/* Run shell */
	run();

		/* We don't need socket any more.
		 * Task shutdown should clear other descriptors (not implemented yet)
		 */
	close(*client_descr_p);
	*client_descr_p = -1;
	return NULL;
}

#else
	/* Generous version */
static void *telnet_thread_handler(void* args) {
	int *client_descr_p = (int *)args;
	struct task_resources *t_r = task_self_res();
	struct idx_desc *i_d = task_res_idx_get(t_r, *client_descr_p);
	struct socket *s= i_d->data;

		/* Bad idea. Closing one of those will lead to socket close. */
	struct idx_desc *r0 = task_idx_desc_alloc(TASK_IDX_TYPE_SOCKET, s);
	struct idx_desc *r1 = task_idx_desc_alloc(TASK_IDX_TYPE_SOCKET, s);
	struct idx_desc *r2 = task_idx_desc_alloc(TASK_IDX_TYPE_SOCKET, s);

	if (!(r0 && r1 && r2)) {
		out_msgs("Not enough resources\n", "Can't allocate resources\n",
				 "idx_allocate", *client_descr_p, NULL);
		/*
		 * Task shutdown should clear other descriptors (not implemented yet)
		 */
		close(*client_descr_p);
		*client_descr_p = -1;
		return NULL;
	}

		/* Redirect stdin, stdout, stderr to our socket
		 * ToDo:
		 * 	It's unsertain what we should do with the original descriptors
		 *	We can close them, but it may corrupt the original task
		 */
	task_res_idx_set(t_r, 0, r0);
	task_res_idx_set(t_r, 1, r1);
	task_res_idx_set(t_r, 2, r2);

		/* Run shell */
	run();

		/* We don't need socket any more.
		 * Task shutdown should clear other descriptors (not implemented yet)
		 */
	close(*client_descr_p);
	*client_descr_p = -1;
	return NULL;
}
#endif


static int exec(int argc, char **argv) {
	int res;

	struct sockaddr_in listening_socket;
	int listening_descr;

	clients[0] = -1;
	for (res = 1; res < TELNETD_MAX_CONNECTIONS; res++) {
		memcpy(&clients[res], &clients[0], sizeof(clients[0]));
	}

	listening_socket.sin_family = AF_INET;
	listening_socket.sin_port= htons(TELNETD_PORT);
	listening_socket.sin_addr.s_addr = htonl(TELNETD_ADDR);

	if (!((TELNETD_ADDR == INADDR_ANY) || ip_is_local(TELNETD_ADDR, false, false) )) {
		fatal_error("telnetd address is incorrect", TELNETD_ADDR);
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

	MD(printf("telnetd is ready to accept connections\n"));
	while (1) {
		struct sockaddr_in client_socket;
		int client_socket_len = sizeof(client_socket);
		int client_descr = accept(listening_descr, (struct sockaddr *)&client_socket,
								  &client_socket_len);

		if (client_descr < 0) {
			MD(printf("accept() failed. code=%d\n", client_descr));
		} else {
			uint i;

			MD(printf("Attempt to connect from address %s:%d",
					inet_ntoa(client_socket.sin_addr), ntohs(client_socket.sin_port)) );

			{
				/* Useless paranoidal check for descriptor allocation */
				struct task_resources *t_r = task_self_res();
				struct idx_desc *i_d = task_res_idx_get(t_r, client_descr);

				if (!i_d) {
					fatal_error("Socked descriptor doesn't belong to task resources", client_descr);
				}
			}


			for (i = 0; i < TELNETD_MAX_CONNECTIONS; i++) {
				if (clients[i] == -1) {
					struct thread th;
					struct thread *thds = &th;
					clients[i] = client_descr;

					/* Strange: depending of the options we might not inherit
					 * the descriptors. Why?
					 */
					if ((res = thread_create(&thds, THREAD_FLAG_PRIORITY_INHERIT | THREAD_FLAG_IN_NEW_TASK,
											 telnet_thread_handler, &clients[i]))) {
						out_msgs("Internal error with shell creation\n", " failed. Can't create shell\n",
								 "shell_create", client_descr, &client_socket);
						MD(printf("thread_create() returned with code=%d\n", res));
						close(client_descr);
						clients[i] = -1;
					} else {
						MD(printf(" success\n"));
					}
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

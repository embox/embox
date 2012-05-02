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
//	struct task_resources *t_r = task_self_res();
//	struct idx_desc *i_d = task_res_idx_get(t_r, *client_descr_p);

		/* Redirect stdin, stdout, stderr to our socket
		 * Unfortunately it's not working. We try to treat stdout as
		 * a socket, but we can't map 1 into a socket descriptor
		 * ToDo:
		 * 	It's unsertain what we should do with the original descriptors
		 *	We can close them, but it may corrupt the original task
		 */
#if 0
	We cant do that. See error below
	close(0);
	close(1);
	close(2);
	task_res_idx_set(t_r, 0, i_d);
	task_res_idx_set(t_r, 1, i_d);
	task_res_idx_set(t_r, 2, i_d);
-----------------------------
ASSERTION FAILED at src/mem/misc/pool.c : 39,
	in function pool_free:

obj
kernel debug panic

Stack:
39 pool_free() pool.c:40 0x00101078
38 net_buff_free() skbuff.c:52 0x0011c597
37 kfree_skb() skbuff.c:81 0x0011c609
36 alloc_skb() skbuff.c:115 0x0011c6ec
35 alloc_prep_skb() tcp.c:156 0x00123a34
34 tcp_v4_sendmsg() tcp.c:1297 0x001261d7
33 inet_sendmsg() af_inet.c:227 0x0011f977
32 kernel_socket_sendmsg() kernel_socket.c:444 0x0011e656
31 sendto_sock() socket.c:212 0x0011d221
30 sendto() socket.c:234 0x0011d2b7
29 this_write() socket.c:35 0x0011ce25
28 write() file.c:70 0x0010f17e
27 display_printchar() printf.c:50 0x00130184
26 prints() printf_impl.c:60 0x00130321
25 printi() printf_impl.c:116 0x00130493
24 __print() printf_impl.c:202 0x001307ad
23 vprintf() printf.c:55 0x001301b6
22 debug_print() tcp.c:129 0x00123806
21 packet_print() tcp.c:139 0x00123a0c
20 tcp_xmit() tcp.c:330 0x00124168
19 tcp_sock_xmit() tcp.c:382 0x00124366
18 process_ack() tcp.c:784 0x001251c9
17 pre_process() tcp.c:878 0x001254c2
16 tcp_handle() tcp.c:943 0x001255de
15 tcp_process() tcp.c:1004 0x001257fc
14 tcp_v4_rcv() tcp.c:1037 0x00125944
13 ip_rcv() ip_input.c:130 0x00120012
12 netif_receive_skb() network_layer.c:128 0x0011efbe
11 process_backlog() dev.c:225 0x0011f434
10 dev_rx_processing() dev.c:260 0x0011f4fa
9 net_rx_action() network_layer.c:139 0x0011eff5
8 softirq_wrap() softirq.c:17 0x00110801
7 softirq_dispatch() softirq.c:119 0x00128ed6
6 critical_dispatch_pending() critical.c:31 0x001292b7
5 irq_handler() irq_handler.c:37 0x001009db
4 irq_stub() entry.S:99 0x0012c81b
3 <symbol is not available> 0x001a7ee4
2 <symbol is not available> 0x00000010
1 <symbol is not available> 0x001a7f50
#endif

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

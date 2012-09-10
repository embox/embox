/**
 * @file
 * @brief Rlogin client
 * @date 10.09.2012
 * @author Alexander Kalmuk
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

#include <framework/mod/options.h>

#define RLOGIN_MAX_CONNECTIONS  OPTION_GET(NUMBER, max_conn_count)
static int connection_count = 0;

#define RLOGIN_ADDR INADDR_ANY
#define RLOGIN_PORT (unsigned int)513

	/* Allow to turn off/on extra debugging information */
#if 0
#	define RLOGIN_DEBUG(x) do {\
		x;\
	} while (0);
#else
#	define RLOGIN_DEBUG(x) do{\
	} while (0);
#endif

static int error = -1;

static void fatal_error(const char *msg, int code) {
#if 0	/* LOG_ERROR might be off now */
	LOG_ERROR("%s, code=%d, last errno=%d\n", msg, code, errno);
#else
	RLOGIN_DEBUG(printf("%s, code=%d, last errno=%d\n", msg, code, errno));
#endif
	thread_exit(&error);
}

	/* Shell thread for telnet */
static void *rlogin_thread_handler(void* args) {
	int *client_descr_p = (int *)args;

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	dup(*client_descr_p);
	dup(*client_descr_p);
	dup(*client_descr_p);

		/* Hack. Emulate future output, we need a char from user to exit from
		 * parameters mode
		 */
	{
		static const char* prompt = "embox>"; /* OPTION_STRING_GET(prompt); */
		printf("Welcome to rlogin!\n%s", prompt);
	}

	/* Run shell */
	shell_run();

	close(*client_descr_p);
	*client_descr_p = -1;

	return NULL;
}


static int exec(int argc, char **argv) {
	int res = -1, sockfd;
	struct sockaddr_in our, dst;

	if (connection_count >= RLOGIN_MAX_CONNECTIONS)
		return -1;

	if (!inet_aton(argv[argc -1], &dst.sin_addr)) {
		printf("Invalid ip address %s\n", argv[argc -1]);
		return -ENOENT;
	}

	our.sin_family = AF_INET;
	our.sin_port= htons(RLOGIN_PORT);
	our.sin_addr.s_addr = htonl(RLOGIN_ADDR);

	if (!((RLOGIN_ADDR == INADDR_ANY) || ip_is_local(RLOGIN_ADDR, false, false))) {
		fatal_error("rlogin address is incorrect", RLOGIN_ADDR);
		return -1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fatal_error("can't create socket", sockfd);
		return -1;
	}

	if ((res = bind(sockfd, (struct sockaddr *)&our, sizeof(our))) < 0) {
		fatal_error("bind() failed", res);
		goto exit;
	}

	dst.sin_family = AF_INET;
	dst.sin_port = htons(RLOGIN_PORT);

	if (connect(sockfd, (struct sockaddr *)&dst, sizeof dst) < 0) {
		printf("Error... Cant connect to remote address %s:%d\n",
				inet_ntoa(dst.sin_addr), RLOGIN_PORT);
		close(sockfd);
		return -1;
	}

	connection_count++;

	RLOGIN_DEBUG(printf("connected\n"));

	if ((res = new_task(rlogin_thread_handler, &sockfd)))
		RLOGIN_DEBUG(printf("new_task() returned with code=%d\n", res));

	res = ENOERR;

exit:
	close(sockfd);
	return res;
}

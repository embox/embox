/**
 * @file
 * @brief Rlogin client
 * @date 10.09.2012
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <util/ring_buff.h>

#include <embox/cmd.h>

EMBOX_CMD(exec);

#define RLOGIN_ADDR INADDR_ANY

/* rlogin port */
#define RLOGIN_PORT 513

/* control bytes */
#define R_DISCARD_All 0x02
#define R_RAW         0x10
#define R_RESUME      0x20
#define R_WINDOW_SZ   0x80

/* start, stop output on local terminal */
#define R_START       0x11
#define R_STOP        0x13

/* client mode */
#define MODE_COOKED   0
#define MODE_RAW      R_RAW

#define RCVBUFFER_SIZE 256

/* FIXME cheat for now */
static const char *client = "embox";
static const char *term = "dumb/38000";

static void print_usage(void) {
	printf("Usage: rlogin [-l username] <server> \n");
}

	/* Allow to turn off/on extra debugging information */
#if 0
#	define RLOGIN_DEBUG(x) do {\
		x;\
	} while (0);
#else
#	define RLOGIN_DEBUG(x) do{\
	} while (0);
#endif

/**
 * @brief handle code if it is control byte
 * @return result if @c code is control byte
 * @retval -1 if not
 * @retval 0 if it is
 */
static int handle_cntl_byte(unsigned char code, int *state, int *mode) {
	switch (code) {
	case R_DISCARD_All:
		*state = R_STOP;
		break;
	case R_RAW:
		*mode = MODE_RAW;
		break;
	case R_RESUME:
		if (*mode == MODE_RAW) {
			*state = R_START;
			*mode = MODE_COOKED;
			break;
		}
		return -EINVAL;
	case R_WINDOW_SZ:
		break;
	case R_START:
		if (*mode != MODE_RAW) {
			*state = R_START;
			break;
		}
		return -EINVAL;
	case R_STOP:
		if (*mode != MODE_RAW) {
			*state = R_STOP;
		} else {
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static size_t rlogin_write(int fd, struct ring_buff *rbuf, size_t size) {
	unsigned char b[RCVBUFFER_SIZE];
	size_t res;

	ring_buff_dequeue(rbuf, b, size);
	res = write(fd, b, size);
	ring_buff_enqueue(rbuf, (b+res), size - res);

	return res;
}

static int rlogin_handle(int sock) {
	fd_set readfds, writefds;
	/*unsigned char c;*/
	unsigned char sock_storage[RCVBUFFER_SIZE], stdin_storage[RCVBUFFER_SIZE];
	struct ring_buff sock_buf, stdin_buf;
	int state = R_START, mode = MODE_COOKED;
	struct termios tios;
	int c_lflags;
	int err = 0;

	tcgetattr(STDIN_FILENO, &tios);
	c_lflags = tios.c_lflag;
	tios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &tios);

	ring_buff_init(&stdin_buf, 1, RCVBUFFER_SIZE, stdin_storage);
	ring_buff_init(&sock_buf, 1, RCVBUFFER_SIZE, sock_storage);

	while (1) {
		int sock_data_len, stdin_data_len;

		stdin_data_len = ring_buff_get_cnt(&stdin_buf);
		sock_data_len = ring_buff_get_cnt(&sock_buf);

		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		FD_SET(sock, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
		if (stdin_data_len > 0) {
			FD_SET(sock, &writefds);
		}
		if (sock_data_len > 0) {
			FD_SET(STDOUT_FILENO, &writefds);
		}

		select(sock + 1, &readfds, &writefds, NULL, NULL);

		if (FD_ISSET(sock, &writefds)) {
			if ((err = rlogin_write(sock, &stdin_buf, stdin_data_len)) < 0) {
				/*goto reset_out;*/
			}
		}

		if (FD_ISSET(STDOUT_FILENO, &writefds)) {
			if ((err = rlogin_write(STDOUT_FILENO, &sock_buf, sock_data_len)) < 0) {
				/*goto reset_out;*/
			}
		}

		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			char in_buf[4];
			if ((err = read(STDIN_FILENO, in_buf, 4)) < 0) {
				/*goto reset_out;*/
			}
#if 1
			for (int i = 0; i < err; i++) {
			/* XXX NVT terminal requires '\r' as enter */
				if (in_buf[i] == '\n') {
					in_buf[i] = '\r';
				}
			}
#endif
			ring_buff_enqueue(&stdin_buf, in_buf, err);
		}

		if (FD_ISSET(sock, &readfds)) {
			unsigned char buf[RCVBUFFER_SIZE];
			int cur = 0;

			if ((err = read(sock, buf, ring_buff_get_space(&sock_buf))) < 0) {
				/*goto reset_out;*/
			}

			do {
				if (handle_cntl_byte(buf[cur], &state, &mode) && state != R_STOP) {
					ring_buff_enqueue(&sock_buf, &buf[cur], 1);
				}
			} while(++cur != err);
		}
	} /* while (1) */

/*reset_out:*/
	tios.c_lflag = c_lflags;
	tcsetattr(STDIN_FILENO, TCSANOW, &tios);

	return err;
}

static int exec(int argc, char **argv) {
	static int tries = 0;
	int res = -1, sock, opt;
	struct sockaddr_in our, dst;
	int len;
	char *buf;
	char *server = (char*)client;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hl:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'l':
			server = optarg;
			break;
		default:
			printf("error: unsupported option %c\n", optopt);
			return -EINVAL;
		}
	}

	if (!inet_aton(argv[argc -1], &dst.sin_addr)) {
		printf("Invalid ip address %s\n", argv[argc -1]);
		return -EINVAL;
	}
	dst.sin_family = AF_INET;
	dst.sin_port = htons(RLOGIN_PORT);

	len = strlen(client) + strlen(server) + strlen(term) + 3;
	buf = malloc(len);
	if (buf == NULL) {
		return -ENOMEM;
	}
	memset(buf, 0, len);

	our.sin_family = AF_INET;
	our.sin_port= htons(RLOGIN_PORT + tries++);
	our.sin_addr.s_addr = htonl(RLOGIN_ADDR);

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("can not allocate socket\n");
		return -errno;
	}

	if ((res = bind(sock, (struct sockaddr *)&our, sizeof(our))) < 0) {
		printf("can not bind socket\n");
		goto exit;
	}

	if (connect(sock, (struct sockaddr *)&dst, sizeof dst) < 0) {
		printf("Error... Cant connect to remote address %s:%d\n",
				inet_ntoa(dst.sin_addr), RLOGIN_PORT);
		res = -errno;
		goto exit;
	}
	RLOGIN_DEBUG(printf("connected\n"));

	/* send Handshake */
	if (write(sock, buf, 1) < 0) {
		res = -errno;
		goto exit;
	}

	len = 0;
	memcpy(buf, client, strlen(client));
	len += strlen(client) + 1;

	memcpy(buf + len, server, strlen(server));
	len += strlen(server) + 1;

	memcpy(buf + len, term, strlen(term));
	len += strlen(term) + 1;
	/* send user info */
	if (write(sock, buf, len) < 0) {
		res = -errno;
		goto exit;
	}

	res = rlogin_handle(sock);

exit:
	free(buf);
	close(sock);
	return res;
}

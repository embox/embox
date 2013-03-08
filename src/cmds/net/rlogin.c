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
#include <unistd.h>

#include <net/ip.h>
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

/* FIXME cheat for now */
static const char *client = "embox";
static const char *term = "xterm/38000";

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

static void rlogin_handle(int sock) {
	int sock_data_len = 0, stdin_data_len = 0;
	unsigned char c;
	unsigned char buf[128];
	unsigned char *s = buf;
	int state = R_START, mode = MODE_COOKED;

	ioctl(STDIN_FILENO, O_NONBLOCK, 1);
	fcntl(sock, F_SETFD, O_NONBLOCK);

	while (1) {
		if (stdin_data_len > 0) {
			if (write(sock, &c, 1) == 1) {
				stdin_data_len -= 1;
			}
		} else {
			stdin_data_len = read(STDIN_FILENO, &c, 1);
			/* XXX NVT terminal requires '\r' as enter */
			if ( c == '\n') {
				c = '\r';
			}
		}

		if (sock_data_len > 0) {
			bool handeled;
			/* Try to handle each byte of server data. */
			do {
				handeled = false;
				if (!handle_cntl_byte(*s, &state, &mode) || state == R_STOP
						|| (write(STDOUT_FILENO, s, 1) == 1)) {
					sock_data_len -= 1;
					s += 1;
					handeled = true;
				}
			} while (handeled && sock_data_len > 0);
		} else {
			s = buf;
			sock_data_len = read(sock, s, 128);
		}
	} /* while (1) */
}

static int exec(int argc, char **argv) {
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
		return -ENOENT;
	}

	our.sin_family = AF_INET;
	our.sin_port= htons(RLOGIN_PORT);
	our.sin_addr.s_addr = htonl(RLOGIN_ADDR);

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		printf("can not allocate socket\n");
		return -ENOMEM;
	}

	len = strlen(client) + strlen(server) + strlen(term) + 3;
	buf = malloc(len);
	memset(buf, 0, len);

	if ((res = bind(sock, (struct sockaddr *)&our, sizeof(our))) < 0) {
		printf("can not bind socket\n");
		goto exit;
	}

	dst.sin_family = AF_INET;
	dst.sin_port = htons(RLOGIN_PORT);

	if (connect(sock, (struct sockaddr *)&dst, sizeof dst) < 0) {
		printf("Error... Cant connect to remote address %s:%d\n",
				inet_ntoa(dst.sin_addr), RLOGIN_PORT);
		goto exit;
	}
	RLOGIN_DEBUG(printf("connected\n"));

	/* send Handshake */
	if (write(sock, buf, 1) < 0) {
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
		goto exit;
	}

	res = ENOERR;

	rlogin_handle(sock);

exit:
	res = -errno;
	free(buf);
	close(sock);
	return res;
}

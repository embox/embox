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

#include <lib/libds/ring_buff.h>

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
#define RLOGIN_USERBUF_LEN 128

#define RLOGIN_DEFAULT_CLIENT "embox"
#define RLOGIN_TERM_SPEC "dumb/38000"

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

struct rlogin_state {
	int state;
	int mode;
};

/**
 * @brief handle code if it is control byte
 * @return result if @c code is control byte
 * @retval -1 if not
 * @retval 0 if it is
 */
static int handle_cntl_byte(unsigned char code, struct rlogin_state *rs) {
	switch (code) {
	case R_DISCARD_All:
		rs->state = R_STOP;
		break;
	case R_RAW:
		rs->mode = MODE_RAW;
		break;
	case R_RESUME:
		if (rs->mode == MODE_RAW) {
			rs->state = R_START;
			rs->mode = MODE_COOKED;
			break;
		}
		return -EINVAL;
	case R_WINDOW_SZ:
		break;
	case R_START:
		if (rs->mode != MODE_RAW) {
			rs->state = R_START;
			break;
		}
		return -EINVAL;
	case R_STOP:
		if (rs->mode != MODE_RAW) {
			rs->state = R_STOP;
		} else {
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int rlogin_handle_cntl(unsigned char *bufout, unsigned char *bufin, int blen,
		struct rlogin_state *rs) {
	unsigned char *bop = bufout;
	unsigned char *bip = bufin;

	while (bip < bufin + blen) {
		unsigned char c = *bip++;
		if (handle_cntl_byte(c, rs) && rs->state != R_STOP) {
			*bop++ = c;
		}
	}
	return bop - bufout;
}

/* XXX NVT terminal requires '\r' as enter */
static void rlogin_fix_nr(unsigned char *buf, int blen) {
	for (int i = 0; i < blen; i++) {
		if (buf[i] == '\n') {
			buf[i] = '\r';
		}
	}
}

static int rlogin_write(int fd, unsigned char *buf, int *size) {
	int nwrite;
	assert(*size > 0);
	nwrite = write(fd, buf, *size);
	if (0 < nwrite) {
		*size -= nwrite;
		memmove(buf, buf + nwrite, *size);
	}
	return nwrite;
}

static int rlogin_handle(int sock) {
	unsigned char io2net[RCVBUFFER_SIZE], net2io[RCVBUFFER_SIZE];
	int io2net_blen, net2io_blen;
	fd_set readfds, writefds;
	struct termios tios;
	struct rlogin_state rs;
	int c_lflags;

	rs.state = R_START;
	rs.mode = MODE_COOKED;

	tcgetattr(STDIN_FILENO, &tios);
	c_lflags = tios.c_lflag;
	tios.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &tios);

	io2net_blen = net2io_blen = 0;
	while (1) {
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		if (io2net_blen > 0) {
			FD_SET(sock, &writefds);
		} else {
			FD_SET(STDIN_FILENO, &readfds);
		}

		if (net2io_blen > 0) {
			FD_SET(STDOUT_FILENO, &writefds);
		} else {
			FD_SET(sock, &readfds);
		}

		if (0 >= select(sock + 1, &readfds, &writefds, NULL, NULL)) {
			continue;
		}

		if (FD_ISSET(sock, &writefds)) {
			assert(io2net_blen > 0);
			if (0 > rlogin_write(sock, io2net, &io2net_blen)) {
				break;
			}
		}

		if (FD_ISSET(STDOUT_FILENO, &writefds)) {
			assert(net2io_blen > 0);
			rlogin_write(STDOUT_FILENO, net2io, &net2io_blen);
		}

		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			assert(io2net_blen == 0);
			io2net_blen = read(STDIN_FILENO, io2net, sizeof(io2net));
			if (io2net_blen > 0) {
				rlogin_fix_nr(io2net, io2net_blen);
			}
		}

		if (FD_ISSET(sock, &readfds)) {
			assert(net2io_blen == 0);
			net2io_blen = read(sock, net2io, sizeof(net2io));

			if (net2io_blen <= 0) {
				break;
			}

			net2io_blen = rlogin_handle_cntl(net2io, net2io, net2io_blen, &rs);
		}
	}

	tios.c_lflag = c_lflags;
	tcsetattr(STDIN_FILENO, TCSANOW, &tios);

	return 0;
}

static int rlogin_send_user(int sock, const char *user) {
	char buf[RLOGIN_USERBUF_LEN];
	const int userlen = strlen(user);
	int res;

	strncpy(buf, user, RLOGIN_USERBUF_LEN);
	strncpy(buf + userlen + 1, user, RLOGIN_USERBUF_LEN - (userlen + 1));
	strncpy(buf + 2 * (userlen + 1), RLOGIN_TERM_SPEC, RLOGIN_USERBUF_LEN - 2 * (userlen + 1));
	if (write(sock, buf, 2 * (userlen + 1) + strlen(RLOGIN_TERM_SPEC) + 1) < 0) {
		res = -errno;
	} else {
		res = 0;
	}
	return res;
}

int main(int argc, char **argv) {
	static int tries = 0;
	int res = -1, sock, opt;
	struct sockaddr_in our, dst;
	const char *user = RLOGIN_DEFAULT_CLIENT;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "hl:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'l':
			user = optarg;
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
	if (write(sock, "", 1) < 0) {
		res = -errno;
		goto exit;
	}

	if (0 > (res = rlogin_send_user(sock, user))) {
		goto exit;
	}

	res = rlogin_handle(sock);
exit:
	close(sock);
	return res;
}

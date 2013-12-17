/**
 * @file
 * @brief Easy TFTP client
 * @details RFC 1350
 *
 * @date 12.03.10
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <embox/cmd.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <net/l3/ipv4/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stddef.h>

EMBOX_CMD(exec);

/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define SEGSIZE 512       /* data segment size */
#define PKTSIZE SEGSIZE+4 /* full packet size */

#define TFTP_TRANSFER_PORT 69 /* default well known port */

/*
 * Packet types.
 */
#define RRQ     1	/* read request */
#define WRQ     2	/* write request */
#define DATA    3	/* data packet */
#define ACK     4	/* acknowledgement */
#define ERROR   5	/* error code */
#define OACK    6	/* options acknowledgment */

struct tftp_msg {
	__be16 opcode;
	union {
		struct {
			char name_and_mode[2];
		} cmd /*__attribute__ ((packed))*/;
		struct {
			__be16 block_num;
			char stuff[SEGSIZE];
		} data /*__attribute__ ((packed))*/;
		struct {
			__be16 block_num;
		} ack /*__attribute__ ((packed))*/;
		struct {
			__be16 error_code;
			char error_msg[1];
		} err /*__attriibute__ ((packed))*/;
	} op /*__attribute__ ((packed))*/;
} __attribute__ ((packed));

/*
 * Errors
 */

/* These initial 7 are passed across the net in "ERROR" packets. */
#define	TFTP_EUNDEF      0	/* not defined */
#define	TFTP_ENOTFOUND   1	/* file not found */
#define	TFTP_EACCESS     2	/* access violation */
#define	TFTP_ENOSPACE    3	/* disk full or allocation exceeded */
#define	TFTP_EBADOP      4	/* illegal TFTP operation */
#define	TFTP_EBADID      5	/* unknown transfer ID */
#define	TFTP_EEXISTS     6	/* file already exists */
#define	TFTP_ENOUSER     7	/* no such user */
/* These extensions are return codes in our API, *never* passed on the net. */
#define TFTP_TIMEOUT     8	/* operation timed out */
#define TFTP_NETERR      9	/* some sort of network error */
#define TFTP_INVALID    10	/* invalid parameter */
#define TFTP_PROTOCOL   11	/* protocol violation */
#define TFTP_TOOLARGE   12	/* file is larger than buffer */

static int open_socket(int *out_sock, struct sockaddr *sa,
		socklen_t salen) {
	int ret;

	ret = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ret == -1) {
		perror("tftp: socket() failure");
		return -errno;
	}

	if (-1 == connect(ret, sa, salen)) {
		perror("tftp: connect() failure");
		close(ret);
		return -errno;
	}

	*out_sock = ret;

	return 0;
}

static int close_socket(int sock) {
	if (close(sock) < 0) {
		fprintf(stderr, "Can't close socket\n");
		return -errno;
	}

	return 0;
}

static int make_remote_addr(char *hostname,
		struct sockaddr *out_raddr, socklen_t *out_raddr_len) {
	int ret;
	struct sockaddr_in *raddr_in;

	raddr_in = (struct sockaddr_in *)out_raddr;
	memset(out_raddr, 0, sizeof *out_raddr);
	raddr_in->sin_family = AF_INET;
	raddr_in->sin_port = htons(TFTP_TRANSFER_PORT);
	ret = inet_aton(hostname, &raddr_in->sin_addr);
	if (!ret) {
		fprintf(stderr, "Can't parse remote address '%s`\n", hostname);
		return -EINVAL;
	}

	*out_raddr_len = sizeof *raddr_in;

	return 0;
}

static char *get_file_mode_r(char binary_on) {
	return binary_on ? "rb" : "r";
}

static char *get_file_mode_w(char binary_on) {
	return binary_on ? "wb" : "w";
}

static char *get_transfer_mode(char binary_on) {
	return binary_on ? "octet" : "netascii";
}

static int open_file(char *filename, char *mode, FILE **out_fp) {
	FILE *fp;

	fp = fopen(filename, mode);
	if (fp == NULL) {
		fprintf(stderr, "Can't open file '%s` with mode \"%s\"\n", filename, mode);
		return -errno;
	}

	*out_fp = fp;

	return 0;
}

static int close_file(FILE *fp) {
	if (fclose(fp) < 0) {
		fprintf(stderr, "Can't close file\n");
		return -errno;
	}

	return 0;
}

static int read_file(FILE *fp, char *buff, size_t buff_sz, size_t *out_bytes) {
	int ret;

	ret = fread(buff, 1, buff_sz, fp);
	if (ret < 0) {
		fprintf(stderr, "Can't read data from file\n");
		return -errno;
	}

	*out_bytes = (size_t)ret;

	return 0;
}

static int write_file(FILE *fp, char *data, size_t data_sz) {
	int ret;

	ret = fwrite(data, 1, data_sz, fp);
	if ((ret < 0) || ((size_t)ret != data_sz)) {
		fprintf(stderr, "Can't write data to file\n");
		return -errno;
	}

	return 0;
}

static int tftp_build_msg_cmd(struct tftp_msg *msg, size_t *msg_len,
		uint16_t type, char *filename, char *mode) {
	char *ptr;
	size_t sz;

	msg->opcode = htons(type);
	*msg_len = sizeof msg->opcode;

	ptr = &msg->op.cmd.name_and_mode[0];
	sz = strlen(filename) + 1;
	memcpy(ptr, filename, sz * sizeof(char));
	*msg_len += sz * sizeof(char);

	ptr += sz;
	sz = strlen(mode) + 1;
	memcpy(ptr, mode, sz * sizeof(char));
	*msg_len += sz * sizeof(char);

	return 0;
}

static int tftp_build_msg_data(struct tftp_msg *msg, size_t *msg_len,
		FILE *fp, uint16_t block_num) {
	int ret;
	size_t bytes = 0;

	msg->opcode = htons(DATA);
	*msg_len = sizeof msg->opcode;

	msg->op.data.block_num = htons(block_num);
	*msg_len += sizeof msg->op.data.block_num;

	ret = read_file(fp, &msg->op.data.stuff[0], sizeof msg->op.data.stuff, &bytes);
	if (ret != 0) return ret;
	*msg_len += bytes;

	return 0;
}

static int tftp_build_msg_ack(struct tftp_msg *msg, size_t *msg_len, uint16_t block_num) {
	msg->opcode = htons(ACK);
	*msg_len = sizeof msg->opcode;

	msg->op.ack.block_num = htons(block_num);
	*msg_len += sizeof msg->op.ack.block_num;

	return 0;
}

static int msg_with_correct_len(struct tftp_msg *msg, size_t msg_len) {
	size_t field_sz, left_sz;
	char *tmp;

	left_sz = msg_len;

	field_sz = sizeof msg->opcode;
	if (left_sz < field_sz) return 0;

	left_sz -= field_sz;

	switch (ntohs(msg->opcode)) {
	case RRQ:
	case WRQ:
		tmp = &msg->op.cmd.name_and_mode[0];
		/* filename */
		do
			if (left_sz-- == 0) return 0;
		while (*tmp++ != '\0');
		/* mode */
		do
			if (left_sz-- == 0) return 0;
		while (*tmp++ != '\0');
		break;
	case DATA:
		/* block number */
		field_sz = sizeof msg->op.data.block_num;
		if (left_sz < field_sz) return 0;
		left_sz -= field_sz;
		/* data */
		left_sz = 0;
		break;
	case ACK:
		/* block number */
		field_sz = sizeof msg->op.ack.block_num;
		if (left_sz < field_sz) return 0;
		left_sz -= field_sz;
		break;
	case ERROR:
		/* error code */
		field_sz = sizeof msg->op.err.error_code;
		if (left_sz < field_sz) return 0;
		left_sz -= field_sz;
		/* error message */
		tmp = &msg->op.err.error_msg[0];
		do
			if (left_sz-- == 0) return 0;
		while (*tmp++ != '\0');
		break;
	default: /* unknown operation */
		return 0;
	}

	return !left_sz;
}

static int tftp_msg_send(struct tftp_msg *msg, size_t msg_len,
		int sock) {
	assert(msg_with_correct_len(msg, msg_len)); /* debug msg_with_correct_len */

	if (-1 == send(sock, (char *)msg, msg_len, 0)) {
		perror("tftp: send() failure");
		return -errno;
	}

	return 0;
}

static int tftp_msg_recv(struct tftp_msg *msg, size_t *msg_len,
		int sock) {
	ssize_t ret;

	assert(msg_len != NULL);

	ret = recv(sock, (char *)msg, sizeof *msg, 0);
	if (ret == -1) {
		perror("tftp: recv() failure");
		return -errno;
	}

	*msg_len = ret;

	return 0;
}

static int tftp_send_file(char *filename, char *hostname, char binary_on) {
	int ret, sock;
	struct sockaddr_storage remote_addr;
	socklen_t remote_addr_len;
	FILE *fp = NULL;
	struct tftp_msg snd, rcv;
	size_t snd_len, rcv_len;
	uint16_t pkg_number;

	ret = make_remote_addr(hostname,
			(struct sockaddr *)&remote_addr, &remote_addr_len);
	if (ret != 0) { sock = -1; fp = NULL; goto error; }

	ret = open_socket(&sock, (struct sockaddr *)&remote_addr,
			remote_addr_len);
	if (ret != 0) { sock = -1; fp = NULL; goto error; }

	ret = open_file(filename, get_file_mode_r(binary_on), &fp);
	if (ret != 0) { fp = NULL; goto error; }

	pkg_number = 0;

	ret = tftp_build_msg_cmd(&snd, &snd_len, WRQ, filename, get_transfer_mode(binary_on));
	if (ret != 0) goto error;

	/* Send Write Request */
	goto send_msg;

	while (1) {
		/* receive reply */
		ret = tftp_msg_recv(&rcv, &rcv_len, sock);
		if (ret != 0) goto error;

		/* check message length */
		if (!msg_with_correct_len(&rcv, rcv_len)) goto send_msg; /* bad packet, send again */

		/* handling of the reply msg */
		switch (ntohs(rcv.opcode)) {
		case ACK:
			if (ntohs(rcv.op.ack.block_num) != pkg_number) {
				goto send_msg; /* invalid acknowledgement, send again */
			}
			break;
		case ERROR:
			fprintf(stderr, "%s: error: code=%d, msg='%s`\n",
					hostname, (int)ntohs(rcv.op.err.error_code), &rcv.op.err.error_msg[0]);
			goto error;
		default:
			goto send_msg;
		}

		/* whether we have more data to transfer? */
		if ((pkg_number != 0) && (snd_len != sizeof snd)) {
			break; /* no more data */
		}

		/* get next stuff of data */
		ret = tftp_build_msg_data(&snd, &snd_len, fp, ++pkg_number);
		if (ret != 0) goto error; /* TODO send error package */

send_msg:
		/* send request / data */
		ret = tftp_msg_send(&snd, snd_len, sock);
		if (ret != 0) goto error;
	}

	ret = close_file(fp);
	if (ret != 0) { fp = NULL; goto error; }

	ret = close_socket(sock);
	if (ret != 0) { sock = -1; fp = NULL; goto error; }

	fprintf(stdout, "File '%s` was transferred\n", filename);

	return 0;

error:
	if (sock >= 0) close(sock);
	if (fp) fclose(fp);
	return ret;
}

static int tftp_recv_file(char *filename, char *hostname, char binary_on) {
	int ret, sock;
	struct sockaddr_storage remote_addr;
	socklen_t remote_addr_len;
	FILE *fp = NULL;
	struct tftp_msg snd, rcv;
	size_t snd_len, rcv_len, data_len;
	uint16_t pkg_number;

	ret = make_remote_addr(hostname,
			(struct sockaddr *)&remote_addr, &remote_addr_len);
	if (ret != 0) { sock = -1; fp = NULL; goto error; }

	ret = open_socket(&sock, (struct sockaddr *)&remote_addr,
			remote_addr_len);
	if (ret != 0) { sock = -1; fp = NULL; goto error; }

	ret = open_file(filename, get_file_mode_w(binary_on), &fp);
	if (ret != 0) { fp = NULL; goto error; }

	pkg_number = 0;

	ret = tftp_build_msg_cmd(&snd, &snd_len, RRQ, filename, get_transfer_mode(binary_on));
	if (ret != 0) goto error;

	/* Send Write Request */
	goto send_msg;

	do {
		/* receive reply */
		ret = tftp_msg_recv(&rcv, &rcv_len, sock);
		if (ret != 0) goto error;

		/* check message length */
		if (!msg_with_correct_len(&rcv, rcv_len)) goto send_msg; /* bad packet, send again */

		/* handling of the reply msg */
		switch (ntohs(rcv.opcode)) {
		case DATA:
			if (ntohs(rcv.op.ack.block_num) != pkg_number + 1) {
				goto send_msg; /* invalid data package, send again */
			}
			/* save data */
			data_len = rcv_len - (sizeof rcv - sizeof rcv.op.data.stuff);
			ret = write_file(fp, &rcv.op.data.stuff[0], data_len);
			if (ret != 0) goto error;
			break;
		case ERROR:
			fprintf(stderr, "%s: error: code=%d, msg='%s`\n",
				hostname, (int)ntohs(rcv.op.err.error_code), &rcv.op.err.error_msg[0]);
			goto error;
		default:
			goto send_msg;
		}

		/* send ack */
		ret = tftp_build_msg_ack(&snd, &snd_len, ++pkg_number);
		if (ret != 0) goto error;

send_msg:
		/* send request / ack */
		ret = tftp_msg_send(&snd, snd_len, sock);
		if (ret != 0) goto error;

		/* whether we get more data? */
	} while ((pkg_number == 0) || (rcv_len == sizeof rcv));

	ret = close_file(fp);
	if (ret != 0) {
		fp = NULL;
		goto error;
	}

	ret = close_socket(sock);
	if (ret != 0) {
		sock = -1;
		fp = NULL;
		goto error;
	}

	fprintf(stdout, "File '%s` was transferred\n", filename);

	return 0;

error:
	if (sock >= 0) close(sock);
	if (fp) fclose(fp);
	return ret;
}

static int exec(int argc, char **argv) {
	int ret, i;
	char param_ascii, param_binary, param_get, param_put;
	int (*file_hnd)(char *, char *, char);

	/* Initialize objects */
	param_ascii = param_binary = param_get = param_put = 0;
	getopt_init();

	/* Get options */
	while ((ret = getopt(argc, argv, "habgp")) != -1) {
		switch (ret) {
		default:
		case '?':
			fprintf(stderr, "%s: error: unrecognized option '%c`\n", argv[0], (char)optopt);
			fprintf(stderr, "Try -h for more information\n");
			return -EINVAL;
		case 'h':
			fprintf(stdout, "Usage: %s [-hab] -[g|p] files destination\n", argv[0]);
			return 0;
		case 'a':
		case 'b':
			if (param_ascii || param_binary) {
				fprintf(stderr, "%s: error: already using %s mode to transfer files\n",
						argv[0], get_transfer_mode(!param_ascii));
				return -EINVAL;
			}
			*(ret == 'a' ? &param_ascii : &param_binary) = 1;
			break;
		case 'g':
		case 'p':
			if (param_get || param_put) {
				fprintf(stderr, "%s: error: %s mode already was selected\n",
						argv[0], param_get ? "forwarding" : "receiving");
				return -EINVAL;
			}
			*(ret == 'g' ? &param_get : &param_put) = 1;
			break;
		}
	}

	/* Check transfering mode options */
	if (!param_ascii && !param_binary) {
		param_binary = 1; /* default mode */
	}

	/* Check action flags */
	if (!param_get && !param_put) {
		fprintf(stderr, "%s: error: please specify action on files\n", argv[0]);
		return -EINVAL;
	}

	/* Check presence of files names and address of remote machine */
	if (argc - (--optind) < 2) {
		fprintf(stderr, "%s: erorr: please specify at least one file and address of remote host\n",
				argv[0]);
		return -EINVAL;
	}

	/* Handling */
	file_hnd = param_get ? &tftp_recv_file : &tftp_send_file;
	for (i = optind; i < argc - 1; ++i) {
		ret = (*file_hnd)(argv[i], argv[argc - 1], param_binary);
		if (ret != 0) {
			fprintf(stderr, "%s: error: error occured when handled file '%s`\n",
					argv[0], argv[i]);
			return ret;
		}
	}

	return 0;
}

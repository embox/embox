/**
 * @file
 * @brief Simple TFTP client
 * @details RFC 1350
 *
 * @date 12.03.10
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lib/tftp.h>

static char *get_file_mode_r(char binary_on) {
	return binary_on ? "rb" : "r";
}

static char *get_file_mode_w(char binary_on) {
	return binary_on ? "wb" : "w";
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
	if (ferror(fp) || ret < 0) {
		fprintf(stderr, "Can't read data from file\n");
		return -1;
	}

	*out_bytes = (size_t)ret;

	return 0;
}

static int tftp_send_file(char *filename, char *out_file, char *hostname,
								char binary_on, void *addr) {
	struct tftp_stream *s;
	FILE *fp = NULL;
	size_t bytes = 0;
	int ret = 0;
	uint8_t buf[TFTP_SEGSIZE];

	s = tftp_new_stream(hostname, filename, TFTP_DIR_PUT, (bool) binary_on);

	if (0 != open_file(filename, get_file_mode_r(binary_on), &fp)) {
		tftp_delete_stream(s);
		return -1;
	}

	while (1) {
		if (0 != read_file(fp, (char *) buf, sizeof(buf), &bytes)) {
			tftp_delete_stream(s);
			return -1;
		}

		if (bytes == 0) {
			/* End of file */
			break;
		}

		if (0 > (ret = tftp_stream_write(s, buf, bytes))) {
			fprintf(stderr, "%s: error: code=%d, msg='%s`\n",
					hostname, -ret, tftp_error(s));
			tftp_delete_stream(s);
			return ret;

		}
	}

	tftp_delete_stream(s);

	return 0;
}

static int tftp_recv_file(char *filename, char *out_file, char *hostname,
							char binary_on, void *addr) {
	struct tftp_stream *s;	
	FILE *fp = NULL;
	int bytes;
	uint8_t buf[TFTP_SEGSIZE];

	s = tftp_new_stream(hostname, filename, TFTP_DIR_GET, (bool) binary_on);

	if (addr == NULL) {
		if (out_file == NULL) {
			out_file = filename;
		}
		if (0 != open_file(out_file, get_file_mode_w(binary_on), &fp)) {
			tftp_delete_stream(s);
			return -1;
		}
	}

	while (1) {
		bytes = tftp_stream_read(s, buf);

		if (bytes < 0) {
			fprintf(stderr, "%s: error: code=%d, msg='%s`\n",
					hostname, -bytes, tftp_error(s));
			tftp_delete_stream(s);
			return bytes;
		}

		if (bytes == 0) {
			/* End of file */
			break;
		}

		if (addr == NULL) {
			if (bytes > fwrite(buf, 1, bytes, fp)) {
				tftp_delete_stream(s);
				return -2;
			}
		} else {
			memcpy(addr, buf, bytes);
			addr += bytes;
		}
	}

	if (addr == NULL) {
		close_file(fp);
	}

	tftp_delete_stream(s);
	return 0;
}

int main(int argc, char **argv) {
	int ret, i;
	char param_ascii, param_binary, param_get, param_put;
	int (*file_hnd)(char *, char*, char *, char, void *);
	void *addr = NULL;
	char *out_file = NULL;

	/* Initialize objects */
	param_ascii = param_binary = param_get = param_put = 0;

	/* Get options */
	while ((ret = getopt(argc, argv, "habgpm:o:")) != -1) {
		switch (ret) {
		default:
		case '?':
			fprintf(stderr, "%s: error: unrecognized option '%c`\n", argv[0], (char)optopt);
			fprintf(stderr, "Try -h for more information\n");
			return -EINVAL;
		case 'h':
			fprintf(stdout, "Usage: %s [-hab] -[g [-m addr] [-o output file] | p] files destination\n", argv[0]);
			return 0;
		case 'a':
		case 'b':
			if (param_ascii || param_binary) {
				fprintf(stderr, "%s: error: already using %s mode to transfer files\n",
						argv[0], param_ascii ? "netascii" : "binary");
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
		case 'm':
			addr = (void *) strtol(optarg, NULL, 0);
			break;
		case 'o':
			out_file = optarg;
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
	for (i = optind + 1; i < argc - 1; ++i) {
		ret = (*file_hnd)(argv[i], out_file, argv[argc - 1], param_binary, addr);
		if (ret != 0) {
			fprintf(stderr, "%s: error: error occured when handled file '%s`\n",
					argv[0], argv[i]);
			return ret;
		}
	}

	return 0;
}

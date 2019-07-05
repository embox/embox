/**
 * @file
 * @brief Filter adjacent matching lines from FILE, writing to standard output.
 *
 * @date 02.07.19
 * @author Nastya Nizharadze
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#define NL_AND_NUL 2

#define FLAG_DUPLICATES (1 << 4)
#define FLAG_C          (1 << 3)
#define FLAG_d          (1 << 2)
#define FLAG_D          (1 << 1)
#define FLAG_Z          (1 << 0)
#define NO_FLAG          0

static void print_usage(void) {
	printf(
		"Usage: uniq [OPTION]... [FILE]...\n"
		"Filter  adjacent matching lines from FILE, "
		"writing to standard output.\n"
		"\nOptions:\n\n"
		"  -c\tprefix lines by the number of occurrences\n"
		"  -d\tonly print duplicate lines, one for each group\n"
		"  -D\tprint all duplicate lines\n"
		"  -z\tline delimiter is NUL, not newline\n"
		"  -h\tdisplay this help and exit\n"
	);
}

static int read_line(char *line_buff, int max_length, int flags, FILE *fp) {
	int str_size;	
	if (!fgets(line_buff, max_length, fp)) {
		return -1;
	}

	str_size = strlen(line_buff);
	if (line_buff[str_size - 1] != '\n') {
		return 0;
	}

	if (FLAG_Z & flags) {
		line_buff[--str_size] = '\0';
	} else {
		line_buff[str_size] = '\0';
	}

	return 0;
}

static void print_with_flags(char *line_buff, int flags, int dup_count) {
	int i = 0;
	if (dup_count > 1) {
		flags |= FLAG_DUPLICATES;
	}

	switch (flags & ~FLAG_Z) {
		case FLAG_C | FLAG_DUPLICATES | FLAG_d:
		case FLAG_C | FLAG_DUPLICATES:
		case FLAG_C:
			printf("	%i %s", dup_count, line_buff);
			break;
		case NO_FLAG:
		case FLAG_DUPLICATES:
		case FLAG_DUPLICATES | FLAG_d:
			printf("%s", line_buff);
			break;
		case FLAG_D | FLAG_DUPLICATES:
		case FLAG_D | FLAG_DUPLICATES | FLAG_d:
			for (i = 0; i < dup_count; i++) {
				printf("%s", line_buff);
			}
			break;
	}
}

int main(int argc, char **argv) {
	int opt, i = 0;
	char buff, *line_buff_curr, *line_buff_prev, *tmp;
	FILE *fp;	
	int flags = 0;
	int max_length = 0;
	int dup_count = 0;

	if (argc < 2) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc, argv, "cdDzh"))) {
		switch (opt) {
		case 'c':
			flags |= FLAG_C;
			break;
		case 'd':
			flags |= FLAG_d;
			break;
		case 'D':
			flags |= FLAG_D;
			break;
		case 'z':
			flags |= FLAG_Z;
			break;
		case '?':
			printf(
				"uniq: invalid option -- '%c'\n"
				"Try 'uniq -h' for more information.\n",
				optopt
			);
			return 0;
		case 'h':
			print_usage();
			return 0;
		default:
			return -EINVAL;
		}
	}

	if ((FLAG_C & flags) && (FLAG_D & flags)) {
		printf(
			"uniq: printing all duplicated lines and repeat counts is meaningless\n"
			"Try 'uniq --help' for more information.\n"
		);
		return -EINVAL;
	}

	if (NULL == (fp = fopen(argv[argc - 1], "r"))) {
		printf("uniq: cannot read: %s: No such file or directory\n", argv[argc - 1]);
		return -errno;
	}

	while (1 == fread(&buff, sizeof(char), 1, fp)) {
		if (buff != '\n') {
			++i;
		} else {
			max_length = (i > max_length) ? i : max_length;
			i = 0;
		}
	}
	max_length = (max_length + NL_AND_NUL) * sizeof(char);
	fseek(fp, 0, SEEK_SET);

	line_buff_curr = (char *) malloc(max_length);
	line_buff_prev = (char *) malloc(max_length);
	memset(line_buff_curr, '\0', max_length);
	memset(line_buff_prev, '\0', max_length);

	read_line(line_buff_prev, max_length, flags, fp);
	dup_count = 1;

	while (!read_line(line_buff_curr, max_length, flags, fp)) {
		if (strncmp(line_buff_curr, line_buff_prev, max_length)) {
			print_with_flags(line_buff_prev, flags, dup_count);
			dup_count = 1;
			tmp = line_buff_curr;
			line_buff_curr = line_buff_prev;
			line_buff_prev = tmp;
		} else {
			++dup_count;
		}
	}
	print_with_flags(line_buff_prev, flags, dup_count);

	free(line_buff_curr);
	free(line_buff_prev);
	fclose(fp);

	return 0;
}

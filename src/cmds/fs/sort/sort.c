/**
 * @file
 * @brief Sort lines of text files to standart output.
 *
 * @date 6.10.18
 * @author Mark Kovalev
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

static int blank, ign_case, reverse;

static void print_usage(void) {
	printf(
		"Usage: sort [OPTION]... [FILE]...\n"
		"Write sorted lines of FILE to standard output.\n"
		"\nOrdering options:\n\n"
		"  -b\tignore leading blanks\n"
		"  -f\tfold lower case to upper case characters\n"
		"  -r\treverse the result of comparisons\n"
		"\nOther options:\n\n"
		"  -h\tdisplay this help and exit\n"
		"\n*** WARNING ***\n"
		"The locale specified by the environment affects sort order.\n"
		"This implementation assumes that LC_ALL=C set to get the\n"
		"traditional sort order that uses native byte values.\n"
	);
}

/* 	This function similar to "strcasecmp" except for "tolower" changed to "toupper",
	because "sort -f" folds lower case to upper case characters */
static int case_compare(const char *str1, const char *str2) {
	unsigned const char *s1 = (unsigned const char *) str1;
	unsigned const char *s2 = (unsigned const char *) str2;

	while (*s1 && (toupper(*s1) == toupper(*s2))) {
		++s1;
		++s2;
	}

	return toupper(*s1) - toupper(*s2);
}

static int compare(const void *str1_ptr, const void *str2_ptr) {
	int result, blank_result;
	const char *str1 = *(const char **) str1_ptr;
	const char *str2 = *(const char **) str2_ptr;

	if (blank) {
		/* Keeping this result because with -b option "a" < "[SPACE]b" < "b" */
		blank_result = strcmp(str1, str2);
		while (isspace(*str1)) {
			++str1;
		}
		while (isspace(*str2)) {
			++str2;
		}
	}

	if (ign_case) {
		result = case_compare(str1, str2);
		/* Doing this because with -f option "A" < "a" < "B" */
		if (result) {
			return result;
		} else if (blank) {
			return blank_result;
		} else {
			return strcmp(str1, str2);
		}
	} else {
		result = strcmp(str1, str2);
		return (blank && result == 0) ? blank_result : result;
	}
}

int main(int argc, char **argv) {
	int opt, line_count = 1, max_line = 0, str_size = 0, i = 0;
	char buff, *line_buff, **strs;
	FILE *fp;

	blank = 0;
	ign_case = 0;
	reverse = 0;

	if (argc < 2) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc - 1, argv, "bfrh"))) {
		switch (opt) {
		case 'b':
			blank = 1;
			break;
		case 'f':
			ign_case = 1;
			break;
		case 'r':
			reverse = 1;
			break;
		case '?':
			printf(
				"sort: invalid option -- '%c'\n"
				"Try 'sort -h' for more information.\n",
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

	if (NULL == (fp = fopen(argv[argc - 1], "r"))) {
		printf("sort: cannot read: %s: No such file or directory\n", argv[argc - 1]);
		return -errno;
	}

	while (1 == fread(&buff, 1, 1, fp)) {
		if (buff != '\n') {
			++i;
		} else {
			max_line = (i > max_line) ? i : max_line;
			i = 0;
			++line_count;
		}
	}
	fseek(fp, 0, SEEK_SET);
	if (i == 0) {
		--line_count;
	}

	strs = (char **) malloc(sizeof(char *) * line_count);
	line_buff = (char *) malloc(sizeof(char) * (max_line + 2));

	i = 0;
	while (NULL != fgets(line_buff, max_line + 2, fp)) {
		str_size = strlen(line_buff);
		if (line_buff[str_size - 1] == '\n') {
			line_buff[--str_size] = '\0';
		}

		strs[i] = (char *) malloc(sizeof(char) * (str_size + 1));
		strcpy(strs[i++], line_buff);
	}

	qsort(strs, line_count, sizeof(char *), compare);

	if (reverse) {
		for (i = line_count - 1; i >= 0; --i) {
			printf("%s\n", strs[i]);
			free(strs[i]);
		}
	} else {
		for (i = 0; i < line_count; ++i) {
			printf("%s\n", strs[i]);
			free(strs[i]);
		}
	}

	free(line_buff);
	free(strs);
	fclose(fp);

	return 0;
}
/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2014
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <lib/libds/bitmap.h>
#include <debug/symbol.h>

extern int coverage_getstat(const struct symbol **sym_table,
		const unsigned long **cov_bitmap);

int main(int argc, char *argv[]) {
	const char *outfile = NULL;
	FILE *out;
	const struct symbol *sym_table;
	const unsigned long *cov_bitmap;
	int opt, i, sym_n;

	while (-1 != (opt = getopt(argc, argv, "o:h"))) {
		switch (opt) {
		case 'h':
			printf("Usage: %s [-h] [-o output_file]\n", argv[0]);
			return 0;
		case 'o':
			outfile = optarg;
		default:
			break;
		}
	}

	if (outfile) {
		out = fopen(outfile, "w");
		if (!out) {
			perror("fopen");
			return -errno;
		}
	} else {
		out = stdout;
	}


	sym_n = coverage_getstat(&sym_table, &cov_bitmap);
	if (sym_n <= 0) {
		if (outfile)
			fclose(out);
		return sym_n;
	}

	fprintf(out, "%32s\t%8s\n", "FUNCTION", "EXECUTED");
	for (i = 0; i < sym_n; i++) {

		fprintf(out, "%21s@0x%08lx\t%8s\n", sym_table[i].name,
				(unsigned long) sym_table[i].addr,
				bitmap_test_bit(cov_bitmap, i) ? "true" : "false");
	}

	if (outfile)
		fclose(out);

	return 0;
}

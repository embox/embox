/*
 * @file
 * @brief
 *
 * @date 21.11.2012
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/indexator.h>

#define WHOLE_NAME     0
#define DIGITAL_IDX    1
#define CHARACTER_IDX  2

static char block_dev_check_type_name (char *p) {
	if (*p == '#') {
		return DIGITAL_IDX;
	}
	else if(*p == '*') {
		return CHARACTER_IDX;
	}
	/* specifies the full name of the device */
	else {
		return WHOLE_NAME;
	}
}

static void block_dev_select_name(char *name, int idx) {
	char *p;

	p = name;
	if (*p) {
	    p += strlen(p+1);
	}
	else {
		return;
	}

	switch (block_dev_check_type_name (p)) {
	case WHOLE_NAME:
		break;

	case DIGITAL_IDX:
		sprintf(p, "%d", idx);
		break;

	case CHARACTER_IDX:
		sprintf(p, "%c", 'a' + idx);
		break;
	}
	return;
}

int block_dev_named(char *name, struct indexator *indexator) {
	int idx;

	if(-1 == (idx = index_alloc(indexator, INDEX_ALLOC_MIN))) {
		return -1;
	}
	block_dev_select_name(name, idx);

	return idx;
}


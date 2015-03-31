/**
 * @file
 * @brief
 *
 * @date 19.11.2012
 * @author Andrey Gazukin
 *          - Initial implementation.
 * @author Eldar Abusalimov
 *          - path_next
 */
#include <stdio.h>
#include <string.h>

#include <fs/hlpr_path.h>
#include <assert.h>


/*
 * remove the top directory name from path
 */
void path_cut_mount_dir(char *path, char *mount_dir) {
	char *p;

	p = path;
	while (*mount_dir && (*mount_dir == *p)) {
		mount_dir++;
		p++;
	}
	strcpy((char *) path, (const char *) p);
}

/*
 *  move the last name of the whole directory to the top of tail
 */
int path_nip_tail(char *head, char *tail) {
	char *p_tail;
	char *p;

	p = p_tail = head + strlen(head);
	strcat(head, tail);

	do {
		p_tail--;
		if (head > p_tail) {
			*p = '\0';
			return -1;
		}
	} while ('/' != *p_tail);

	strcpy (tail, p_tail);
	*p_tail = '\0';

	return 0;
}

/*
 * move the top of tail as the last name of the head
 */
int path_increase_tail(char *head, char *tail) {
	char *p_tail;

		p_tail = head + strlen(head);
		strcat(head, tail);

		do {
			if ('\0' == *p_tail) {
				break;
			}
			p_tail++;
		} while ('/' != *p_tail);

		strcpy (tail, p_tail);
		*p_tail = '\0';

		return 0;
}

/*
 * Save first node name in path into node_name parameter.
 * Return the remaining part of path.
 */
char *path_get_next_name(const char *path, char *node_name, int buff_len) {
	char *p = (char *) path;
	char *nm = node_name;

	*nm = '\0'; /* empty node_name */

	/* we must pass '/' symbol */
	if ('/' == *p) {
		p++;
	}

	/* we copy chars while not appear end of string or we don't find next item
	 * or don't fill the buffer*/
	while (('/' != *p) && ('\0' != *p) && (buff_len-- > 0)) {
		*nm++ = *p++;
	}

	/* we must setup terminate symbol to the end of node_name */
	*nm = '\0';

	/* if we found some symbols it's a correct node_name  */
	if (nm != node_name) {
		return p;
	}

	return NULL;
}

static inline int path_is_single_dot(const char *path) {
	char nc = *(path + 1);

	return *path == '.' && (nc == '/' || nc == '\0');
}

const char *path_next(const char *path, size_t *p_len) {

	if (!path) {
		return NULL;
	}

	/* Skip leading slashes. */
	/* XXX Skip single dots too */
	while (*path == '/' || path_is_single_dot(path)) {
		++path;
	}

	if (!*path) {
		return NULL;
	}

	if (p_len) {
		const char *end = path;

		while (*end && *end != '/') {
			++end;
		}

		*p_len = end - path;
	}

	return path;
}

int path_is_dotname(const char *name, size_t name_len) {
	if (name_len - 1 <= 1 && name[0] == '.' && name[name_len - 1] == '.') {
		return name_len;
	} else {
		return 0;
	}
}

int path_is_double_dot(const char *path) {
	return *path == '.' && *(path + 1) == '.'
		&& (*(path + 2) == '/' || *(path + 2) == '\0');
}

/**
 * @file
 *
 * @date 28.08.2009
 * @author Roman Evstifeev
 */
#ifndef FILE_IFACE_H_
#define FILE_IFACE_H_

#define MAX_FILENAME_LEN 80

typedef struct  {
	char name[MAX_FILENAME_LEN];
	int size;
} FLIST_ITEM;

#endif // FILE_IFACE_H_

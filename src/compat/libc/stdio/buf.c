/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    24.11.2014
 */

#include "file_struct.h"

#include <stdio.h>

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {

	if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF) {
		return -1;
	}

	fflush(stream);

	stream->obuf = buf;
	stream->obuf_sz = size;
	stream->obuf_len = 0;

	return 0;
}

void setbuffer(FILE *stream, char *buf, size_t size) {
	setvbuf(stream, buf, buf ? _IOFBF : _IONBF, size);

}

void setbuf(FILE *stream, char *buf) {
	setbuffer(stream, buf, BUFSIZ);
}

#if 0
void setlinebuf(FILE *stream) {
	/* relies on heap allocation, not implemented */
}
#endif




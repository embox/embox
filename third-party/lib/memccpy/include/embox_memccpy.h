/* 
    memccpy() function implementation
    based on the POSIX implementation

*/


// embox_memccpy() takes a void pointer str
// and copies no more than n bytes into dest

// if character c found, it stops

void* embox_memccpy(void* dest, const void* src, int c, size_t n);
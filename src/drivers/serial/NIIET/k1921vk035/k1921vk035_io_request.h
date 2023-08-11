#include <stddef.h>
#include <stdint.h>

int io_request_read(char* buf, size_t count);
int io_request_read_timeout(char* buf, size_t count, int32_t timeout_ms);

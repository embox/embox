#include <sys/uio.h>
#include <stdint.h>
#include "semaphore.h"

typedef enum {
    IO_REQUEST_MODE_NONE = 0,
    IO_REQUEST_MODE_READ = 1,
    IO_REQUEST_MODE_DISCARD = 2,
} io_mode_type;

typedef struct {
    char* buffer;
    size_t count;
} io_mode_read;

typedef struct {
    size_t count;
} io_mode_discard;

typedef struct {
    sem_t semaphore;
    io_mode_type type;
    union {
        io_mode_read read;
        io_mode_discard discard;
    } mode;
    struct ring_buff* rx_buff;
    struct uart* uart;
} io_request_t;


int io_request_read(io_request_t* io, char* buf, size_t count);
int io_request_read_timeout(io_request_t* io, char* buf, size_t count, int32_t timeout_ms);
int io_request_readv(io_request_t* io, struct iovec* iov, size_t iov_count);
int io_request_readv_timeout(io_request_t* io, struct iovec* iov, size_t iov_count, int32_t timeout_ms_per_iov);

int io_request_write(io_request_t* io, char* buf, size_t count);

void io_request_discard_next_bytes(io_request_t* io, size_t count);

/**
 * @file
 * @brief SHA-1 cryptographic hash function
 *
 * @date Sept 03, 2025
 * @author Peize Li
 */

#ifndef LIB_SHA1_H_
#define LIB_SHA1_H_

#include <stddef.h>
#include <stdint.h>

#define SHA1_DIGEST_SIZE 20
#define SHA1_BLOCK_SIZE  64

typedef struct sha1_state {
	uint32_t h[5];   /* Hash state */
	uint64_t count;  /* 64-bit bit count */
	uint8_t buf[64]; /* Accumulate block */
	int buf_len;     /* Bytes in buffer */
} sha1_state_t;

#ifdef __cplusplus
extern "C" {
#endif

/* One-shot SHA1 calculation */
uint8_t *sha1_count(const uint8_t *data, size_t len,
    uint8_t digest[SHA1_DIGEST_SIZE]);

/* Initialize SHA1 context */
void sha1_init(sha1_state_t *ctx);

/* Update SHA1 context with data */
void sha1_update(sha1_state_t *ctx, const uint8_t *data, size_t len);

/* Finalize and get digest */
void sha1_final(sha1_state_t *ctx, uint8_t digest[SHA1_DIGEST_SIZE]);

#ifdef __cplusplus
}
#endif

#endif /* LIB_SHA1_H_ */
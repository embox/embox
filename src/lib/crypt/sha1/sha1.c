/**
 * @file
 * @brief SHA-1 implementation for Embox
 *
 * @date Sept 03, 2025
 * @author Peize Li
 */

#include <endian.h>
#include <string.h>

#include <lib/crypt/sha1.h>

/* SHA-1 constants */
#define K0 0x5A827999
#define K1 0x6ED9EBA1
#define K2 0x8F1BBCDC
#define K3 0xCA62C1D6

/* Circular left shift */
#define ROTL(n, x) (((x) << (n)) | ((x) >> (32 - (n))))

/* SHA-1 functions */
#define F0(b, c, d) ((b & c) | (~b & d))
#define F1(b, c, d) (b ^ c ^ d)
#define F2(b, c, d) ((b & c) | (b & d) | (c & d))
#define F3(b, c, d) (b ^ c ^ d)

/* Convert byte array to 32-bit big-endian */
static inline uint32_t get_be32(const uint8_t *p) {
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
	       | ((uint32_t)p[2] << 8) | p[3];
}

/* Convert 32-bit to big-endian byte array */
static inline void put_be32(uint8_t *p, uint32_t v) {
	p[0] = (v >> 24) & 0xff;
	p[1] = (v >> 16) & 0xff;
	p[2] = (v >> 8) & 0xff;
	p[3] = v & 0xff;
}

/* Process one 512-bit block */
static void sha1_process_block(sha1_state_t *ctx, const uint8_t *block) {
	uint32_t w[80];
	uint32_t a, b, c, d, e;
	uint32_t temp;
	int i;

	/* Prepare message schedule */
	for (i = 0; i < 16; i++) {
		w[i] = get_be32(block + i * 4);
	}

	for (i = 16; i < 80; i++) {
		w[i] = ROTL(1, w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16]);
	}

	/* Initialize working variables */
	a = ctx->h[0];
	b = ctx->h[1];
	c = ctx->h[2];
	d = ctx->h[3];
	e = ctx->h[4];

	/* Main loop */
	for (i = 0; i < 20; i++) {
		temp = ROTL(5, a) + F0(b, c, d) + e + w[i] + K0;
		e = d;
		d = c;
		c = ROTL(30, b);
		b = a;
		a = temp;
	}

	for (i = 20; i < 40; i++) {
		temp = ROTL(5, a) + F1(b, c, d) + e + w[i] + K1;
		e = d;
		d = c;
		c = ROTL(30, b);
		b = a;
		a = temp;
	}

	for (i = 40; i < 60; i++) {
		temp = ROTL(5, a) + F2(b, c, d) + e + w[i] + K2;
		e = d;
		d = c;
		c = ROTL(30, b);
		b = a;
		a = temp;
	}

	for (i = 60; i < 80; i++) {
		temp = ROTL(5, a) + F3(b, c, d) + e + w[i] + K3;
		e = d;
		d = c;
		c = ROTL(30, b);
		b = a;
		a = temp;
	}

	/* Update hash state */
	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
	ctx->h[4] += e;

	/* Clear sensitive data */
	memset(w, 0, sizeof(w));
}

void sha1_init(sha1_state_t *ctx) {
	ctx->h[0] = 0x67452301;
	ctx->h[1] = 0xEFCDAB89;
	ctx->h[2] = 0x98BADCFE;
	ctx->h[3] = 0x10325476;
	ctx->h[4] = 0xC3D2E1F0;
	ctx->count = 0;
	ctx->buf_len = 0;
}

void sha1_update(sha1_state_t *ctx, const uint8_t *data, size_t len) {
	if (len == 0) {
		return;
	}

	/* Handle partial block */
	if (ctx->buf_len > 0) {
		size_t copy = SHA1_BLOCK_SIZE - ctx->buf_len;
		if (copy > len) {
			copy = len;
		}
		memcpy(ctx->buf + ctx->buf_len, data, copy);
		ctx->buf_len += copy;
		data += copy;
		len -= copy;
		ctx->count += copy * 8;

		if (ctx->buf_len == SHA1_BLOCK_SIZE) {
			sha1_process_block(ctx, ctx->buf);
			ctx->buf_len = 0;
		}
	}

	/* Process full blocks */
	while (len >= SHA1_BLOCK_SIZE) {
		sha1_process_block(ctx, data);
		data += SHA1_BLOCK_SIZE;
		len -= SHA1_BLOCK_SIZE;
		ctx->count += SHA1_BLOCK_SIZE * 8;
	}

	/* Save remaining bytes */
	if (len > 0) {
		memcpy(ctx->buf, data, len);
		ctx->buf_len = len;
		ctx->count += len * 8;
	}
}

void sha1_final(sha1_state_t *ctx, uint8_t digest[SHA1_DIGEST_SIZE]) {
	uint8_t pad[SHA1_BLOCK_SIZE * 2]; // 最多两个块
	uint64_t bit_count;
	int i;

	/* Save bit count */
	bit_count = ctx->count;

	/* Start with 0x80 */
	pad[0] = 0x80;

	/* Calculate how many zero bytes needed before length field */
	/* We want: (ctx->buf_len + 1 + zeros) % 64 == 56 */
	int zeros = (56 - (ctx->buf_len + 1) % 64 + 64) % 64;
	memset(pad + 1, 0, zeros);

	/* Append 0x80 + zeros */
	sha1_update(ctx, pad, 1 + zeros);

	/* Append 8-byte big-endian bit length */
	for (i = 7; i >= 0; i--) {
		pad[i] = bit_count & 0xff;
		bit_count >>= 8;
	}
	sha1_update(ctx, pad, 8);

	/* Extract digest */
	for (i = 0; i < 5; i++) {
		put_be32(digest + i * 4, ctx->h[i]);
	}

	/* Clear sensitive data */
	memset(ctx, 0, sizeof(*ctx));
}

uint8_t *sha1_count(const uint8_t *data, size_t len,
    uint8_t digest[SHA1_DIGEST_SIZE]) {
	sha1_state_t ctx;
	sha1_init(&ctx);
	sha1_update(&ctx, data, len);
	sha1_final(&ctx, digest);
	return digest;
}

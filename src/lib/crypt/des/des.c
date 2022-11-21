/**
 * @file
 * @brief  Data Encryption Standard (DES)
 *
 * @date Sep 17, 2022
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "des.h"

#define DES_BITMAP_SIZE      64

/*
 * Tables bellow can be verified on wikipedia:
 * http://en.wikipedia.org/wiki/DES_supplementary_material
 */

static const uint8_t initial_permutation [DES_BITMAP_SIZE]	=	{
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

static const uint8_t final_permunation[DES_BITMAP_SIZE]	=	{
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41,  9, 49, 17, 57, 25
};

/* Substitution boxes (S-boxes)
 * This table lists the eight S-boxes used in DES.
 * Each S-box replaces a 6-bit input with a 4-bit output. Given a 6-bit input,
 * the 4-bit output is found by selecting the row using the outer two bits,
 * and the column using the inner four bits. For example, an input "011011"
 * has outer bits "01" and inner bits "1101"; noting that the first row is "00"
 * and the first column is "0000", the corresponding output for S-box S5
 * would be "1001" (=9), the value in the second row, 14th column. (See S-box).
 */
static const uint8_t s_boxes[8][4][16]	=	{
	/* S1 */
	{
		{14,  4, 13, 1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
		{ 0, 15,  7, 4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
		{ 4,  1, 14, 8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
		{15, 12,  8, 2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13}
	},
	/* S2 */
	{
		{15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
		{ 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
		{ 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
		{13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9}
	},
	/* S3 */
	{
		{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
		{13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
		{13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
		{ 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}
	},
	/* S4 */
	{
		{ 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
		{13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
		{10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
		{ 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}
	},
	/* S5 */
	{
		{ 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
		{14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
		{ 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
		{11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3}
	},
	/* S6 */
	{
		{12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
		{10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
		{ 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
		{ 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13}
	},
	/* S7 */
	{
		{ 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
		{13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
		{ 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
		{ 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12}
	},
	/* S8 */
	{
		{13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
		{ 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
		{ 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
		{ 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11}
	}
};

static const char des_extansion[48]= {
	 32,  1,  2,  3,  4,  5,
	  4,  5,  6,  7  ,8,  9,
	  8,  9, 10, 11, 12, 13,
	 12, 13, 14, 15, 16, 17,
	 16, 17, 18, 19, 20, 21,
	 20, 21, 22, 23, 24, 25,
	 24, 25, 26, 27, 28, 29,
	 28, 29, 30, 31, 32,  1
};

/* The P permutation shuffles the bits of a 32-bit half-block. */
static const char permutation[32]={
	 16,  7, 20, 21, 29, 12, 28, 17,
	  1, 15, 23, 26,  5, 18, 31, 10,
	  2,  8, 24, 14, 32, 27,  3,  9,
	 19, 13, 30,  6, 22, 11,  4,  25
};

/* The "Left" and "Right" halves of the table show which bits from
 * the input key form the left and right sections of the key schedule state.
 * Note that only 56 bits of the 64 bits of the input are selected;
 * the remaining eight (8, 16, 24, 32, 40, 48, 56, 64) were specified for use as parity bits.
 */
static const char permuted_choice_1[56] = {
		 57, 49, 41, 33, 25, 17,  9,
		  1, 58, 50, 42, 34, 26, 18,
		 10,  2, 59, 51, 43, 35, 27,
		 19, 11,  3, 60, 52, 44, 36,
		 63, 55, 47, 39, 31, 23, 15,
		  7, 62, 54, 46, 38, 30, 22,
		 14,  6, 61, 53, 45, 37, 29,
		 21, 13,  5, 28, 20, 12,  4
};

static const char rotation_table[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

static const char permuted_choice_2[48]= {
		 14, 17, 11, 24,  1,  5,
		  3, 28, 15,  6, 21, 10,
		 23, 19, 12,  4, 26,  8,
		 16,  7, 27, 20, 13,  2,
		 41, 52, 31, 37, 47, 55,
		 30, 40, 51, 45, 33, 48,
		 44, 49, 39, 56, 34, 53,
		 46, 42, 50, 36, 29, 32
};

static inline void bytes_to_bits(uint8_t src[8], uint8_t dst[DES_BITMAP_SIZE]) {
	int i, k, n;

	n = 0;

	memset(dst, 0 , DES_BITMAP_SIZE);

	for (i = 0; i < 8; i++) {
		for (k = 7; k >= 0; k--) {
			dst[n] = (src[i] >> k) & 0x01;
			n++;
		}
	}
}

static inline void bits_to_bytes(uint8_t dst[DES_BITMAP_SIZE], uint8_t src[8]) {
	int n;

	memset(src, 0, 8);

	for (n = 0; n < DES_BITMAP_SIZE; n++) {
		src[n / 8] |= dst[n] << (7 - (n % 8));
	}
}

static inline void
permutation_init(uint8_t data[8], uint8_t left[32], uint8_t right[32]) {
	int i;
	uint8_t bits[DES_BITMAP_SIZE];

	bytes_to_bits(data, bits);

	i = 0;

	for (; i < 32; i++) {
		left[i] = bits[initial_permutation[i] - 1];
		right[i] = bits[initial_permutation[i + 32] - 1];
	}
}

static inline void
permutation_finit(uint8_t res[8], uint8_t left[32], uint8_t right[32]) {
	uint8_t tmp1[DES_BITMAP_SIZE];
	uint8_t d[DES_BITMAP_SIZE];
	int i;

	for (i = 0; i < 32; i++) {
		d[i + 32] = right[i];
		d[i] = left[i];
	}

	for (i = 0; i < 32; i++) {
		tmp1[i] = d[final_permunation[i] - 1];
		tmp1[i + 32] = d[final_permunation[i + 32] - 1];
	}

	bits_to_bytes(tmp1, res);
}

static inline void fill_keys(uint8_t bin_key[8], uint8_t c[28], uint8_t d[28]) {
	int i;
	uint8_t key_bits[DES_BITMAP_SIZE];

	bytes_to_bits(bin_key, key_bits);

	for (i = 0; i < 28; i++) {
		c[i] = key_bits[permuted_choice_1[i] - 1];
		d[i] = key_bits[permuted_choice_1[i + 28] - 1];
	}
}

static inline void rotate(int step, uint8_t c[28], uint8_t d[28], uint8_t key[48]) {
	int i, k;
	uint8_t tmp_key[56];

	for (i = 0; i < 28; i++) {
		k = (i + rotation_table[step]) % 28;
		tmp_key[i] = c[k];
		tmp_key[i + 28] = d[k];
	}

	for (i = 0; i < 28; i++) {
		d[i] = tmp_key[i + 28];
		c[i] = tmp_key[i];
	}

	for (i = 0; i < 48; i++) {
		key[i] = tmp_key[permuted_choice_2[i] - 1];
	}
}

static inline void
permutate(uint8_t left[32], uint8_t right[32], uint8_t key[48], uint8_t res[32]) {
	uint8_t tmp_key[48];
	int i;
	uint8_t tmp[32];

	for (i = 0; i < 48; i++) {
		tmp_key[i] = (uint8_t) (right[des_extansion[i] - 1] ^ key[i]);
	}

	/* 48->32 */
	for (i = 0; i < 8; i++) {
		uint8_t *p = &tmp_key[i * 6];
		uint8_t col, row;
		uint8_t k;

		col = (p[1] << 3) ^ (p[2] << 2) ^ (p[3] << 1) ^ p[4];
		row = (p[0] << 1) ^ p[5];

		k = s_boxes[i][row][col];

		tmp[i * 4 + 0] = ((k >> 3) & 0x01);
		tmp[i * 4 + 1] = ((k >> 2) & 0x01);
		tmp[i * 4 + 2] = ((k >> 1) & 0x01);
		tmp[i * 4 + 3] = ((k >> 0) & 0x01);
	}

	for (i = 0; i < 32; i++) {
		res[i] = tmp[permutation[i] - 1] ^ left[i];
	}
}

void des_encrypt(uint8_t data[8], uint8_t key[8], uint8_t res[8]) {
	int n;
	uint8_t left[DES_BITMAP_SIZE / 2], right[DES_BITMAP_SIZE / 2];
	uint8_t tmp_bits[DES_BITMAP_SIZE / 2];
	uint8_t c[28], d[28];
	uint8_t tmp_key[48];

	/* 8 bytes -> 64 bytes -> 2 x 28 (56) bytes */
	fill_keys(key, c, d);

	permutation_init(data, left, right);

	for (n = 0; n < 16; n++) {

		rotate(n, c, d, tmp_key);

		permutate(left, right, tmp_key, tmp_bits);

		memcpy(left, right, sizeof(left));
		memcpy(right, tmp_bits, sizeof(right));
	}

	permutation_finit(res, right, left);
}

void des_decrypt(uint8_t data[8], uint8_t key[8], uint8_t res[8]) {
	int  n;
	uint8_t left[DES_BITMAP_SIZE / 2], right[DES_BITMAP_SIZE / 2];
	uint8_t tmp_bits[DES_BITMAP_SIZE / 2];
	uint8_t c[28], d[28];
	uint8_t tmp_keys[16][48];

	fill_keys(key, c, d);

	permutation_init(data, right, left);

	for (n = 15; n >= 0; n--) {
		rotate(15 - n, c, d, tmp_keys[n]);
	}

	for (n = 0; n < 16; n++) {
		permutate(right, left, tmp_keys[n], tmp_bits);

		memcpy(right, left, 32);
		memcpy(left, tmp_bits, 32);
	}

	permutation_finit(res, left, right);
}

/**
 * @file
 * @brief lib for read and draw PNG
 *
 * @date May 10, 2019
 * @author Filipp Chubukov
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <lib/png.h>
#include <arpa/inet.h>
#include <util/log.h>
#include <fs/file_format.h>

#define CHUNK_INFO_LEN 12 /* 4 = length, 4 = name, 4 = crc */

/* 256 literals, end code, length codes + 2 unused codes */
#define NUMBER_OF_CODE_SYMBOLS 288

/* the distance codes have their own symbols, 30 used, 2 unused */
#define NUMBER_OF_DISTANCE_SYMBOLS 32

/* lengths codes */
#define NUMBER_OF_LENGTH_CODES 19
#define FIRST_LENGTH_INDEX 257
#define LAST_LENGTH_INDEX 285

/**
 * the base of distances(the bits of distance
 * codes appear after length codes and use their own huffman tree)
 */
static const int DISTANCE_BASE[30]
	= {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
		769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};

/* addit bits of distances (added to base)*/
static const int DISTANCE_ADDITIONAL[30]
	= {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

/* represented by codes 257-285 */
static const int LENGTH_BASE[29]
	= {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
		67, 83, 99, 115, 131, 163, 195, 227, 258};

/* addit bits with codes 257-285 (added to base length) */
static const int LENGTH_ADDITIONAL[29]
	= {0, 0, 0, 0, 0, 0, 0,  0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
		4, 4, 4, 4, 5, 5, 5, 5, 0};
/**
 * the order in which "code length alphabet code lengths" are stored, out of this
 * the huffman tree of the dynamic huffman tree lengths is generated
 */
static const int CLCL_ORDER[NUMBER_OF_LENGTH_CODES]
	= {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

static const char *list_of_chunks = "IDATIENDbKGDcHRMdSIGeXIFgAMAhISTiCCPiTXtpHYssBITsPLTsRGBsTERtEXttIMEtRNSzTXt";

#define READBIT(bitpointer, bitstream) ((bitstream[bitpointer >> 3] >> (bitpointer & 0x7)) & (uint8_t)1)

#define IHDR_END 33 /* position of end of IHDR chunk */
#define IHDR_SIZE 13 /* len of IHDR data */

void png_unload(struct png *png_data) {
	png_data->width = 0;
	png_data->height = 0;
	png_data->color = 0;
	png_data->image_size = 0;

	free(png_data->image);
}

struct png_header {
	uint32_t width;
	uint32_t height;

	uint8_t bit_depth;
	uint8_t color_type; /* 1 - plt, 2 - RGB, 4 or 6 - RGBA */
	uint8_t compression_method; /* == 0 - deflate */
	uint8_t filter_method; /* == 0 */
	uint8_t interlace_method; /* 0 - standart, 1 - Adam7 */
};

/* def Huffmantree struct and functions to work with it */
typedef struct HuffmanTree {
	int *tree2d;
	int *tree1d;
	int *lengths; /* lengths of the codes(tree1d) */
	int maxbitlen; /* max bits that single code can get */
	int num_of_codes; /* number of symbols in the alph */
} HuffmanTree;

static void HuffmanTree_init(HuffmanTree* tree) {
	tree->tree2d = 0;
	tree->tree1d = 0;
	tree->lengths = 0;
}

static void HuffmanTree_cleanup(HuffmanTree* tree) {
	free(tree->tree2d);
	free(tree->tree1d);
	free(tree->lengths);
}

/* fun to decode symbols, return decoded symbol or error(-1) */
static int huffman_decode_symbol(uint8_t* in, int* bp,
				HuffmanTree* tree, int inbitlength) {
	int pos = 0, tmp;
	/* endless 'for' until get symbol or get error */
	for (;;) {
		if (*bp >= inbitlength) {
			return -1; /* error: end of input memory reached without endcode */
		}
		tmp = tree->tree2d[(pos << 1) + READBIT(*bp, in)];
		(*bp)++;

		if (tmp < tree->num_of_codes) {
			return tmp; /*get decoded symbol, return it*/
		} else {
			pos = tmp - tree->num_of_codes; /* not yet symbol, continue */
		}

		if (pos >= tree->num_of_codes) {
			return -1; /* error: it appeared outside the codetree */
		}
	}
}

/*
 * make from tree1d[] tree2d[][]. value = 6666 means not init
 * value < number_of_codes is a codes
 * value >= number_of_codes is an address to another bit
 * The 2 rows are the 2 possible bit values (0 or 1), there are as
 * many columns as codes - 1.
 */
static int make_tree2d(HuffmanTree *tree) {
	int nodefill_info = 0;
	int pos = 0; /* pos in the tree */
	int i, n;

	tree->tree2d = (int*)malloc(tree->num_of_codes * 2 * sizeof(int));
	if (!tree->tree2d) {
		log_error("Malloc error.");
		return ENOMEM;
	}

	for (n = 0; n < tree->num_of_codes * 2; n++) {
		tree->tree2d[n] = 6666; /* 6666 means not filled */
	}

	for (n = 0; n < tree->num_of_codes; n++) /* the codes */ {
		for (i = 0; i != tree->lengths[n]; i++) /* the bits for this code */ {

			uint8_t bit = (uint8_t)((tree->tree1d[n] >> (tree->lengths[n] - i - 1)) & 1);
			if (tree->tree2d[2 * pos + bit] == 6666) /* not yet filled */ {
				if (i + 1 == tree->lengths[n]) /* last bit */ {
					tree->tree2d[2 * pos + bit] = n; /* put the current code in it */
					pos = 0;
				} else {
					/* put address of the next step, it's (nodefilled + 1) */
					nodefill_info++;
					/* addresses encoded with numcodes added to it */
					tree->tree2d[2 * pos + bit] = nodefill_info + tree->num_of_codes;
					pos = nodefill_info;
				}
			} else {
				pos = tree->tree2d[2 * pos + bit] - tree->num_of_codes;
			}
		}
	}
	for (n = 0; n < tree->num_of_codes * 2; n++) {
		if (tree->tree2d[n] == 6666) {
			tree->tree2d[n] = 0; /* remove possible remaining 6666 */
		}
	}
	return 0;
}

/*
 * Second step of making tree.
 * num_of_codes, lengths need to be filled.
 * return 0 with success or error.
 */
static int make_tree_second_step(HuffmanTree *tree) {
	int blcount[tree->maxbitlen + 1];
	int nextcode[tree->maxbitlen + 1];
	int bits, i;

	for (i = 0; i < tree->maxbitlen + 1; i++) {
		blcount[i] = 0;
		nextcode[i] = 0;
	}

	tree->tree1d = (int*)malloc(tree->num_of_codes * sizeof(int));
	if (!tree->tree1d) {
		log_error("Malloc error.");
		return ENOMEM;
	} else {
		/* step 1: count number of instances of each code length */
		for (bits = 0; bits != tree->num_of_codes; bits++) {
			blcount[tree->lengths[bits]]++;
		}

		/* step 2: generate the nextcode values */
		for (bits = 1; bits <= tree->maxbitlen; bits++) {
			nextcode[bits] = (nextcode[bits - 1] + blcount[bits - 1]) << 1;
		}

		/* step 3: generate all the codes */
		for (i = 0; i != tree->num_of_codes; i++) {
			if (tree->lengths[i] != 0) tree->tree1d[i] = nextcode[tree->lengths[i]]++;
		}
	}
	return make_tree2d(tree);
}

/*
 * first step of making tree, generate the tree as defined
 * by Deflate.
 * return 0 with success of error.
 */
static int make_tree_first_step(HuffmanTree *tree, int *bitlen,
					int number_of_codes, int maxbitlen) {
	int i;

	tree->lengths = (int*)malloc(number_of_codes * sizeof(int));
	if (!tree->lengths) {
		log_error("Malloc error.");
		return ENOMEM;
	}

	for (i = 0; i != number_of_codes; i++) {
		tree->lengths[i] = bitlen[i];
	}

	tree->num_of_codes = number_of_codes; /* number of symbols */
	tree->maxbitlen = maxbitlen;

	return make_tree_second_step(tree);
}

/* fun for reading bits from stream */
static int read_bits_from_stream(int *bitpointer, uint8_t *bitstream, int nbits) {
	int result = 0, i;
	for (i = 0; i != nbits; i++) {
		result += ((int)READBIT(*bitpointer, bitstream)) << i;
		(*bitpointer)++;
	}
	return result;
}

/* get the tree with dynamic huffman blocks */
static int build_dynamic_trees(HuffmanTree *ll_tree, HuffmanTree *dist_tree,
								uint8_t *in, int *bp, int inlen) {
	int err = 0;
	int n, HLIT, HDIST, HCLEN, i;
	int in_bitlen = inlen * 8;
	HuffmanTree cl_tree; /* the huffman tree for compressed huffman trees */
	int bitlen_ll[NUMBER_OF_CODE_SYMBOLS]; /* lit,len code lengths */
	int bitlen_d[NUMBER_OF_DISTANCE_SYMBOLS];
	/*
	 * the bit lengths of the huffman tree used for
	 * compress bitlen_ll and bitlen_d
	 */
	int bitlen_cl[NUMBER_OF_LENGTH_CODES];
	int replength, value, code;
	/* number of literal/length codes + 257 added by spec */
	HLIT =  read_bits_from_stream(bp, in, 5) + 257;
	/* number of distance codes + 1 added by spec */
	HDIST = read_bits_from_stream(bp, in, 5) + 1;
	/* number of code length codes + 4 added by spec */
	HCLEN = read_bits_from_stream(bp, in, 4) + 4;

	HuffmanTree_init(&cl_tree);

	while (!err) {
		/* read the code length codes out of 3 * (amount of code length codes) bits */
		for (i = 0; i != NUMBER_OF_LENGTH_CODES; i++) {
			if (i < HCLEN) {
				bitlen_cl[CLCL_ORDER[i]] = read_bits_from_stream(bp, in, 3);
			} else {
				bitlen_cl[CLCL_ORDER[i]] = 0; /*if not, it must stay 0*/
			}
		}
		err = make_tree_first_step(&cl_tree, bitlen_cl, NUMBER_OF_LENGTH_CODES, 7);
		if (err) {
			break;
		}
		/* now we can use this tree to read the lengths for cl_tree */

		for (i = 0; i != NUMBER_OF_CODE_SYMBOLS; i++) {
			bitlen_ll[i] = 0;
		}

		for (i = 0; i != NUMBER_OF_DISTANCE_SYMBOLS; i++) {
			bitlen_d[i] = 0;
		}
		/**
		 * i is the current symbol we're reading in the part
		 * that contains the code lengths of lit/len and dist codes
		 */
		i = 0;

		while (i < HLIT + HDIST) {

			code = huffman_decode_symbol(in, bp, &cl_tree, in_bitlen);

			if (code <= 15) /* a length code */ {
				if (i < HLIT) {
					bitlen_ll[i] = code;
				} else {
					bitlen_d[i - HLIT] = code;
				}
				i++;
			} else if (code == 16) /* repeat previous */ {
				replength = 3; /* read in the 2 bits that indicate repeat length (3-6) */

				if (i == 0) {
					err = -1; /* can't repeat previous if i is 0 */
					break;
				}

				if ((*bp + 2) > in_bitlen) {
					err = -1; /* error, bit pointer jumps past memory */
					break;
				}
				replength += read_bits_from_stream(bp, in, 2);

				if (i < HLIT + 1) {
					value = bitlen_ll[i - 1];
				} else {
					value = bitlen_d[i - HLIT - 1];
				}
				/* repeat this value in the next lengths */
				for (n = 0; n < replength; n++) {
					if (i >= HLIT + HDIST) {
						err = -1; /* error: i is larger than the amount of codes */
						break;
					}
					if (i < HLIT) {
						bitlen_ll[i] = value;
					} else {
						bitlen_d[i - HLIT] = value;
					}
					i++;
				}
			} else if (code == 17) /* repeat "0" 3-10 times */ {
				replength = 3; /* read in the bits that indicate repeat length */

				if ((*bp + 3) > in_bitlen) {
					err = -1; /* error, bit pointer jumps past memory */
					break;
				}

				replength += read_bits_from_stream(bp, in, 3);

				/* repeat this value in the next lengths */
				for (n = 0; n < replength; n++) {
					if (i >= HLIT + HDIST) {
						return -1; /* error: i is larger than the amount of codes */
					}

					if (i < HLIT) {
						bitlen_ll[i] = 0;
					} else {
						bitlen_d[i - HLIT] = 0;
					}
					i++;
				}
			} else if (code == 18) /* repeat "0" 11-138 times */ {
				replength = 11; /* read in the bits that indicate repeat length */
				if ((*bp + 7) > in_bitlen) {
					err = -1; /* error, bit pointer jumps past memory */
					break;
				}
				replength += read_bits_from_stream(bp, in, 7);

				/* repeat this value in the next lengths */
				for (n = 0; n < replength; n++) {
					if (i >= HLIT + HDIST) {
						err = -1; /* error: i is larger than the amount of codes */
						break;
					}
					if (i < HLIT) {
						bitlen_ll[i] = 0;
					} else {
						bitlen_d[i - HLIT] = 0;
					}
					i++;
				}
			} else {
				if (code == -1) {
					err = -1; /* error in decodesymbol fun */
					break;
				}
			}
		}
		if (err) {
			break;
		}

		if (bitlen_ll[256] == 0) {
			err = -1; /* the length of the end code 256 must be larger than 0 */
			break;
		}

		/* generate the code trees */
		err = make_tree_first_step(ll_tree, bitlen_ll, NUMBER_OF_CODE_SYMBOLS, 15);

		if (err) {
			break;
		}

		err = make_tree_first_step(dist_tree, bitlen_d, NUMBER_OF_DISTANCE_SYMBOLS, 15);

		break; /* end of while (!err) */
	}
	HuffmanTree_cleanup(&cl_tree);
	return err;
}

/* get the dist_tree with fixed huffman */
static int build_fixed_dist_tree(HuffmanTree *tree) {
	int i;
	int bitlen[NUMBER_OF_DISTANCE_SYMBOLS];

	for (i = 0; i != NUMBER_OF_DISTANCE_SYMBOLS; i++) {
		bitlen[i] = 5;
	}

	return make_tree_first_step(tree, bitlen, NUMBER_OF_DISTANCE_SYMBOLS, 15);
}

/* get the ll_tree with fixed huffman */
static int build_fixed_ll_tree(HuffmanTree *tree) {
	int i, err = 0;
	int bitlen[NUMBER_OF_CODE_SYMBOLS];
	/* fill bit lengths according unpacked values */
	for (i = 0; i <= 143; i++) { /* codes from 00110000 to 10111111 */
		bitlen[i] = 8;
	}

	for (i = 144; i <= 255; i++) { /* codes from 110010000 to 111111111 */
		bitlen[i] = 9;
	}

	for (i = 256; i <= 279; i++) { /* codes from 0000000 to 0010111 */
		bitlen[i] = 7;
	}

	for (i = 280; i <= 287; i++) { /* codes from 11000000 to 11000111 */
		bitlen[i] = 8;
	}

	err = make_tree_first_step(tree, bitlen, NUMBER_OF_CODE_SYMBOLS, 15);
	return err;
}

/* get the trees with fixed huffman blocks */
static int build_fixed_trees(HuffmanTree *ll_tree, HuffmanTree *dist_tree) {
	int err;
	err = build_fixed_ll_tree(ll_tree);
	if (!err) {
		err = build_fixed_dist_tree(dist_tree);
	}
	return err;
}

static int inflate_huffman(uint8_t *out, uint8_t *in, int *bp,
								int *pos, int in_length, int btype) {
	int err = 0;
	HuffmanTree ll_tree; /* literal and length codes */
	HuffmanTree dist_tree; /* distance codes */
	int in_bitlen = in_length * 8;
	int l_code, length_base; /* ll code and length */
	int dist_code, dist; /* distance code and distance */
	int addit_bits_ll, addit_bits_d; /* extra bits for length and distance */
	int first_pos, forw, back;

	/* build Huffman trees */
	HuffmanTree_init(&ll_tree);
	HuffmanTree_init(&dist_tree);

	if (btype == 1) {
		err = build_fixed_trees(&ll_tree, &dist_tree);
	} else if (btype == 2) {
		err = build_dynamic_trees(&ll_tree, &dist_tree, in, bp, in_length);
	}

	while (true) { /* decode symbols until error or end */
		l_code = huffman_decode_symbol(in, bp, &ll_tree, in_bitlen);  /* lit symbol */

		if (l_code <= 255) {
			out[*pos] = (uint8_t)l_code;
			(*pos)++;
		} else if (l_code >= FIRST_LENGTH_INDEX && l_code <= LAST_LENGTH_INDEX) {

			/* step 1: get length_base */
			length_base = LENGTH_BASE[l_code - FIRST_LENGTH_INDEX];

			/* step 2: get additional bits and add them value to length_base */
			addit_bits_ll = LENGTH_ADDITIONAL[l_code - FIRST_LENGTH_INDEX];
			length_base += read_bits_from_stream(bp, in, addit_bits_ll);

			/* step 3: get distance code and distance */
			dist_code = huffman_decode_symbol(in, bp, &dist_tree, in_bitlen);

			if (dist_code == -1) { /* decode_symbol return -1 */
				err = 2; /* TODO table of errors */
				break;
			}

			if (dist_code > 29) { /* distance code cant be 30-32 */
				err = 3;
				break;
			}
			dist = DISTANCE_BASE[dist_code];

			/* step 4: get extra bits from distance */
			addit_bits_d = DISTANCE_ADDITIONAL[dist_code];
			dist += read_bits_from_stream(bp, in, addit_bits_d);

			/* step 5: fill in all the out[n] values based on the length and dist */
			first_pos = (*pos);
			back = first_pos - dist;
			if (dist < length_base) {
				for (forw = 0; forw < length_base; forw++) {
					out[(*pos)++] = out[back++];
				}
			} else {
				memcpy(out + *pos, out + back, length_base);
				*pos += length_base;
			}

		} else if (l_code == 256) {
			break; /* end of code */

		} else if (l_code == -1) { /* decodesymbol return -1 */
			err = 2;
			break;
		}
	}
	HuffmanTree_cleanup(&ll_tree);
	HuffmanTree_cleanup(&dist_tree);

	return err;
}

static uint8_t predictor(short a, short b, short c) {
	short pa = abs(b - c);
	short pb = abs(a - c);
	short pc = abs(a + b - c - c);

	if (pc < pa && pc < pb) {
		return (uint8_t)c;
	} else if (pb < pa) {
		return (uint8_t)b;
	} else {
		return (uint8_t)a;
	}
}

static int unfilter_second_step(uint8_t *recon, const uint8_t *scanline,
	const uint8_t *precon, int bytewidth, uint8_t filterType, int length) {

	int i;
	switch(filterType) {
		case 0:
			for (i = 0; i != length; i++) {
				recon[i] = scanline[i];
			}
			break;

		case 1:
			for (i = 0; i != bytewidth; i++) {
				recon[i] = scanline[i];
			}

			for (i = bytewidth; i < length; i++) {
				recon[i] = scanline[i] + recon[i - bytewidth];
			}
			break;

		case 2:
			if (precon) {
				for (i = 0; i != length; i++) {
					recon[i] = scanline[i] + precon[i];
				}
			} else {
				for (i = 0; i != length; i++) {
					recon[i] = scanline[i];
				}
			}
			break;

		case 3:
			if (precon) {
				for (i = 0; i != bytewidth; i++) {
					recon[i] = scanline[i] + (precon[i] >> 1);
				}
				for (i = bytewidth; i < length; i++) {
					recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) >> 1);
				}
			} else {
				for (i = 0; i != bytewidth; i++) {
					recon[i] = scanline[i];
				}
				for (i = bytewidth; i < length; i++) {
					recon[i] = scanline[i] + (recon[i - bytewidth] >> 1);
				}
			}
			break;

		case 4:
			if (precon) {
				for (i = 0; i != bytewidth; i++) {
					recon[i] = (scanline[i] + precon[i]);
				}
				for (i = bytewidth; i < length; i++) {
					recon[i] = (scanline[i] + predictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
				}
			} else {
				for (i = 0; i != bytewidth; i++) {
					recon[i] = scanline[i];
				}
				for (i = bytewidth; i < length; i++) {
					recon[i] = (scanline[i] + recon[i - bytewidth]);
				}
			}
			break;

		default:
			return -1; /* error in filter */
		}
		return 0;
}

static int unfilter_first_step(uint8_t *out, const uint8_t *in, int w, int h, int bpp) {
	int i;
	uint8_t* prevline = 0;

	int bytewidth = (bpp + 7) / 8;
	int linebytes = (w * bpp + 7) / 8;

	for (i = 0; i < h; i++) {
		int outindex = linebytes * i;
		int inindex = (1 + linebytes) * i;
		uint8_t filterType = in[inindex];

		if (unfilter_second_step(&out[outindex], &in[inindex + 1],
			prevline, bytewidth, filterType, linebytes) == -1) {
				log_error("Error in filter");
				return -1;
			}

		prevline = &out[outindex];
	}
	return 0;
}

/**
 * 1)This function converts the filtered-padded-interlaced data in img
 * 2)out: big enough for contain full image,
 * 3)in: contain the full decompressed data from
 * the IDAT chunks (with filter index bytes and possible padding bits)
 * 4)Steps:
 * a) if no Adam7: 1) unfilter 2) TODO: remove padding bits (if bpp < 8)
 * b) TODO: if adam7: 1) 7x unfilter 2) 7x remove padding bits 3) Adam7_deinterlace
 * return 0 - success or error
*/
static int after_decode_unfilter(uint8_t *out, uint8_t *in, int w, int h, int interflace, int bpp) {
	if (interflace == 0) {
		/* TODO removePaddingBits */
		unfilter_first_step(out, in, w, h, bpp);
	} else {
		log_error("Adam7 not supported.");
		return -1;
	}
	return 0;
}

/**
 * this function start of decoding IDAT
 * out : buffer for decoded image, big enough for img
 * in : contain IDAT bytes
 * inlen : length of in(without zlib_head)
*/
static int inflate(uint8_t *out, uint8_t *in, int inlen, struct png_header *png_head) {
	int i, bp = 0;/* bit point in in */
	int pos = 0;/*pos in out */
	int btype;/* huffman type (1 - fixed/2 - dynamic) */
	int bpp;
	int final = 0;

	uint8_t *Huffman_out = (uint8_t*)malloc(png_head->width * png_head->height * 5);
	if (Huffman_out == NULL) {
		log_error("Malloc error.");
		return ENOMEM;
	}

	while(!final) {
		final = (uint8_t)(READBIT(bp, in));
		bp++;
		btype = 1u * (int)(READBIT(bp, in));
		bp++;
		btype += 2u * (int)(READBIT(bp, in));
		bp++;
		if (inflate_huffman(Huffman_out, in, &bp, &pos, inlen, btype) != 0) {
			log_error("Error : can't decode IDAT chunk.");
			free(Huffman_out);
			return -1;
		}
	}
	bpp = png_head->color_type * png_head->bit_depth;

	if (png_head->color_type == PNG_GRSA) {
		uint8_t *out_greyscale = (uint8_t*)malloc(png_head->width * png_head->height * PNG_GRSA);

		if (after_decode_unfilter(out_greyscale, Huffman_out, png_head->width, png_head->height,
													png_head->interlace_method, bpp) != 0) {
			log_error("Error while unfilter");
			free(out_greyscale);
			free(Huffman_out);
			return -1;
		}

		for (i = 0; i < png_head->width * png_head->height; i++, out += 4) {
			out[0] = out[1] = out[2] = out_greyscale[i * 2];
			out[3] = out_greyscale[i * 2 + 1];
		}
		free(out_greyscale);
	} else {
		if (after_decode_unfilter(out, Huffman_out, png_head->width, png_head->height,
												png_head->interlace_method, bpp) != 0) {
			log_error("Error while unfilter");
			free(Huffman_out);
			return -1;
		}
	}
	free(Huffman_out);
	return 0;
}



/* return pos of chunk if list of chunks or -1 if error */
static int ident_chunk(uint8_t *chunk_name) {
	int pos = 0;

	while(pos < 19) {
		if (memcmp(list_of_chunks, chunk_name, 4) == 0) {
			list_of_chunks -= pos * 4;
			return pos;
		}
		list_of_chunks += 4;
		pos++;
	}
	return -1;
}

static int count_IDAT(int fd) {
	int chunk_num, pos = IHDR_END, idat_size = 0;
	uint8_t chunk_name[4];
	uint32_t chunk_length;

	while (true) {
		lseek(fd, pos, SEEK_SET);
		if (read(fd, &chunk_length, sizeof(chunk_length)) != sizeof(chunk_length)) {
			log_error("Can't read chunk.");
			return -1;
		}
		if (read(fd, chunk_name, sizeof(chunk_name)) != sizeof(chunk_name)) {
			log_error("Can't read chunk.");
			return -1;
		}

		chunk_num = ident_chunk(chunk_name);

		if (chunk_num == 1) {
			/* IEND chunk */
			return idat_size;
		}
		switch (chunk_num) {
			case 0:
				/* IDAT chunk */
				idat_size += (int)htonl(chunk_length);
				break;

			case -1:
				/* invalid chunk */
				log_error("Error : invalid png file.");
				return -1;

			default:
				/*one of not important chunks */
				break;
		}

		pos += CHUNK_INFO_LEN + (int)htonl(chunk_length);
	}
}

/* Read IHDR chunk and get info */
static int read_IHDR(int fd, struct png_header *png_head) {

	/* read and check IHDR data */
	if (read(fd, png_head, IHDR_SIZE) != IHDR_SIZE) {
		log_error("Error : can't read IHDR chunk.");
		return -1;
	}
	png_head->width = (int)htonl(png_head->width);
	png_head->height = (int)htonl(png_head->height);

	switch (png_head->color_type) {
		case 1:
			log_error("pallete images are not supporting.");
			return -1;
		case 2:
			png_head->color_type = PNG_TRCL;
			break;

		case 4:
			png_head->color_type = PNG_GRSA;
			break;

		case 6:
			png_head->color_type = PNG_TRCLA;
			break;

		default:
			log_error("Error : invalid color type.");
			return -1;
		}

	if (png_head->compression_method != 0) {
		log_error("Error : invalid compression method.");
		return -1;
	}

	if (png_head->filter_method != 0) {
		log_error("Error : invalid filter method.");
		return -1;
	}

	if (png_head->interlace_method != 0 && png_head->interlace_method != 1) {
		log_error("Error : invalid interlace method");
		return -1;
	}
	return 0;
}

/* Now lib support main chunks and this fun is stub for work with other chunks*/
static void read_chunk(int fd, uint8_t *chunk_name) {
	return;
}

static int png_read(int fd, struct png *png_data) {
	struct png_header png_head;
	uint32_t chunk_length;
	uint8_t *idat_buffer, *idat_data, chunk_name[4], zlib[2];
	int chunk_num, i, idat_size, idat_oldsize = 0, idat_addsize, pos = IHDR_END;

	/**
	 * IDAT_flag
	 * 1 - one IDAT was found
	 * 0 - IDAT not yet met
	 */
	int flag_IDAT = 0;

	/* read IHDR chunk and put file info in struct png_header */
	if (read(fd, &chunk_length, sizeof(chunk_length)) != sizeof(chunk_length)) {
		log_error("Error : can't read IHDR chunk.");
		return -1;
	}

	if ((int)htonl(chunk_length) != IHDR_SIZE) {
		log_error("Error : invalid length of IHDR data.");
		return -1;
	}

	if (read(fd, chunk_name, sizeof(chunk_name)) != sizeof(chunk_name)) {
		log_error("Error : can't read IHDR chunk.");
		return -1;
	}

	if (memcmp(chunk_name, "IHDR", sizeof(chunk_name)) != 0) {
		log_error("Error : invalid IHDR chunk.");
		return -1;
	}

	if (read_IHDR(fd, &png_head) != 0) {
		return -1;
	}

	png_data->width = png_head.width;
	png_data->height = png_head.height;
	png_data->color = png_head.color_type;
	if (png_head.color_type == PNG_TRCL) {
		png_data->image_size = png_head.width * png_head.height * PNG_TRCL;
	} else {
		png_data->image_size = png_head.width * png_head.height * PNG_TRCLA;
	}
	/* malloc mem for image */
	png_data->image = (uint8_t*)malloc(png_data->image_size);
	if (png_data->image == NULL) {
		log_error("Malloc error.");
		return ENOMEM;
	}

	idat_size = count_IDAT(fd);
	idat_data = (uint8_t*)malloc(idat_size);
	if (idat_data == NULL) {
		log_error("Malloc error.");
		return ENOMEM;
	}

	/**
	 * Reading chunks until IEND
	 * its simple version and support just important chunks
	 * fun read_chunk is a stub
	 * TODO add reading of other chunks
	 */
	while (true) {
		lseek(fd, pos, SEEK_SET);
		if (read(fd, &chunk_length, sizeof(chunk_length)) != sizeof(chunk_length)) {
			log_error("Error : can't read chunk.");
			free(png_data->image);
			free(idat_data);
			return -1;
		}
		if (read(fd, chunk_name, sizeof(chunk_name)) != sizeof(chunk_name)) {
			log_error("Error : can't read chunk.");
			free(png_data->image);
			free(idat_data);
			return -1;
		}

		chunk_num = ident_chunk(chunk_name);

		if (chunk_num == 1) {
			/* IEND chunk */
			if (inflate(png_data->image, idat_data, idat_oldsize - sizeof(zlib), &png_head) != 0) {
				log_error("Error while decode IDAT.");
				free(png_data->image);
				free(idat_data);
				return -1;
			}
			if (!flag_IDAT) {
				log_error("Error : there is no IDAT chunk in the file");
				free(png_data->image);
				free(idat_data);
				return -1;
			}
			break;
		}

		switch (chunk_num) {
			case 0:
				if (!flag_IDAT) {
					if (read(fd, zlib, sizeof(zlib)) != sizeof(zlib)) {
						log_error("Error : can't read chunk.");
						free(png_data->image);
						free(idat_data);
						return -1;
					}
					idat_addsize = (int)htonl(chunk_length) - 2;
				} else {
					idat_addsize = (int)htonl(chunk_length);
				}
				idat_buffer = (uint8_t*)malloc(idat_addsize);
				if (idat_buffer == NULL) {
					log_error("Malloc error.");
					free(png_data->image);
					free(idat_data);
					return ENOMEM;
				}

				if (read(fd, idat_buffer, idat_addsize) != idat_addsize) {
					log_error("Error : can't read chunk.");
					free(png_data->image);
					free(idat_data);
					free(idat_buffer);
					return -1;
				}

				for (i = 0; i < idat_addsize; i++) {
					idat_data[idat_oldsize + i] = idat_buffer[i];
				}
				idat_oldsize += idat_addsize;
				free(idat_buffer);
				flag_IDAT = 1;

				break;

			case -1:
				if (flag_IDAT == 1) {
					log_error("Warning : IDAT was successfull decoded, but IEND was not found");
					free(idat_data);
					return 0;
				}
				log_error("Error : invalid png file.");
				free(png_data->image);
				free(idat_data);
				return -1;

			default:
				read_chunk(fd, chunk_name);
				break;
		}

		pos += CHUNK_INFO_LEN + (int)htonl(chunk_length);
	}
	free(idat_data);
	return 0;
}

int png_load(char *file_name, struct png *png_data) {

	uint8_t signature[8];

	if (file_name == NULL) {
		return EINVAL;
	}

	if (png_data == NULL) {
		return EINVAL;
	}

	int fd = open(file_name, O_RDONLY);
	if (fd == -1) {
		log_error("Error : can't open file.");
		return -1;
	}

	if (read(fd, signature, sizeof(signature)) != sizeof(signature)) {
		log_error("Error : can't read signature of file.");
		close(fd);
		return -1;
	}

	if (raw_get_file_format(signature) != PNG_FILE) {
		log_error("Error : it's not PNG file");
		close(fd);
		return -1;
	}

	if (png_read(fd, png_data) != 0) {
		close(fd);
		return -1;
	}
	return 0;
}

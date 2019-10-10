/**
 * @file
 * @brief Detect file format by header/etc
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-12-10
 */

#define FILE_HEADER_LEN 64

#include <stdio.h>
#include <stdint.h>

enum fformat {
	ERR_FILE = -1,
	TEXT_FILE,
	RIFF_FILE,
	BMP_FILE,
	PNG_FILE
};

/* http://soundfile.sapp.org/doc/WaveFormat/ */
struct wave_header {
	/* RIFF header */
	uint8_t  ChunkID[4];     /* "RIFF" in ASCII */
	uint32_t ChunkSize;
	uint8_t  Format[4];      /* "WAVE" in ASCII */
	/* fmt subchunk */
	uint8_t  Subchunk1ID[4]; /* "fmt " in ASCII */
	uint32_t Subchunk1Size;  /* 16 for PCM */
	uint16_t AudioFormat;    /* 1 for PCM */
	uint16_t NumChannels;    /* 1 for mono, 2 for stereo, etc. */
	uint32_t SampleRate;     /* 8000, 44100, etc */
	uint32_t ByteRate;       /* == SampleRate * NumChannels * BitsPerSample/8 */
	uint16_t BlockAlign;	 /* == NumChannels * BitsPerSample/8 */
	uint16_t BitsPerSample;
	/* data subchunk */
	uint8_t Subchunk2ID[4];    /* "data" in ASCII */
	uint32_t Subchunk2Size;  /* == NumSamples * NumChannels * BitsPerSample/8;
				    also this is actual size of following data in bytes */
	/* The actual sound data follows */
};

/* Fill default values for WAVE format header */
static inline void _wave_header_fill(struct wave_header *hdr, int nchan,
					int sample_rate, int bits_per_sample, int len) {
	*hdr = (struct wave_header) {
		.ChunkID       = "RIFF",
		.ChunkSize     = 4 + (8 + 16) + (8 + len),
		.Format        = "WAVE",
		.Subchunk1ID   = "fmt ",
		.Subchunk1Size = 16,
		.AudioFormat   = 1,
		.NumChannels   = (uint16_t) nchan,
		.SampleRate    = (uint32_t) sample_rate,
		.ByteRate      = (uint32_t) (sample_rate * nchan * bits_per_sample / 8),
		.BitsPerSample = bits_per_sample,
		.Subchunk2ID   = "data",
		.Subchunk2Size = len
	};
}

extern enum fformat raw_get_file_format(uint8_t *file);
extern enum fformat libc_get_file_format(FILE *file);
extern enum fformat posix_get_file_format(int fd);

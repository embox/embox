/**
 * @file
 * @brief Simple audio player
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-26
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <drivers/audio/portaudio.h>
#include <fs/file_format.h>

static void print_usage(void) {
	printf("Usage: play [WAVAUDIOFILE]\n"
	       "       play -s\n");
}

double _sin(double x) {
	if (x > 0) {
		while (x > 3.14)
			x -= 3.14;
	} else {
		while (x < -3.14)
			x += 3.14;
	}
	return x - x * x * x / 6. + x * x * x * x * x / 120.;
}

static int _sin_w = 10;
static int _sin_h = 30000;
static int sin_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	uint16_t *data;

	data = outputBuffer;

	for (int i = 0; i < framesPerBuffer; i++) {
		double x = (i % _sin_w) / _sin_w * 3.14;
		*data++ = (int) (_sin(x) * _sin_h); /* Left channel  */
		*data++ = (int) (_sin(x) * _sin_h); /* Right channel */
	}

	return 0;
}

static int fd_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
        uint32_t read_bytes;
	FILE *file;

	file = userData;

	read_bytes = framesPerBuffer * 2 * 2; /* Stereo 16-bit */

	fread(outputBuffer, 1, read_bytes, file);

	return 0;
}

int main(int argc, char **argv) {
	int opt;
	int err;
	FILE *fd;
	static uint8_t fmt_buf[128];
	int chan_n;
	int sample_rate;
	int bits_per_sample;
	PaStreamCallback *callback;
	PaStream *stream = NULL;

	struct PaStreamParameters out_par;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	callback = &fd_callback;

	while (-1 != (opt = getopt(argc, argv, "nsh"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's':
			callback = &sin_callback;
			break;
		default:
			printf("Unknown argument: %c", opt);
			return 0;
		}
	}

	if (callback == fd_callback) {
		if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
			printf("Can't open file %s\n", argv[argc - 1]);
			return 0;
		}

		fread(fmt_buf, 1, 44, fd);
		if (raw_get_file_format(fmt_buf) != RIFF_FILE) {
			printf("%s is not a RIFF audio file\n", argv[argc - 1]);
			return 0;
		}

		chan_n          = *((uint16_t*) &fmt_buf[22]);
		sample_rate     = *((uint32_t*) &fmt_buf[24]);
		bits_per_sample = *((uint16_t*) &fmt_buf[34]);

		printf("File size:             %d bytes\n", *((uint32_t*) &fmt_buf[4]));
		printf("File type header:      %c%c%c%c\n", fmt_buf[8], fmt_buf[9], fmt_buf[10], fmt_buf[11]);
		printf("Length of format data: %d\n", *((uint32_t*) &fmt_buf[16]));
		printf("Type format:           %d\n", *((uint16_t*) &fmt_buf[20]));
		printf("Number of channels:    %d\n", chan_n);
		printf("Sample rate:           %d\n", sample_rate);
		printf("Bits per sample:       %d\n", bits_per_sample);
		printf("Size of data section:  %d\n", *((uint32_t*) &fmt_buf[40]));
	}
	/* Initialize PA */
	if (paNoError != (err = Pa_Initialize())) {
		printf("Portaudio error: could not initialize!\n");
		goto err_close_fd;
	}

	out_par = (PaStreamParameters) {
		.device                    = 0,
		.channelCount              = chan_n,
		.sampleFormat              = paInt16,
		.suggestedLatency          = 10,
		.hostApiSpecificStreamInfo = NULL,
	};

	err = Pa_OpenStream(&stream,
			NULL,
			&out_par,
			sample_rate,
			256,
			0,
			callback,
			fd);

	if (err != paNoError) {
		printf("Portaudio error: could not open stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_StartStream(stream))) {
		printf("Portaudio error: could not start stream!\n");
		goto err_terminate_pa;
	}

	Pa_Sleep(10000); /* Wait 10 secs */

	if (paNoError != (err = Pa_StopStream(stream))) {
		printf("Portaudio error: could not stop stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_CloseStream(stream))) {
		printf("Portaudio error: could not close stream!\n");
		goto err_terminate_pa;
	}

err_terminate_pa:
	if (paNoError != (err = Pa_Terminate()))
		printf("Portaudio error: could not terminate!\n");

err_close_fd:
	fclose(fd);
	return 0;
}

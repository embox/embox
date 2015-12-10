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
	printf("Usage: play [WAVAUDIOFILE]\n");
}

int paCallback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	return 0;
}

#define SAMPLE_RATE 44100

int main(int argc, char **argv) {
	int opt;
	int err;
	FILE *fd;
	PaStream *stream = NULL;

	struct PaStreamParameters out_par;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc - 1, argv, "nh"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			printf("Unknown argument: %c", opt);
			return 0;
		}
	}

	if (NULL == (fd = fopen(argv[argc - 1], "r"))) {
		printf("Can't open file %s\n", argv[argc - 1]);
		return 0;
	}

	if (libc_get_file_format(fd) != RIFF_FILE) {
		printf("%s is not a RIFF audio file\n", argv[argc - 1]);
		return 0;
	}

	/* Initialize PA */
	if (paNoError != (err = Pa_Initialize())) {
		printf("Portaudio error: could not initialize!\n");
		goto err_close_fd;
	}

	out_par = (PaStreamParameters) {
		.device                    = 0,
		.channelCount              = 1,
		.sampleFormat              = paFloat32,
		.suggestedLatency          = 10,
		.hostApiSpecificStreamInfo = NULL,
	};

	err = Pa_OpenStream(&stream,
			NULL,
			&out_par,
			SAMPLE_RATE,
			256,
			0,
			paCallback,
			NULL);

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

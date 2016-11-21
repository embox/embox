/**
 * @file
 * @brief Simple audio recorder
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-11-21
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <drivers/audio/portaudio.h>
#include <fs/file_format.h>
#include <util/math.h>

static void print_usage(void) {
	printf("Usage: record [FILENAME]\n");
}

static void write_wave(char *name, uint8_t *buf, int len) {
	struct wave_header hdr;
	int fd = open(name, O_CREAT);
	
	_wave_header_fill(&hdr, 1, 44100, 16, len);

	write(fd, &hdr, sizeof(hdr));
	write(fd, buf, len);
}

static uint8_t in_buf[1024 * 256];
static int cur_ptr;

static int record_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	printf("Much recording, wow\n");
	for (int i = 0; i < 256; i++) {
		printf("%x ", ((int*) inputBuffer)[i]);
	}
	printf("\n");

	for (int i = 0; i < framesPerBuffer; i++) {
		in_buf[cur_ptr + i] = ((int*)inputBuffer)[i];
	}
	cur_ptr += framesPerBuffer;

	return paContinue;
}
	

int main(int argc, char **argv) {
	int opt;
	int err;
	int sample_rate = 44100;
	int sleep_msec = 100000;
	PaStream *stream = NULL;

	struct PaStreamParameters in_par;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "nsh"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			printf("Unknown argument: %c", opt);
			return 0;
		}
	}

	cur_ptr = 0;
	/* Initialize PA */
	if (paNoError != (err = Pa_Initialize())) {
		printf("Portaudio error: could not initialize!\n");
		goto err_exit;
	}

	in_par = (PaStreamParameters) {
		.device                    = 0,
		.channelCount              = 1,
		.sampleFormat              = paInt16,
		.suggestedLatency          = 10,
		.hostApiSpecificStreamInfo = NULL,
	};

	err = Pa_OpenStream(&stream,
			&in_par,
			NULL,
			sample_rate,
			256 * 1024 / 4,
			0,
			NULL,
			record_callback);

	if (err != paNoError) {
		printf("Portaudio error: could not open stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_StartStream(stream))) {
		printf("Portaudio error: could not start stream!\n");
		goto err_terminate_pa;
	}

	printf("Recording! Speak to the microphone\n");

	Pa_Sleep(sleep_msec);

	if (paNoError != (err = Pa_StopStream(stream))) {
		printf("Portaudio error: could not stop stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_CloseStream(stream))) {
		printf("Portaudio error: could not close stream!\n");
		goto err_terminate_pa;
	}

	write_wave(argv[1], in_buf, sizeof(in_buf));

err_terminate_pa:
	if (paNoError != (err = Pa_Terminate()))
		printf("Portaudio error: could not terminate!\n");
err_exit:
	return 0;
}

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

int main(int argc, char **argv) {
	int opt;
	int err;
	FILE *fd;
	static uint8_t snd_buf[128 * 1024];
	int chan_n;
	int sample_rate;
	int bits_per_sample;

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

	fread(snd_buf, 128, 1024, fd);
	if (raw_get_file_format(snd_buf) != RIFF_FILE) {
		printf("%s is not a RIFF audio file\n", argv[argc - 1]);
		return 0;
	}

	chan_n          = *((uint16_t*) &snd_buf[22]);
	sample_rate     = *((uint32_t*) &snd_buf[24]);
	bits_per_sample = *((uint16_t*) &snd_buf[34]);

	printf("File size:             %d bytes\n", *((uint32_t*) &snd_buf[4]));
	printf("File type header:      %c%c%c%c\n", snd_buf[8], snd_buf[9], snd_buf[10], snd_buf[11]);
	printf("Length of format data: %d\n", *((uint32_t*) &snd_buf[16]));
	printf("Type format:           %d\n", *((uint16_t*) &snd_buf[20]));
	printf("Number of channels:    %d\n", chan_n);
	printf("Sample rate:           %d\n", sample_rate);
	printf("Bits per sample:       %d\n", bits_per_sample);
	printf("Size of data section:  %d\n", *((uint32_t*) &snd_buf[40]));

	/* Initialize PA */
	if (paNoError != (err = Pa_Initialize())) {
		printf("Portaudio error: could not initialize!\n");
		goto err_close_fd;
	}

	out_par = (PaStreamParameters) {
		.device                    = 0,
		.channelCount              = chan_n,
		.sampleFormat              = paFloat32,
		.suggestedLatency          = 10,
		.hostApiSpecificStreamInfo = NULL,
	};

	err = Pa_OpenStream(&stream,
			NULL,
			&out_par,
			sample_rate,
			256,
			0,
			paCallback,
			&snd_buf[44]);

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

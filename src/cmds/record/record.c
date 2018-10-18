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
	int fd = open(name, O_CREAT | O_RDWR);

	_wave_header_fill(&hdr, 2, 44100, 16, len);

	write(fd, &hdr, sizeof(hdr));
	write(fd, buf, len);
}

static uint32_t in_buf[4 * 2 * 32 / 4 * 0xff00]; /* Hardcoded 4x intel-ac buffer size */
static int cur_ptr;

double _sin(double x) {
	double m = 1.;
	while (x > 2. * 3.14)
		x -= 2. * 3.14;

	if (x > 3.14) {
		x -= 3.14;
		m = -1.;
	}
	return m * (x - x * x * x / 6. + x * x * x * x * x / 120.
		- x * x * x * x * x * x * x / (120 * 6 * 7)
		+ x * x * x * x * x * x * x * x * x/ (120 * 6 * 7 * 8 * 9));
}

static int _sin_w = 100;
static int _sin_h = 30000;
static int sin_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	int i;

	for (i = 0; i < framesPerBuffer; i++) {
		double x = 2 * 3.14 * (i % _sin_w) / _sin_w;
		int tmp = (1. + _sin(x)) * _sin_h;
		in_buf[cur_ptr] = (tmp << 16) | tmp;
		cur_ptr++;
	}

	return 0;
}

static int record_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	int i;

	framesPerBuffer = 2 * 32 / 4 * 0xff00; /* XXX */

	for (i = 0; i < framesPerBuffer; i++) {
		if (cur_ptr > sizeof(in_buf))
			break;
		in_buf[cur_ptr] = ((int*)inputBuffer)[i];
		cur_ptr++;
	}

	if (cur_ptr * 4 > sizeof(in_buf))
		return paComplete;
	return paContinue;
}


int main(int argc, char **argv) {
	int opt;
	int err;
	int sample_rate = 44100;
	int sleep_msec = 50000;
	char *filename;
	PaStream *stream = NULL;

	struct PaStreamParameters in_par;
	PaStreamCallback *callback;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	callback = &record_callback;
	filename = argv[1];

	while (-1 != (opt = getopt(argc, argv, "nsh"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's':
			callback = &sin_callback;
			filename = argv[2];
			break;
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
		.device                    = Pa_GetDefaultInputDevice(),
		.channelCount              = 2,
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
			callback,
			NULL);

	if (err != paNoError) {
		printf("Portaudio error: could not open stream!\n");
		goto err_terminate_pa;
	}

	if (callback == record_callback) {
		printf("Recording! Speak to the microphone\n");
	} else if (callback == sin_callback) {
		printf("Recording sin()! Instead of getting sound from your microphone\n"
				"sin will be recorded to the output file\n");
	}

	if (paNoError != (err = Pa_StartStream(stream))) {
		printf("Portaudio error: could not start stream!\n");
		goto err_terminate_pa;
	}

	Pa_Sleep(sleep_msec);

	if (paNoError != (err = Pa_StopStream(stream))) {
		printf("Portaudio error: could not stop stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_CloseStream(stream))) {
		printf("Portaudio error: could not close stream!\n");
		goto err_terminate_pa;
	}

	write_wave(filename, (uint8_t*)in_buf, cur_ptr * 4);

err_terminate_pa:
	if (paNoError != (err = Pa_Terminate()))
		printf("Portaudio error: could not terminate!\n");
err_exit:
	return 0;
}

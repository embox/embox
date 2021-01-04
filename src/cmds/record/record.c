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
#include <assert.h>
#include <stdlib.h>

#include <portaudio.h>
#include <fs/file_format.h>
#include <util/math.h>
#include <framework/mod/options.h>

#define USE_LOCAL_BUFFER OPTION_GET(BOOLEAN, use_local_buffer)

/* Maximum recording duration in milliseconds */
#define MAX_REC_DURATION 50000
#define AUDIO_BUFFER_SIZE 0x800000

static int sample_rate;
static int chan_n;

static void print_usage(void) {
	printf("Usage: record [flags] [FILENAME]\n"
			"-r - sample rate (8000, 16000,...)\n"
			"-c - channel count (1 or 2)\n"
			"-d - recording duration in msec\n"
			"-s - \"record\" sin() instead of using microphone\n"
			"-m - record to memory instead of file \n"
			"Examples:\n"
			"  record stereo44100.wav - record stereo44100.wav with default settings\n"
			"  record -r 8000 -c 1 -d 10000 mono8000.wav - record 10 sec mono 8000hz\n"
			"  record -s sin.wav - record sin() to sin.wav\n"
			"  record -m C0000000 - record to 0xC0000000. After you can do \"play -m\"\n"
		);
}

static void write_wave_file(char *name, uint8_t *buf, int len) {
	struct wave_header hdr;
	int fd = open(name, O_CREAT | O_RDWR);

	_wave_header_fill(&hdr, chan_n, sample_rate, 16, len);

	write(fd, &hdr, sizeof(hdr));
	write(fd, buf, len);
}

static void write_wave_addr(uint32_t addr, uint8_t *buf, int len) {
	struct wave_header hdr;

	_wave_header_fill(&hdr, chan_n, sample_rate, 16, len);

	/* Since we already have audio in memory just prepend it with header */
	memcpy((void *) addr, &hdr, sizeof(hdr));
}

#define AUDIO_ADDR_UNINITIALIZED ((uint32_t) -1)

static uint32_t audio_memory_addr = AUDIO_ADDR_UNINITIALIZED;

#if USE_LOCAL_BUFFER
static uint16_t audio_buf[AUDIO_BUFFER_SIZE];
static uint16_t *in_buf = audio_buf;
#else
static uint16_t *in_buf = NULL;
#endif

static int cur_ptr;

static double _sin(double x) {
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

		in_buf[cur_ptr++] = tmp;
		if (chan_n == 2) {
			in_buf[cur_ptr++] = tmp;
		}
	}

	return 0;
}

static int record_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	int i;
	uint16_t *in_data16 = (uint16_t *)inputBuffer;

	assert(in_data16 && in_buf);

	for (i = 0; i < framesPerBuffer; i++) {
		if (cur_ptr > AUDIO_BUFFER_SIZE) {
			break;
		}
		memcpy(&in_buf[cur_ptr], &in_data16[chan_n * i], 2 * chan_n);
		cur_ptr += chan_n;
	}

	printf("|");
	if (cur_ptr > AUDIO_BUFFER_SIZE) {
		printf("\n");
		return paComplete;
	}
	return paContinue;
}

int main(int argc, char **argv) {
	int opt;
	int err;
	int devid;
	int sleep_msec = MAX_REC_DURATION;
	char *filename = NULL;
	PaStream *stream = NULL;

	struct PaStreamParameters in_par;
	PaStreamCallback *callback;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	sample_rate = 44100;
	chan_n = 2;
	callback = &record_callback;

	while (-1 != (opt = getopt(argc, argv, "nshd:r:c:m:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's':
			callback = &sin_callback;
			break;
		case 'd':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &sleep_msec))) {
				print_usage();
				return -1;
			}
			if (sleep_msec > MAX_REC_DURATION) {
				printf("Specified recording duration %d exceeds maximum %d\n",
						sleep_msec, MAX_REC_DURATION);
				print_usage();
				return -1;
			}
			break;
		case 'r':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &sample_rate))) {
				print_usage();
				return -1;
			}
			break;
		case 'c':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &chan_n))) {
				print_usage();
				return -1;
			}
			break;
		case 'm':
			if (!optarg) {
				print_usage();
				return -1;
			}
			audio_memory_addr = (uint32_t) strtoul(optarg, NULL, 0);
			in_buf = (uint16_t *) (audio_memory_addr + sizeof (struct wave_header));
			break;
		default:
			printf("Unknown argument: %c", opt);
			return 0;
		}
	}

	if (audio_memory_addr == AUDIO_ADDR_UNINITIALIZED) {
		filename = argv[argc - 1];
	}

	cur_ptr = 0;
	/* Initialize PA */
	if (paNoError != (err = Pa_Initialize())) {
		printf("Portaudio error: could not initialize!\n");
		goto err_exit;
	}

	if (paNoDevice == (devid = Pa_GetDefaultInputDevice())) {
		printf("No default input device!\n");
		goto err_terminate_pa;
	}

	in_par = (PaStreamParameters) {
		.device                    = devid,
		.channelCount              = chan_n,
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

	printf("Recording wav parameters:\n");
	if (audio_memory_addr == AUDIO_ADDR_UNINITIALIZED) {
		printf("  File: %s\n", filename);
	} else {
		printf("  File memory address: 0x%x\n", audio_memory_addr);
	}
	printf("  Duration - %d msec, rate - %d hz, %s wav\n\n",
			sleep_msec, sample_rate, chan_n == 1 ? "mono" : "stereo");

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

	if (audio_memory_addr == AUDIO_ADDR_UNINITIALIZED) {
		write_wave_file(filename, (uint8_t*) in_buf, cur_ptr * 2);
	} else {
		write_wave_addr(audio_memory_addr, (uint8_t*) in_buf, cur_ptr * 2);
	}

err_terminate_pa:
	if (paNoError != (err = Pa_Terminate()))
		printf("Portaudio error: could not terminate!\n");
err_exit:
	return 0;
}

/**
 * @file
 *
 * @date 14.05.2016
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <util/log.h>

#include <kernel/lthread/lthread.h>

#include <drivers/audio/portaudio.h>
#include <drivers/audio/audio_dev.h>

static struct lthread portaudio_lthread;

struct pa_strm {
	uint8_t devid;
	uint8_t number_of_chan;
	uint32_t sample_format;

	PaStreamCallback *callback;
	void *user_data;

	int active;
	int lthread_start;
};

static struct pa_strm pa_stream;

/**
 * @brief Duplicate left channel for the buffer
 */
static void _mono_to_stereo(void *buf, int len) {
	/* Assume data is 16-bit */
	uint16_t *b16 = buf;
	for (int i = len * 2 - 1; i >= 0; i--) {
		b16[i] = b16[i / 2];
	}
}

/**
 * @brief Remove right channel from audio buffer
 */
static void _stereo_to_mono(void *buf, int len) {
	/* Assume data is 16-bit */
	uint16_t *b16 = buf;
	for (int i = 0; i < len / 2; i++) {
		b16[i] = b16[i * 2];
	}
}

static int portaudio_lthread_handle(struct lthread *self) {
	int retval;
	struct audio_dev *audio_dev;
	uint8_t *out_buf;
	uint8_t *in_buf;
	int inp_frames;

	if (!pa_stream.callback || !pa_stream.active) {
		return 0;
	}

	audio_dev = audio_dev_get_by_idx(pa_stream.devid);
	assert(audio_dev);

	out_buf = audio_dev_get_out_cur_ptr(audio_dev);
	in_buf = audio_dev_get_in_cur_ptr(audio_dev);

	inp_frames = audio_dev->buf_len / 2; /* 16 bit sample */
	/* Even if source is mono channel,
	 * we will anyway put twice as much data
	 * to fill right channel as well */
	inp_frames /= 2;

	log_debug("out_buf = 0x%X, buf_len %d", out_buf, audio_dev->buf_len);

	if (out_buf)
		memset(out_buf, 0, audio_dev->buf_len);

	retval = pa_stream.callback(in_buf,
			out_buf,
			inp_frames,
			NULL,
			0,
			pa_stream.user_data);

	/* Sort out problems related to the number of channels */
	if (pa_stream.number_of_chan != audio_dev->num_of_chan) {
		if (pa_stream.number_of_chan == 1 && audio_dev->num_of_chan == 2) {
			_mono_to_stereo(out_buf, inp_frames);
		} else if (pa_stream.number_of_chan == 2 && audio_dev->num_of_chan == 1) {
			_stereo_to_mono(out_buf, inp_frames);
		} else {
			log_error("Audio configuration is broken!"
			          "Check the number of channels.\n");
			return 0;
		}
	}

	if (retval != paContinue)
		pa_stream.active = 0;

	assert(audio_dev->ad_ops && audio_dev->ad_ops->ad_ops_resume);
	audio_dev->ad_ops->ad_ops_resume(audio_dev);

	return 0;
}

PaError Pa_Initialize(void) {
	lthread_init(&portaudio_lthread, portaudio_lthread_handle);
	return paNoError;
}

PaError Pa_Terminate(void) {
	return paNoError;
}

/* XXX Now we support either only input of ouput streams, bot not both at the same time */
PaError Pa_OpenStream(PaStream** stream,
		const PaStreamParameters *inputParameters,
		const PaStreamParameters *outputParameters,
		double sampleRate, unsigned long framesPerBuffer,
		PaStreamFlags streamFlags, PaStreamCallback *streamCallback,
		void *userData) {
	struct audio_dev *audio_dev;

	assert(stream != NULL);
	assert(streamFlags == paNoFlag || streamFlags == paClipOff);
	assert(streamCallback != NULL);

	log_debug("stream %p input %p output %p rate %f"
			" framesPerBuffer %lu flags %lu callback %p user_data %p",
			stream, inputParameters, outputParameters, sampleRate,
			framesPerBuffer, streamFlags, streamCallback, userData);

	if (outputParameters != NULL) {
		pa_stream.lthread_start   = 1;
		pa_stream.number_of_chan  = outputParameters->channelCount;
		pa_stream.devid           = outputParameters->device;
		pa_stream.sample_format   = outputParameters->sampleFormat;
	} else {
		pa_stream.lthread_start   = 0;
		pa_stream.number_of_chan  = inputParameters->channelCount;
		pa_stream.devid           = inputParameters->device;
		pa_stream.sample_format   = inputParameters->sampleFormat;
	}

	pa_stream.callback       = streamCallback;
	pa_stream.user_data      = userData;
	pa_stream.active         = 1;

	*stream = &pa_stream;

	audio_dev = audio_dev_get_by_idx(pa_stream.devid);
	if (audio_dev == NULL)
		return paInvalidDevice;

	assert(audio_dev->ad_ops);
	assert(audio_dev->ad_ops->ad_ops_ioctl);
	audio_dev->buf_len = audio_dev->ad_ops->ad_ops_ioctl(audio_dev, ADIOCTL_BUFLEN, NULL);
	if (audio_dev->buf_len == -1) {
		return paInvalidDevice;
	}


	/* TODO work on mono sound device */
	audio_dev->num_of_chan = 2;

	assert(audio_dev->ad_ops && audio_dev->ad_ops->ad_ops_start);
	audio_dev->ad_ops->ad_ops_start(audio_dev);

	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	/* We don't actually allocate any resources on
	 * Pa_OpenStream, so it's ok to simply stop the stream */
	return Pa_StopStream(stream);
}

PaError Pa_StartStream(PaStream *stream) {
	if (pa_stream.lthread_start) {
		lthread_launch(&portaudio_lthread);
	}

	pa_stream.lthread_start = 1;

	return paNoError;
}

PaError Pa_StopStream(PaStream *stream) {
	struct pa_strm *strm;
	struct audio_dev *audio_dev;

	strm = (struct pa_strm *)stream;

	audio_dev = audio_dev_get_by_idx(strm->devid);
	assert(audio_dev);
	assert(audio_dev->ad_ops);

	if (audio_dev->ad_ops->ad_ops_pause)
		audio_dev->ad_ops->ad_ops_pause(audio_dev);
	else
		log_error("Stream pause not supported!\n");

	if (audio_dev->ad_ops->ad_ops_stop)
		audio_dev->ad_ops->ad_ops_stop(audio_dev);
	else
		log_error("Stream stop not supported!\n");

	return paNoError;
}

void Pa_Sleep(long msec) {
	usleep(msec * 1000);
}

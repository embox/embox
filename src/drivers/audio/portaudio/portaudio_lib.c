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

#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched.h>
#include <kernel/printk.h>
#include <time.h>

#include <drivers/audio/portaudio.h>
#include <drivers/audio/audio_dev.h>

struct pa_strm {
	uint8_t devid;
	uint8_t number_of_chan;
	uint32_t sample_format;

	PaStreamCallback *callback;
	void *user_data;

	int active;
};

static struct thread *pa_thread;
static struct pa_strm pa_stream;

static void _buf_scale(void *buf, int len1, int len2) {
	uint16_t *b16 = buf;
	if (len2 > len1) {
		for (int i = len2 - 1; i >= 0; i--) {
			b16[i] = b16[i * len1 / len2];
		}
	}
}

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

static void *pa_thread_hnd(void *arg) {
	int err;
	struct audio_dev *audio_dev;
	uint8_t *out_buf;
	uint8_t *in_buf;
	int inp_frames;

	audio_dev = audio_dev_get_by_idx(pa_stream.devid);
	assert(audio_dev);
	assert(audio_dev->ad_ops);
	assert(audio_dev->ad_ops->ad_ops_start);
	assert(audio_dev->ad_ops->ad_ops_resume);

	if (!pa_stream.callback) {
		log_debug("No callback provided for PA thread. "
				"That's probably not what you want.");
		return NULL;
	}

	SCHED_WAIT(pa_stream.active);
	audio_dev->ad_ops->ad_ops_start(audio_dev);

	inp_frames = audio_dev->buf_len / 2; /* 16 bit sample */
	/* Even if source is mono channel,
	 * we will anyway put twice as much data
	 * to fill right channel as well */
	inp_frames /= 2;

	while (1) {
		SCHED_WAIT(pa_stream.active);

		out_buf = audio_dev_get_out_cur_ptr(audio_dev);
		in_buf  = audio_dev_get_in_cur_ptr(audio_dev);

		log_debug("out_buf = 0x%X, buf_len %d", out_buf, audio_dev->buf_len);

		if (out_buf)
			memset(out_buf, 0, audio_dev->buf_len);

		if ((err = 0))
			err = pa_stream.callback(in_buf,
				out_buf,
				inp_frames,
				NULL,
				0,
				pa_stream.user_data);

		inp_frames *= 2;
		_mono_to_stereo(out_buf, inp_frames);
		_buf_scale(out_buf, inp_frames, inp_frames);

		/* for (int i = 0; i < 8 * 0x1000 * 32 * 2; i++) {
			((uint8_t*)out_buf)[i] = i % 255;
		} */

		if (err) {
			log_error("User callback error: %d", err);
			if(err == paComplete) {
				Pa_CloseStream(&pa_stream);
			}
		}
		/* Sort out problems related to the number of channels */
		if (pa_stream.number_of_chan != audio_dev->num_of_chan) {
			if (pa_stream.number_of_chan == 1 && audio_dev->num_of_chan == 2) {
				_mono_to_stereo(out_buf, inp_frames);
			} else if (pa_stream.number_of_chan == 2 && audio_dev->num_of_chan == 1) {
				_stereo_to_mono(out_buf, inp_frames);
			} else {
				log_error("Audio configuration is broken!"
					  "Check the number of channels.\n");
				return NULL;
			}
		}

		pa_stream.active = 0;

		audio_dev->ad_ops->ad_ops_resume(audio_dev);
	}

	return NULL;
}

PaError Pa_Initialize(void) {
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
		pa_stream.active          = 1;
		pa_stream.number_of_chan  = outputParameters->channelCount;
		pa_stream.devid           = outputParameters->device;
		pa_stream.sample_format   = outputParameters->sampleFormat;
	} else {
		pa_stream.active          = 0;
		pa_stream.number_of_chan  = inputParameters->channelCount;
		pa_stream.devid           = inputParameters->device;
		pa_stream.sample_format   = inputParameters->sampleFormat;
	}

	pa_stream.callback       = streamCallback;
	pa_stream.user_data      = userData;

	*stream = &pa_stream;

	audio_dev = audio_dev_get_by_idx(pa_stream.devid);
	if (audio_dev == NULL)
		return paInvalidDevice;

	assert(audio_dev->ad_ops);
	assert(audio_dev->ad_ops->ad_ops_ioctl);
	assert(audio_dev->ad_ops->ad_ops_start);

	audio_dev->buf_len = audio_dev->ad_ops->ad_ops_ioctl(audio_dev, ADIOCTL_BUFLEN, NULL);
	if (audio_dev->buf_len == -1) {
		return paInvalidDevice;
	}

	/* TODO work on mono sound device */
	audio_dev->num_of_chan = 2;

	pa_thread = thread_create(0, pa_thread_hnd, NULL);

	return paNoError;
}

PaError Pa_CloseStream(PaStream *stream) {
	/* We don't actually allocate any resources on
	 * Pa_OpenStream, so it's ok to simply stop the stream */
	return Pa_StopStream(stream);
}

PaError Pa_StartStream(PaStream *stream) {
	pa_stream.active = 1;
	sched_wakeup(&pa_thread->schedee);
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

/**
 * @file info.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-06-28
 */

#include <embox/unit.h>

#include <drivers/audio/portaudio.h>
#include <drivers/audio/audio_dev.h>

#include <util/log.h>

#define MAX_DEV_CNT OPTION_GET(NUMBER, max_dev_count)

ARRAY_SPREAD_DECLARE(const struct audio_dev, __audio_device_registry);

static int pa_info_init(void);
EMBOX_UNIT_INIT(pa_info_init);

static int _dev_cnt;
static PaDeviceInfo _info[MAX_DEV_CNT];

static int pa_info_init(void) {
	struct audio_dev *dev;

	_dev_cnt = ARRAY_SPREAD_SIZE(__audio_device_registry);

	if (_dev_cnt > MAX_DEV_CNT) {
		log_error("Increase max_dev_count option!"
		          " Need %d, have %d\n",
		          _dev_cnt, MAX_DEV_CNT);

		_dev_cnt = MAX_DEV_CNT;
	}

	for (int i = 0; i < _dev_cnt; i++) {
		dev = audio_dev_get_by_idx(i);
		_info[i] = (PaDeviceInfo) {
			.structVersion = 1,
			.name = dev->ad_name,
			.hostApi = 0,
			.maxInputChannels = 1,
			.maxOutputChannels = 1,
			.defaultLowInputLatency = 0,
			.defaultLowOutputLatency = 0,
			.defaultHighInputLatency = 0,
			.defaultHighOutputLatency = 0,
			.defaultSampleRate = 44100
		};
	}

	return 0;
}

PaHostApiIndex Pa_GetHostApiCount(void) {
	return 1;
}

PaDeviceIndex Pa_GetDeviceCount(void) {
	return _dev_cnt;
}

PaDeviceIndex Pa_GetDefaultOutputDevice(void) {
	return 0;
}

const char *Pa_GetErrorText(PaError errorCode) {
	return "Pa_GetErrorText not implemented";
}

const PaDeviceInfo * Pa_GetDeviceInfo(PaDeviceIndex device) {
	if (device >= _dev_cnt)
		return NULL;
	else
		return (const PaDeviceInfo *) &_info[device];
}

const PaHostApiInfo * Pa_GetHostApiInfo(PaHostApiIndex hostApi) {
	return NULL;
}

const PaStreamInfo * Pa_GetStreamInfo(PaStream *stream) {
	static PaStreamInfo info = {
		.structVersion = 1,
		.inputLatency = 0,
		.outputLatency = 0,
		.sampleRate = 44100
	};
	PaStreamInfo *pa_info = stream != NULL ? &info : NULL;

	return pa_info;
}

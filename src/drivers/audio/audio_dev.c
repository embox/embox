/**
 * @file
 *
 * @date 13.06.2016
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <string.h>

#include <util/array.h>

#include <drivers/audio/audio_dev.h>

ARRAY_SPREAD_DEF(const struct audio_dev, __audio_device_registry);


struct audio_dev *audio_dev_get_by_idx(int idx) {
	return (struct audio_dev *)&__audio_device_registry[idx];
}

struct audio_dev *audio_dev_get_by_name(char name[]) {
	const struct audio_dev *audio_dev;

	array_spread_foreach_ptr(audio_dev, __audio_device_registry) {
		if (strcmp(audio_dev->ad_name, name)) {
			return (struct audio_dev *)audio_dev;
		}
	}
	return NULL;
}

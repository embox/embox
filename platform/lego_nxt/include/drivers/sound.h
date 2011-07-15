/**
 * @file
 *
 * @date 09.10.10
 * @author Anton Bondarev
 */

#ifndef NXT_SOUND_H_
#define NXT_SOUND_H_

#include <types.h>

#define   DURATION_MIN          10        // [mS]
#define   FREQUENCY_MIN         220       // [Hz]
#define   FREQUENCY_MAX         14080     // [Hz]
#define   SAMPLETONENO          16
#define   SAMPLEWORDBITS        (sizeof(uint32_t) * 8)
#define   SAMPLEWORDS           8

typedef uint32_t sampleword_t;

/**
 * Sample to play is array of sampleword_t count SAMPLEWORDS
 */
typedef sampleword_t *sample_t;

/**
 * Function returning next sample  to play
 */
typedef sample_t (*sound_handler_t)(void);

/** Starts playing sound with initial buffer @link buff @link,
 * next buffer @link next_buff @link and @link sound_hnd @link
 * as @link sound_handler_t @link
 *
 * @param freq Samplerate frequency
 * @param ms ms to playing
 * @note ms = 0: continuous playing, stops only at explicit stop
 * 		 ms != 0: playing specified time
 * @param buff First sample
 * @param next_buff Second sample
 * @param sound_hnd Sound handler which gives next samples
 */
extern void sound_start_play(uint32_t freq, useconds_t ms,
	sample_t buff, sample_t next_buff, sound_handler_t sound_hnd);

/**
 * Stops playing
 */
extern void sound_stop_play(void);

#endif /* NXT_SOUND_H_ */

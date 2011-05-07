/**
 * @file
 *
 * @date 09.10.10
 * @author Anton Bondarev
 */

#include <types.h>
#include <embox/test.h>
#include <drivers/sound.h>
#include <kernel/irq.h>
#include <drivers/at91sam7s256.h>
#define   SOUNDVOLUMESTEPS      4

EMBOX_TEST(run_sound);

static const SAMPLEWORD patterns[SOUNDVOLUMESTEPS + 1][SAMPLETONENO] =
{
    {
	0xF0F0F0F0,0xF0F0F0F0, // Step 0 = silence
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0
    },
    {
	0xF0F0F0F0,0xF0F0F0F0, // Step 1 = 1/512
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F8,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
        0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0
    },
    {
	0xF0F0F0F0,0xF0F0F0F0, // Step 2 = 0,+3,+4,+3,0,-3,-4,-3
	0xF0F0F0F0,0xF0F8F8F8,
	0xF0F0F8F8,0xF8F8F0F0,
	0xF8F8F8F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0F0F0F0,
	0xF0F0F0F0,0xF0E0E0E0,
	0xF0F0E0E0,0xE0E0F0F0,
	0xE0E0E0F0,0xF0F0F0F0
    },
    {
	0xF0F0F0F0,0xF0F0F0F0, // Step 3 = 0,+10,+14,+10,0,-10,-14,-10
	0xF8F8F8F8,0xF8F8FCFC,
	0xF8F8FCFC,0xFCFCFCFC,
	0xFCFCF8F8,0xF8F8F8F8,
	0xF0F0F0F0,0xF0F0F0F0,
	0xE0E0E0E0,0xE0E0C0C0,
	0xE0E0C0C0,0xC0C0C0C0,
	0xC0C0E0E0,0xE0E0E0E0
    },
    {
	0xF0F0F0F0,0xF0F0F0F0, // Step 4 = 0,+22,+32,+22,0,-22,-32,-22
	0xFCFCFCFC,0xFCFCFDFD,
	0xFFFFFFFF,0xFFFFFFFF,
	0xFDFDFCFC,0xFCFCFCFC,
	0xF0F0F0F0,0xF0F0F0F0,
	0xC0C0C0C0,0xC0C08080,
	0x00000000,0x00000000,
	0x8080C0C0,0xC0C0C0C0
    }
};

enum FREQ_TONE  {
	TONE_C = 262,
	TONE_D = 294,
	TONE_E = 330,
	TONE_F = 349,
	TONE_G = 392,
	TONE_A = 440,
	TONE_H = 494
};

#define DURATION 500

// XXX defined but not used
#if 0
static uint32_t freq_tone[2][3] = {
	{TONE_C, TONE_E, TONE_G},
	{TONE_C, TONE_E, TONE_F}
};

static uint32_t bank_num = 0;
#endif

static int i = 0;

SAMPLEWORD *sound_handler(void) {
	if (++i == 5) {
		i = 0;
	}

	return (SAMPLEWORD *) patterns[i];
}

static int run_sound(void) {
	int count = 5;
	while (count--) {
		sound_start_play(TONE_C, DURATION, (SAMPLEWORD *) patterns[0],
				(SAMPLEWORD *) patterns[1], sound_handler);
		sound_start_play(TONE_E, DURATION, (SAMPLEWORD *) patterns[0],
			    (SAMPLEWORD *) patterns[1], sound_handler);
		sound_start_play(TONE_G, DURATION, (SAMPLEWORD *) patterns[0],
			    (SAMPLEWORD *) patterns[1], sound_handler);
	}

	return 0;
}

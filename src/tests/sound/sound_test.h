/**
 * testing sound
 *
 * @date 22.07.2010
 * @author Darya Dzendzik
*/

typedef   unsigned short int	UWORD;
typedef   unsigned char			UBYTE;
typedef   unsigned long			ULONG;

#define   SAMPLEWORD            ULONG
#define   SAMPLETONENO          16        // No of tone samples
#define   SOUNDIntEnable        {\
									*(unsigned*)AT91C_SSC_IER     = AT91C_SSC_ENDTX;\
                                }

#define   SOUNDIntDisable       {\
									*(unsigned*)AT91C_SSC_IDR     = AT91C_SSC_ENDTX;\
                                }

#define   DURATION_MIN          10        // [mS]
#define   FREQUENCY_MIN         220       // [Hz]
#define   FREQUENCY_MAX         14080     // [Hz]
#define   SOUNDVOLUMESTEPS      4
#define   TRUE                  1
#define   FALSE                 0
#define   OSC                   48054850L
#define   SAMPLEWORDBITS        (sizeof(SAMPLEWORD) * 8)
#define   SAMPLEWORDS           8


SAMPLEWORD		ToneBuffer[SAMPLETONENO];
SAMPLEWORD 		SampleBuffer[SAMPLEBUFFERS][SAMPLEWORDS];
UBYTE     SoundReady;                // Sound channel ready (idle)
UWORD     MelodyPointer;
UBYTE     SoundDivider;              // Volume
UWORD     SoundSamplesLeft;          // Number of samples left on actual sound buffer
UWORD     SoundSamplesLeftNext;      // Number of samples left on next sound buffer
ULONG     ToneCycles;                // No of tone cycles
ULONG     ToneCyclesReady;           // No of tone cycles for ready

const     SAMPLEWORD TonePattern[SOUNDVOLUMESTEPS + 1][SAMPLETONENO] =
{
  {
    0xF0F0F0F0,0xF0F0F0F0,                        // Step 0 = silence
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0
  },
  {
    0xF0F0F0F0,0xF0F0F0F0,                        // Step 1 = 1/512
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F8,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0
  },
  {
    0xF0F0F0F0,0xF0F0F0F0,                        // Step 2 = 0,+3,+4,+3,0,-3,-4,-3
    0xF0F0F0F0,0xF0F8F8F8,
    0xF0F0F8F8,0xF8F8F0F0,
    0xF8F8F8F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0F0F0F0,
    0xF0F0F0F0,0xF0E0E0E0,
    0xF0F0E0E0,0xE0E0F0F0,
    0xE0E0E0F0,0xF0F0F0F0
  },
  {
    0xF0F0F0F0,0xF0F0F0F0,                        // Step 3 = 0,+10,+14,+10,0,-10,-14,-10
    0xF8F8F8F8,0xF8F8FCFC,
    0xF8F8FCFC,0xFCFCFCFC,
    0xFCFCF8F8,0xF8F8F8F8,
    0xF0F0F0F0,0xF0F0F0F0,
    0xE0E0E0E0,0xE0E0C0C0,
    0xE0E0C0C0,0xC0C0C0C0,
    0xC0C0E0E0,0xE0E0E0E0
  },
  {
    0xF0F0F0F0,0xF0F0F0F0,                        // Step 4 = 0,+22,+32,+22,0,-22,-32,-22
    0xFCFCFCFC,0xFCFCFDFD,
    0xFFFFFFFF,0xFFFFFFFF,
    0xFDFDFCFC,0xFCFCFCFC,
    0xF0F0F0F0,0xF0F0F0F0,
    0xC0C0C0C0,0xC0C08080,
    0x00000000,0x00000000,
    0x8080C0C0,0xC0C0C0C0
  }
};

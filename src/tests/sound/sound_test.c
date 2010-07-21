/**
 * testing sound
 *
 * @date 16.07.2010
 * @author Darya Dzendzik
*/

#include "AT91SAM7S256_inc.h"

typedef   unsigned short int	UWORD;
typedef   unsigned char			UBYTE;
typedef   unsigned long			ULONG;
typedef   signed short int		SWORD;
typedef   struct
{
    SWORD      Valprev;                           // Previous output value
    SWORD      Index;                             // Index into stepsize table
}ADPCM_State;

#define   SAMPLEWORD            ULONG
#define   SAMPLETONENO          16        // No of tone samples
#define   SOUNDIntEnable        {\
									*(unsigned*)AT91C_SSC_IER     = AT91C_SSC_ENDTX;\
                                }

#define   SOUNDIntDisable       {\
									*(unsigned*)AT91C_SSC_IDR     = AT91C_SSC_ENDTX;\
                                }
#define   SOUNDDisable                  {\
									*(unsigned*)AT91C_PIOA_PER    = AT91C_PA17_TD;   /* Disable TD on PA17  */\
                                        }
#define   SOUNDEnable                   {\
									*(unsigned*)AT91C_PIOA_PDR    = AT91C_PA17_TD;              /* Enable TD on PA17  */\
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
#define   SAMPLERATE_MIN        2000      // Min sample rate [sps]
#define   SAMPLERATE_DEFAULT    8000      // Default sample rate [sps]
#define   SAMPLERATE_MAX        16000     // Max sample rate [sps]
#define   SAMPLEBITS            (SAMPLEWORDS * SAMPLEWORDBITS)
#define   INIT_PREV_VAL_ADPCM   0x7F
#define   INIT_INDEX_ADPCM      20
#define   SAMPLEBUFFERS         2
#define   SAMPLEMIN             0         // Must be zero (no pwm/interrupt)
#define   SAMPLEMAX             256       // Must be 256 (8 bit wave format)
#define   SAMPLECENTER          (((SAMPLEMAX - SAMPLEMIN) / 2) + SAMPLEMIN)
#define   SAMPLECONSTANT        3    // >> == (SAMPLEMAX / SAMPLEWORDBITS)

SAMPLEWORD		ToneBuffer[SAMPLETONENO];
SAMPLEWORD 		SampleBuffer[SAMPLEBUFFERS][SAMPLEWORDS];
UBYTE     SoundReady;                // Sound channel ready (idle)
UWORD     MelodyPointer;
UBYTE     SoundDivider;              // Volume
UWORD     SoundSamplesLeft;          // Number of samples left on actual sound buffer
UWORD     SoundSamplesLeftNext;      // Number of samples left on next sound buffer
ULONG     ToneCycles;                // No of tone cycles
ULONG     ToneCyclesReady;           // No of tone cycles for ready
UBYTE     CurrentFileFormat;         // Hold current playing file type
UBYTE     *pSoundPointer;            // Pointer to sample in actual sound buffer
UBYTE     *pSoundPointerNext;        // Pointer to sample in next sound buffer
UBYTE     Outdata[2];                // Output buffer used within the ADPCM algorithm
ULONG     ClockNext;                 // Serial clock for next buffer
UBYTE     SampleBufferNo;            // Sample buffer no in use
ADPCM_State State;                   // Struct holding ADPCM state

UBYTE     FractionPattern[SAMPLEWORDS] =
{
  0x00,   // 0 -> 00000000
  0x10,   // 1 -> 00010000
  0x22,   // 2 -> 00100010
  0x4A,   // 3 -> 01001010
  0x55,   // 4 -> 01010101
  0x6D,   // 5 -> 01101101
  0x77,   // 6 -> 01110111
  0x7F,   // 7 -> 01111111
};

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

EMBOX_TEST(run_sound);

ULONG     SoundSampleRate(UWORD Rate)
{
  ULONG   Result;

  if (Rate > SAMPLERATE_MAX)
  {
    Rate = SAMPLERATE_MAX;
  }
  if (Rate < SAMPLERATE_MIN)
  {
    Rate = SAMPLERATE_MIN;
  }
  Result = ((OSC / (2 * SAMPLEBITS)) / Rate) + 1;

  return (Result);
}

/*__ramfunc*/ void CalculateBitstream(SAMPLEWORD *pSampleBuffer,UBYTE Sample)
{
  ULONG   IntegerMask;
  ULONG   FractionMask;
  UBYTE   Integer;
  UBYTE   Fraction;
  UBYTE   Mask;
  UBYTE   Tmp;
  SWORD   STmp;

  if (SoundDivider)
  {
    STmp        = Sample;
    STmp       &= 0xFF;
    STmp       -= SAMPLECENTER;
    STmp      >>= (SOUNDVOLUMESTEPS - SoundDivider);
    STmp       += SAMPLECENTER;
    Sample      = (UBYTE)STmp;
    SOUNDEnable;
  }
  else
  {
    SOUNDDisable;
  }

  Tmp = 0;
  IntegerMask   = 0xFFFF0000;
  Integer       = Sample >> SAMPLECONSTANT;
  Fraction      = Sample - (Integer << SAMPLECONSTANT);
  IntegerMask   = 0xFFFFFFFF << (SAMPLEWORDBITS - Integer);
  FractionMask  = (IntegerMask >> 1) | IntegerMask;
  Mask          = FractionPattern[Fraction];
  while (Tmp < SAMPLEWORDS)
  {
    if ((Mask & (0x01 << Tmp)))
    {
      *pSampleBuffer = FractionMask;
    }
    else
    {
      *pSampleBuffer = IntegerMask;
    }
    pSampleBuffer++;
    Tmp++;
  }
}

UBYTE     SoundStart(UBYTE *Sound,UWORD Length,UWORD SampleRate, UBYTE NewFileFormat)
{
  UBYTE   Result = FALSE;

  if (SoundReady == TRUE)
  {
    if (Length > 1)
    {
      CurrentFileFormat = NewFileFormat;
      *(unsigned*)AT91C_SSC_CMR   = SoundSampleRate(SampleRate);
      pSoundPointer    = Sound;
      SoundSamplesLeft = Length;

      if (0 == CurrentFileFormat)
      {
        CalculateBitstream(SampleBuffer[0],*pSoundPointer);
        *(unsigned*)AT91C_SSC_TPR   = (unsigned int)SampleBuffer[0];
        *(unsigned*)AT91C_SSC_TCR   = SAMPLEWORDS;
        pSoundPointer++;
        SoundSamplesLeft--;
        CalculateBitstream(SampleBuffer[1],*pSoundPointer);
        *(unsigned*)AT91C_SSC_TNPR  = (unsigned int)SampleBuffer[1];
        *(unsigned*)AT91C_SSC_TNCR  = SAMPLEWORDS;
        pSoundPointer++;
        SoundSamplesLeft--;
      }
      else
      {
        State.Valprev = INIT_PREV_VAL_ADPCM;
        State.Index = INIT_INDEX_ADPCM;
        //SoundADPCMDecoder(*pSoundPointer, Outdata, &State.Valprev, &State.Index);
        CalculateBitstream(SampleBuffer[0],Outdata[0]);
        *(unsigned*)AT91C_SSC_TPR   = (unsigned int)SampleBuffer[0];
        *(unsigned*)AT91C_SSC_TCR   = SAMPLEWORDS;
        pSoundPointer++;
        SoundSamplesLeft--;
        CalculateBitstream(SampleBuffer[1],Outdata[1]);
        *(unsigned*)AT91C_SSC_TNPR  = (unsigned int)SampleBuffer[1];
        *(unsigned*)AT91C_SSC_TNCR  = SAMPLEWORDS;
      }
      SampleBufferNo   = 0;
      SoundReady       = FALSE;
      SOUNDIntEnable;
      *(unsigned*)AT91C_SSC_PTCR  = AT91C_PDC_TXTEN;
    }
    Result           = TRUE;
  }
  else
  {
    if (!ToneCycles)
    {
      if (!SoundSamplesLeftNext)
      {
        CurrentFileFormat     = NewFileFormat;
        ClockNext             = SoundSampleRate(SampleRate);
        pSoundPointerNext     = Sound;
        SoundSamplesLeftNext  = Length;
        Result                = TRUE;
      }
    }
  }

  return (Result);
}

UBYTE     SoundStop(void)
{
  ToneCycles            = 0;
  SOUNDIntDisable;
  SOUNDDisable;
  SoundReady            = TRUE;
  SoundSamplesLeft      = 0;
  SoundSamplesLeftNext  = 0;
  MelodyPointer         = 0;

  return (TRUE);
}

void SoundFreq(UWORD Freq,UWORD mS,UBYTE Step){
  UBYTE   Tmp;

  /*checking korektnosti input data*/
  if (mS < DURATION_MIN){
    mS = DURATION_MIN;
  }
  if (Freq)  {
    if (Freq < FREQUENCY_MIN){
      Freq = FREQUENCY_MIN;
    }
    if (Freq > FREQUENCY_MAX){
      Freq = FREQUENCY_MAX;
    }
    if (Step > SOUNDVOLUMESTEPS){
      Step = SOUNDVOLUMESTEPS;
    }
  }
  else{
    Step = 0;
    Freq = 1000;
  }

  SoundDivider          = Step;
  SoundSamplesLeft      = 0;
  SoundSamplesLeftNext  = 0;
  for (Tmp = 0;Tmp < SAMPLETONENO;Tmp++){
    ToneBuffer[Tmp] = TonePattern[Step][Tmp];
  }

  *(ULONG*)AT91C_SSC_CMR   = (((ULONG)OSC / (2L * 512L)) / ((ULONG)Freq)) + 1L;
  ToneCycles       = ((ULONG)Freq * (ULONG)mS) / 1000L - 1L;
  ToneCyclesReady  = ((ULONG)Freq * (ULONG)2L) / 1000L + 1L;

  *(unsigned*)AT91C_SSC_TNPR = (unsigned int)ToneBuffer;
// *((void *)AT91C_SSC_TNPR)  = (unsigned int)ToneBuffer;
  *(unsigned*)AT91C_SSC_TNCR = SAMPLETONENO;
  *(unsigned*)AT91C_SSC_PTCR = AT91C_PDC_TXTEN;
  SoundReady       = FALSE;
  SOUNDIntEnable;
}

int run_sound(){
     SOUNDIntDisable;
     SoundReady                  = TRUE;
     MelodyPointer               = 0;
     *(unsigned*)AT91C_PMC_PCER             = (1L << AT91C_ID_SSC);       /* Enable MCK clock   */
     *(unsigned*)AT91C_PIOA_PER             = AT91C_PA17_TD;              /* Disable TD on PA17  */
     *(unsigned*)AT91C_PIOA_ODR             = AT91C_PA17_TD;
     *(unsigned*)AT91C_PIOA_OWDR            = AT91C_PA17_TD;
     *(unsigned*)AT91C_PIOA_MDDR            = AT91C_PA17_TD;
     *(unsigned*)AT91C_PIOA_PPUDR           = AT91C_PA17_TD;
     *(unsigned*)AT91C_PIOA_IFDR            = AT91C_PA17_TD;
     *(unsigned*)AT91C_PIOA_CODR            = AT91C_PA17_TD;
     *(unsigned*)AT91C_PIOA_IDR             = AT91C_PA17_TD;
     *(unsigned*)AT91C_SSC_CR               = AT91C_SSC_SWRST;
     AT91C_AIC_SMR[AT91C_ID_SSC] = AT91C_AIC_PRIOR_LOWEST | AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED;   /* Set priority*/
     *(unsigned*)AT91C_SSC_TCMR             = AT91C_SSC_CKS_DIV + AT91C_SSC_CKO_CONTINOUS + AT91C_SSC_START_CONTINOUS;
     *(unsigned*)AT91C_SSC_TFMR             = (SAMPLEWORDBITS - 1) + ((SAMPLEWORDS & 0xF) << 8) + AT91C_SSC_MSBF;
     *(unsigned*)AT91C_SSC_CR               = AT91C_SSC_TXEN;             /* TX enable          */
     *(unsigned*)AT91C_AIC_ICCR             = (1L << AT91C_ID_SSC);       /* Clear interrupt    */
     *(unsigned*)AT91C_AIC_IECR             = (1L << AT91C_ID_SSC);       /* Enable int. controller */

	SoundFreq( 99, 99, 2);
	TRACE("FRQ starting\n");
//	SoundStart(5, 2, 321, 3);
//	TRACE("sound starting\n");
//	SoundStop();
//	TRACE("stops\n");
		return 0;
}

/**
 * testing sound
 *
 * @date 16.07.2010
 * @author Darya Dzendzik
*/

#include "AT91SAM7S256_inc.h"
#include "sound_and_graphic_test.h"

EMBOX_TEST(run_sound);

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


void SoundFreq(UWORD Freq,UWORD mS,UBYTE Step){
  /*check correct input data*/
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
  ToneBuffer[0] = TonePattern[Step][6]; /*change only step*/

  *(ULONG*)AT91C_SSC_CMR   = (((ULONG)OSC / (2L * 512L)) / ((ULONG)Freq)) + 1L;
  ToneCycles       = ((ULONG)Freq * (ULONG)mS) / 1000L - 1L;
  ToneCyclesReady  = ((ULONG)Freq * (ULONG)2L) / 1000L + 1L; //Way it's need?

  *(unsigned*)AT91C_SSC_TNPR = (unsigned int)ToneBuffer;
  *(unsigned*)AT91C_SSC_TNCR = SAMPLETONENO;
  *(unsigned*)AT91C_SSC_PTCR = AT91C_PDC_TXTEN;
  SoundReady       = FALSE;
  SOUNDIntEnable;
}

int sound_init(void){
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
	((unsigned*)AT91C_AIC_SMR)[AT91C_ID_SSC] = AT91C_AIC_PRIOR_LOWEST | AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED;   /* Set priority*/
	*(unsigned*)AT91C_SSC_TCMR             = AT91C_SSC_CKS_DIV + AT91C_SSC_CKO_CONTINOUS + AT91C_SSC_START_CONTINOUS;
	*(unsigned*)AT91C_SSC_TFMR             = (SAMPLEWORDBITS - 1) + ((SAMPLEWORDS & 0xF) << 8) + AT91C_SSC_MSBF;
	*(unsigned*)AT91C_SSC_CR               = AT91C_SSC_TXEN;             /* TX enable          */
	*(unsigned*)AT91C_AIC_ICCR             = (1L << AT91C_ID_SSC);       /* Clear interrupt    */
	*(unsigned*)AT91C_AIC_IECR             = (1L << AT91C_ID_SSC);       /* Enable int. controller */
	return 0;
}

static int run_sound(void){
	int tmp;
	int tmp2;
	int modtmp;

	sound_init();
	for(tmp = 0;tmp < 10;tmp++){
		modtmp = tmp % 5;
		SoundFreq( 99, 99,modtmp );
		for(tmp = 0;tmp < 10000;tmp++){
			//TODO: must wait. see is it normal?
			}
	}
	return 0;
}

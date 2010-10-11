/**
 * @file
 *
 * @date 09.10.2010
 * @author Anton Bondarev
 * @author Darya Dzendzik
 *
 */
#include <types.h>
#include <embox/unit.h>
#include <linux/init.h>
#include <hal/reg.h>
#include <drivers/at91sam7s256.h>
#include <drivers/sound.h>

EMBOX_UNIT_INIT(sound_init);

static int __init sound_init(void) {
	REG_STORE(AT91C_PMC_PCER, (1L << AT91C_ID_SSC)); /* Enable MCK clock   */
	REG_STORE(AT91C_PIOA_PER, AT91C_PA17_TD); /* Disable TD on PA17  */
	REG_STORE(AT91C_PIOA_ODR, AT91C_PA17_TD);
	REG_STORE(AT91C_PIOA_OWDR, AT91C_PA17_TD);
	REG_STORE(AT91C_PIOA_MDDR, AT91C_PA17_TD);
	REG_STORE(AT91C_PIOA_PPUDR, AT91C_PA17_TD);
	REG_STORE(AT91C_PIOA_IFDR, AT91C_PA17_TD);
	REG_STORE(AT91C_PIOA_CODR, AT91C_PA17_TD);
	REG_STORE(AT91C_PIOA_IDR, AT91C_PA17_TD);
	REG_STORE(AT91C_SSC_CR, AT91C_SSC_SWRST);
	REG_STORE((uint32_t)AT91C_AIC_SMR[AT91C_ID_SSC], AT91C_AIC_PRIOR_LOWEST	| AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED); /* Set priority*/
	REG_STORE(AT91C_SSC_TCMR, AT91C_SSC_CKS_DIV + AT91C_SSC_CKO_CONTINOUS
			+ AT91C_SSC_START_CONTINOUS);
	REG_STORE(AT91C_SSC_TFMR, (SAMPLEWORDBITS - 1) + ((SAMPLEWORDS & 0xF) << 8) + AT91C_SSC_MSBF);
	REG_STORE(AT91C_SSC_CR, AT91C_SSC_TXEN); /* TX enable          */
	REG_STORE(AT91C_AIC_ICCR, (1L << AT91C_ID_SSC)); /* Clear interrupt    */
	REG_STORE(AT91C_AIC_IECR, (1L << AT91C_ID_SSC)); /* Enable int. controller */
	return 0;
}

void sound_next_sample(uint32_t freq, uint32_t ms, uint32_t buff[SAMPLETONENO]) {
	/*check correct input data*/
	if (ms < DURATION_MIN) {
		ms = DURATION_MIN;
	}
	if (freq) {
		if (freq < FREQUENCY_MIN) {
			freq = FREQUENCY_MIN;
		}
		if (freq > FREQUENCY_MAX) {
			freq = FREQUENCY_MAX;
		}
	} else {
		freq = 1000;
	}

	REG_STORE(AT91C_SSC_CMR, ((CONFIG_SYS_CLOCK / (2L * 512L)) / freq) + 1L);
	REG_STORE(AT91C_SSC_TNPR, (uint32_t) buff);
	REG_STORE(AT91C_SSC_TNCR, SAMPLETONENO);
	REG_STORE(AT91C_SSC_PTCR, AT91C_PDC_TXTEN);
}

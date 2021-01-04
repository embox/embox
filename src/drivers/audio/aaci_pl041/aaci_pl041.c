/**
 * @file
 *
 * @date 27.09.2016
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdint.h>
#include <assert.h>

#include <sys/mman.h>

#include <util/log.h>

#include <hal/reg.h>
#include <kernel/irq.h>

#include <portaudio.h>

#include <drivers/audio/audio_dev.h>

#include <drivers/audio/ac97.h>

#include <embox/unit.h>

#include "aaci_pl041.h"

EMBOX_UNIT_INIT(aaci_pl041_init);

#define BASE_ADDR    OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM      OPTION_GET(NUMBER, irq_num)

#define AACI_MAXBUF_LEN_MAX_BUF_LEN 0x10000
#define FIFO_SAMPLE_SZ   4 /* 32 bits */
#define FRAME_PERIOD_US  50

struct aaci_runtime {
	void *base;
	void *fifo;
	uint32_t cr;
};

struct aaci_pl041_hw_dev {
	uint32_t base_addr;
	uint32_t maincr;
	struct aaci_runtime aaci_runtime;
	uint32_t fifo_depth;

	void *po_stream;
	void *mic_stream;
};

static struct aaci_pl041_hw_dev aaci_pl041_hw_dev;

struct aaci_pl041_dev_priv {
	struct aaci_pl041_hw_dev *hw_dev;
	int devid;
	uint8_t *out_buf;
	uint32_t out_buf_len;
	uint8_t *in_buf;
	uint32_t in_buf_len;

	uint32_t cur_buff_offset;
};

static void udelay(int delay) {
	volatile int i;

	for (i = delay * 10; i != 0; i-- );
}
static void aaci_chan_wait_ready(struct aaci_runtime *aacirun, uint32_t mask) {
	uint32_t val;
	int timeout = 5000;

	do {
		udelay(1);
		val = REG32_LOAD(aacirun->base + AACI_SR);
	} while (val & mask && timeout--);
}

static void aaci_pl041_dev_start(struct audio_dev *dev) {
	uint32_t ie;
	uint32_t len;
	uint32_t stat;
	void *ptr;
	struct aaci_pl041_dev_priv *priv;
	struct aaci_pl041_hw_dev *hw_dev;
	struct aaci_runtime *aacirun;

	priv = dev->ad_priv;
	hw_dev = priv->hw_dev;
	aacirun = &hw_dev->aaci_runtime;

	//log_debug("dev = 0x%X", dev);

	ptr = audio_dev_get_out_cur_ptr(dev);
	len = hw_dev->fifo_depth;

	/* writing 16 bytes at a time */
	for ( ; len > 0; len -= 16) {
		asm(
			"ldmia	%0!, {r0, r1, r2, r3}\n\t"
			"stmia	%1, {r0, r1, r2, r3}"
			: "+r" (ptr)
			: "r" (aacirun->fifo)
			: "r0", "r1", "r2", "r3", "cc");

	}

	//
	//aaci_chan_wait_ready(aacirun, AACI_SR_TXB);
	stat = REG32_LOAD(aacirun->base + AACI_SR);
	aacirun->cr |= AACI_CR_EN | 0x8; /* TODO 4 slot but don't clear why it is required */

	ie = REG32_LOAD(aacirun->base + AACI_IE);
	if (stat & AACI_SR_TXHE) {
		//Pa_StartStream(NULL);
	} else {
		ie |= AACI_IE_URIE | AACI_IE_TXIE;
	}
	//ie |= AACI_IE_URIE | AACI_IE_TXIE;
	REG32_STORE(aacirun->base + AACI_IE, ie);
	REG32_STORE(aacirun->base + AACI_TXCR, aacirun->cr);


}

static void aaci_pl041_dev_stop(struct audio_dev *dev) {
	uint32_t ie;
	struct aaci_pl041_dev_priv *priv;
	struct aaci_pl041_hw_dev *hw_dev;
	struct aaci_runtime *aacirun;

	priv = dev->ad_priv;
	hw_dev = priv->hw_dev;
	aacirun = &hw_dev->aaci_runtime;

	log_debug("dev = 0x%X", dev);
	ie = REG32_LOAD(aacirun->base + AACI_IE);
	ie &= ~(AACI_IE_URIE | AACI_IE_TXIE);
	REG32_STORE(aacirun->base + AACI_IE, ie);
	aacirun->cr &= ~AACI_CR_EN;
	aaci_chan_wait_ready(aacirun, AACI_SR_TXB);
	REG32_STORE(aacirun->base + AACI_TXCR, aacirun->cr);
}

static void aaci_pl041_dev_pause(struct audio_dev *dev) {
	log_debug("dev = 0x%X", dev);
	aaci_pl041_dev_stop(dev);
}

static void aaci_pl041_dev_resume(struct audio_dev *dev) {
	log_debug("dev = 0x%X", dev);
	aaci_pl041_dev_start(dev);
}

static int aaci_pl041_ioctl(struct audio_dev *dev, int cmd, void *args) {
	log_debug("dev = 0x%X", dev);
	switch(cmd) {
	case ADIOCTL_IN_SUPPORT:
		return 0;
	case ADIOCTL_OUT_SUPPORT:
		return AD_STEREO_SUPPORT |
		       AD_16BIT_SUPPORT;
	case ADIOCTL_BUFLEN:
		return aaci_pl041_hw_dev.fifo_depth * FIFO_SAMPLE_SZ;
	case ADIOCTL_GET_RATE:
		return 44100;
	}
	SET_ERRNO(EINVAL);
	return -1;
}

static const struct audio_dev_ops aaci_pl041_dev_ops = {
	.ad_ops_start  = aaci_pl041_dev_start,
	.ad_ops_pause  = aaci_pl041_dev_pause,
	.ad_ops_resume = aaci_pl041_dev_resume,
	.ad_ops_stop   = aaci_pl041_dev_stop,
	.ad_ops_ioctl  = aaci_pl041_ioctl
};

static uint8_t dac1_out_buf[AACI_MAXBUF_LEN_MAX_BUF_LEN] __attribute__ ((aligned(0x1000)));
static uint8_t adc1_in_buf[AACI_MAXBUF_LEN_MAX_BUF_LEN] __attribute__ ((aligned(0x1000)));

static struct aaci_pl041_dev_priv aaci_pl041_dac1 = {
	.hw_dev      = &aaci_pl041_hw_dev,
	.devid       = 0,
	.out_buf     = dac1_out_buf,
	.out_buf_len = sizeof(dac1_out_buf),
};

static struct aaci_pl041_dev_priv aaci_pl041_dac2 = {
	.hw_dev = &aaci_pl041_hw_dev,
	.devid  = 1
};

static struct aaci_pl041_dev_priv aaci_pl041_adc1 = {
	.hw_dev      = &aaci_pl041_hw_dev,
	.devid       = 2,
	.in_buf      = adc1_in_buf,
	.in_buf_len  = sizeof(adc1_in_buf)
};

AUDIO_DEV_DEF("aaci_pl041_dac1",
		(struct audio_dev_ops *)&aaci_pl041_dev_ops,
		&aaci_pl041_dac1,
		AUDIO_DEV_OUTPUT);
AUDIO_DEV_DEF("aaci_pl041_dac2",
		(struct audio_dev_ops *)&aaci_pl041_dev_ops,
		&aaci_pl041_dac2,
		AUDIO_DEV_OUTPUT);
AUDIO_DEV_DEF("aaci_pl041_adc1",
		(struct audio_dev_ops *)&aaci_pl041_dev_ops,
		&aaci_pl041_adc1,
		AUDIO_DEV_INPUT);

void audio_dev_open_out_stream(struct audio_dev *audio_dev, void *stream) {
	struct aaci_pl041_dev_priv *priv = audio_dev->ad_priv;
	priv->hw_dev->po_stream = stream;
}

void audio_dev_open_in_stream(struct audio_dev *audio_dev, void *stream) {
	struct aaci_pl041_dev_priv *priv = audio_dev->ad_priv;
	priv->hw_dev->mic_stream = stream;
}

uint8_t *audio_dev_get_out_cur_ptr(struct audio_dev *audio_dev) {
	struct aaci_pl041_dev_priv *priv;

	priv = audio_dev->ad_priv;

	return priv->out_buf;
}

uint8_t *audio_dev_get_in_cur_ptr(struct audio_dev *audio_dev) {
	return NULL;
}

static void aaci_ac97_select_codec(struct aaci_pl041_hw_dev *hw_dev) {
	uint32_t v;
	uint32_t maincr = hw_dev->maincr;

	/*
	 * Ensure that the slot 1/2 RX registers are empty.
	 */
	v = REG32_LOAD(hw_dev->base_addr + AACI_SLFR);
	if (v & AACI_SLFR_2RXV)
		REG32_LOAD(hw_dev->base_addr + AACI_SL2RX);
	if (v & AACI_SLFR_1RXV)
		REG32_LOAD(hw_dev->base_addr + AACI_SL1RX);

	if (maincr != REG32_LOAD(hw_dev->base_addr + AACI_MAINCR)) {
		REG32_STORE(hw_dev->base_addr + AACI_MAINCR, maincr);
		REG32_LOAD(hw_dev->base_addr + AACI_MAINCR);
		udelay(1);
	}
}

void ac97_reg_write(unsigned short reg, unsigned short val) {
	int timeout;
	uint32_t v;

	struct aaci_pl041_hw_dev *hw_dev = &aaci_pl041_hw_dev;
	aaci_ac97_select_codec(hw_dev);

	/*
	 * P54: You must ensure that AACI_SL2TX is always written
	 * to, if required, before data is written to AACI_SL1TX.
	 */
	REG32_STORE(hw_dev->base_addr + AACI_SL2TX, val << 4);
	REG32_STORE(hw_dev->base_addr + AACI_SL1TX, reg << 12);

	/* Initially, wait one frame period */
	udelay(FRAME_PERIOD_US);

	/* And then wait an additional eight frame periods for it to be sent */
	timeout = FRAME_PERIOD_US * 8;
	do {
		udelay(1);
		v = REG32_LOAD(hw_dev->base_addr + AACI_SLFR);
	} while ((v & (AACI_SLFR_1TXB | AACI_SLFR_2TXB)) && --timeout);

	if (v & (AACI_SLFR_1TXB | AACI_SLFR_2TXB))
		log_error("timeout waiting for write to complete\n");
}

uint16_t ac97_reg_read(unsigned short reg) {
	int timeout, retries = 10;
	uint32_t v;
	struct aaci_pl041_hw_dev *hw_dev = &aaci_pl041_hw_dev;

	aaci_ac97_select_codec(hw_dev);

	/*
	 * Write the register address to slot 1.
	 */
	REG32_STORE(hw_dev->base_addr + AACI_SL1TX, (reg << 12) | (1 << 19));

	/* Initially, wait one frame period */
	udelay(FRAME_PERIOD_US);

	/* And then wait an additional eight frame periods for it to be sent */
	timeout = FRAME_PERIOD_US * 8;
	do {
		udelay(1);
		v = REG32_LOAD(hw_dev->base_addr + AACI_SLFR);
	} while ((v & AACI_SLFR_1TXB) && --timeout);

	if (v & AACI_SLFR_1TXB) {
		log_error("timeout on slot 1 TX busy");
		v = ~0;
		goto out;
	}

	/* Now wait for the response frame */
	udelay(FRAME_PERIOD_US);

	/* And then wait an additional eight frame periods for data */
	timeout = FRAME_PERIOD_US * 8;
	do {
		udelay(1);
		//cond_resched();
		v = REG32_LOAD(hw_dev->base_addr + AACI_SLFR) & (AACI_SLFR_1RXV | AACI_SLFR_2RXV);
	} while ((v != (AACI_SLFR_1RXV|AACI_SLFR_2RXV)) && --timeout);

	if (v != (AACI_SLFR_1RXV|AACI_SLFR_2RXV)) {
		log_error("timeout on RX valid");
		v = ~0;
		goto out;
	}

	do {
		v = REG32_LOAD(hw_dev->base_addr + AACI_SL1RX) >> 12;
		if (v == reg) {
			v = REG32_LOAD(hw_dev->base_addr + AACI_SL2RX) >> 4;
			break;
		} else if (--retries) {
			log_warning("ac97 read back fail.  retry");
			continue;
		} else {
			log_warning("wrong ac97 register read back (%x != %x)", v, reg);
			v = ~0;
		}
	} while (retries);
out:
	return v;
}

static int aaci_pl041_probe_ac97(uint32_t base) {
	/*
	 * Assert AACIRESET for 2us
	 */
	REG32_STORE(base + AACI_RESET, 0);
	udelay(2);
	REG32_STORE(base + AACI_RESET, RESET_NRST);

	/*
	 * Give the AC'97 codec more than enough time
	 * to wake up. (42us = ~2 frames at 48kHz.)
	 */
	udelay(FRAME_PERIOD_US * 2);
	return 0;
}

static int aaci_size_fifo(struct aaci_pl041_hw_dev *hw_dev) {
	struct aaci_runtime *aacirun;
	int i;

	aacirun = &hw_dev->aaci_runtime;
	/*
	 * Enable the channel, but don't assign it to any slots, so
	 * it won't empty onto the AC'97 link.
	 */
	aacirun->cr = AACI_CR_FEN | AACI_CR_SZ16 | AACI_CR_EN;
	REG32_STORE(aacirun->base + AACI_TXCR, aacirun->cr);

	for (i = 0; !(REG32_LOAD(aacirun->base + AACI_SR) & AACI_SR_TXFF) && i < 4096; i++) {
		REG32_STORE(aacirun->fifo, 0);
	}

	REG32_STORE(aacirun->base + AACI_TXCR, 0);

	/*
	 * Re-initialise the AACI after the FIFO depth test, to
	 * ensure that the FIFOs are empty.  Unfortunately, merely
	 * disabling the channel doesn't clear the FIFO.
	 */
	REG32_STORE(hw_dev->base_addr + AACI_MAINCR, hw_dev->maincr & ~AACI_MAINCR_IE);
	REG32_LOAD(hw_dev->base_addr + AACI_MAINCR);
	udelay(1);
	REG32_STORE(hw_dev->base_addr + AACI_MAINCR, hw_dev->maincr);

	/*
	 * If we hit 4096 entries, we failed.  Go back to the specified
	 * fifo depth.
	 */
	if (i == 4096) {
		i = 8;
	}

	return i;
}
/*
 * Interrupt support.
 */
static void aaci_fifo_irq(uint32_t base, int channel, uint32_t mask) {
	//log_debug("base = 0x%X channel = %d mask = 0x%X", base, channel, mask);

	if (mask & AACI_ISR_ORINTR) {
		REG32_STORE(base + AACI_INTCLR, AACI_ICLR_RXOEC1 << channel);
	}

	if (mask & AACI_ISR_RXTOINTR) {
		REG32_STORE(base + AACI_INTCLR, AACI_ICLR_RXTOFEC1 << channel);
	}

	if (mask & AACI_ISR_RXINTR) {
	}

	if (mask & AACI_ISR_URINTR) {
		REG32_STORE(base + AACI_INTCLR, AACI_ICLR_TXUEC1 << channel);
	}

	if (mask & AACI_ISR_TXCINTR) {
	/*
	AACITXINTR 1-4
	If the FIFO is enabled, the FIFO transmit interrupt is asserted
	when the PrimeCell AACI transmit FIFO is less than, or equal to,
	half-empty and the mask bit TxIE is set. The FIFO transmit
	interrupt is cleared by filling the transmit FIFO to more than half
	full.
	*/
		struct aaci_runtime *aacirun;
		uint32_t val;

		aacirun = &aaci_pl041_hw_dev.aaci_runtime;
		/* we must disable irq because fulling fifo will be in light thread */
		val = REG32_LOAD(aacirun->base + AACI_IE);
		val &= ~( AACI_IE_URIE | AACI_IE_TXIE);
		REG32_STORE(aacirun->base + AACI_IE, val);

		Pa_StartStream(aaci_pl041_hw_dev.po_stream);
	}
}

static irq_return_t aaci_pl041_irq_handler(unsigned int irq_num, void *dev_id) {
	uint32_t mask;
	int i;
	uint32_t base;

	base = aaci_pl041_hw_dev.base_addr;

	mask = REG32_LOAD(base + AACI_ALLINTS);
	log_debug("mask = 0x%X", mask);
	if (mask) {
		uint32_t m = mask;
		for (i = 0; i < 4; i++, m >>= 7) {
			if (m & 0x7f) {
				aaci_fifo_irq(base, i, m);
			}
		}
	}

	return mask ? IRQ_HANDLED : IRQ_NONE;
}

/**************************************************
 * End of AC'97
 **************************************************/

static int aaci_pl041_init(void) {
	int i;
	int ret;

	aaci_pl041_hw_dev.base_addr = (uintptr_t)mmap_device_memory(
			(void*)BASE_ADDR,
			0x1000,
			PROT_WRITE | PROT_READ,
			MAP_FIXED,
			BASE_ADDR);
	/* Set MAINCR to allow slot 1 and 2 data IO */
	aaci_pl041_hw_dev.maincr = AACI_MAINCR_IE | AACI_MAINCR_SL1RXEN | AACI_MAINCR_SL1TXEN |
			AACI_MAINCR_SL2RXEN | AACI_MAINCR_SL2TXEN;

	aaci_pl041_hw_dev.aaci_runtime.base = (void *)aaci_pl041_hw_dev.base_addr + AACI_CTRL_CH1;
	aaci_pl041_hw_dev.aaci_runtime.fifo = (void *)aaci_pl041_hw_dev.base_addr + AACI_DR1;

	for (i = 0; i < 4; i++) {
		void *base = (void *)(aaci_pl041_hw_dev.base_addr + i * 0x14);

		REG32_STORE(base + AACI_IE, 0);
		REG32_STORE(base + AACI_TXCR, 0);
		REG32_STORE(base + AACI_RXCR, 0);
	}

	REG32_STORE(BASE_ADDR + AACI_INTCLR, 0x1fff);
	REG32_STORE(BASE_ADDR + AACI_MAINCR, aaci_pl041_hw_dev.maincr);
	/*
	 * Fix: ac97 read back fail errors by reading
	 * from any arbitrary aaci register.
	 */
	REG32_LOAD(BASE_ADDR + AACI_CTRL_CH1);
	ret = aaci_pl041_probe_ac97(BASE_ADDR);
	if (ret)
		goto out;

	/*
	 * Size the FIFOs (must be multiple of 16).
	 * This is the number of entries in the FIFO.
	 */
	aaci_pl041_hw_dev.fifo_depth = aaci_size_fifo(&aaci_pl041_hw_dev);

	if ((ret = ac97_init())) {
		return ret;
	}

	ret = irq_attach(IRQ_NUM, aaci_pl041_irq_handler, IF_SHARESUP, &aaci_pl041_hw_dev, "aaci_pl041");
	if (ret) {
		goto out;
	}

out:
	return ret;
}

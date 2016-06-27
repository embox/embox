/**
 * @file
 * @brief Driver for Intel Corporation 82801AA AC'97 Audio Controller (rev 01),
 * which is based on PortAudio library
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-12-03
 */

#include <unistd.h>
#include <errno.h>

#include <util/log.h>

#include <asm/io.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/audio/portaudio.h>
#include <drivers/audio/audio_dev.h>
#include <drivers/audio/ac97.h>
#include <kernel/irq.h>



struct intel_ac_hw_dev {
	uint32_t base_addr_nam;
	uint32_t base_addr_namb;
};

struct intel_ac_dev_priv {
	struct intel_ac_hw_dev *hw_dev;
	int devid;
	uint8_t *out_buf;
	uint32_t out_buf_len;
	uint8_t *in_buf;

	uint32_t cur_buff_offset;
};

static struct intel_ac_hw_dev intel_ac_hw_dev;

/* You may want to look at "Intel 82801AA (ICH) & Intel 82801AB(ICH0)
 * IO Controller Hub AC ’97 Programmer’s Reference Manual" for more
 * technical details */

/* Offsets of registers */
#define INTEL_AC_COM	0x04 /* Command */
#define INTEL_AC_NAMBA	0x10 /* Native Audio Mixer Base Address */
#define INTEL_AC_NABMBA	0x14 /* Native Audio Bus Mastering Base Address */
#define INTEL_AC_INTLN	0x3c /* Interrupt Line */

#define INTEL_AC_PCM_IN_BUF  0x00
#define INTEL_AC_PO_BUF 0x10
#define INTEL_AC_MIC_BUF     0x20

/* Last Valid Index */
#define INTEL_AC_PCM_IN_LVI  0x05
#define INTEL_AC_PO_LVI 0x15
#define INTEL_AC_MIC_LVI     0x25

/* Control registers */
#define INTEL_AC_PCM_IN_CR  0x0B
#define INTEL_AC_PO_CR 0x1B
#define INTEL_AC_MIC_CR     0x2B

#define INTEL_AC_SAMPLE_SZ 2  /* Bytes */
#define INTEL_AC_BUFFER_SZ 32 /* Buffer descriptors */
struct intel_ac_buff_desc {
	uint32_t pointer;           /* 2-byte aligned */
	unsigned int ioc : 1;       /* Interrupt on completion */
	unsigned int bup : 1;       /* Buffer underrun policy */
	unsigned int reserved : 14; /* Must be zeroes */
	unsigned int length : 16;   /* Buffer length in samples */
} __attribute__((aligned(2)));

/* This buffers could be allocated in runtime */
static struct intel_ac_buff_desc pcm_out_buff_list[INTEL_AC_BUFFER_SZ];

/* Some of this stuff probably should be placed into
 * separate module */
#define INTEL_AC_MAX_BUF_LEN (INTEL_AC_SAMPLE_SZ * INTEL_AC_BUFFER_SZ)

static int intel_ac_buf_init(int n, struct audio_dev *dev) {
	struct intel_ac_dev_priv *priv;

	priv = dev->ad_priv;

	pcm_out_buff_list[n] = (struct intel_ac_buff_desc) {
		.pointer = (uint32_t)(priv->out_buf) + 2 * 0xFF00 * n,
		.length  = 0xFF00,//INTEL_AC_BUFFER_SZ,
		.bup = 1,
		.ioc = 1,
	};
	return 0;
}

/* Intel Corporation 82801AA AC'97 Audio Controller,
 * provided by QEMU with flag -soundhw ac97 */
#define INTEL_AC_VID 0x8086
#define INTEL_AC_PID 0x2415

static irq_return_t iac_interrupt(unsigned int irq_num, void *dev_id) {
	log_debug("irq_num = %d", irq_num);

	return IRQ_HANDLED;
}

static int intel_ac_init(struct pci_slot_dev *pci_dev) {
	int err;
	assert(pci_dev);

	pci_set_master(pci_dev);

	intel_ac_hw_dev.base_addr_nam = pci_dev->bar[0] & 0xFF00;
	intel_ac_hw_dev.base_addr_namb = pci_dev->bar[1] & 0xFF00;

	if ((err = ac97_init(pci_dev)))
		return err;

	if ((err = irq_attach(pci_dev->irq, iac_interrupt, IF_SHARESUP, &intel_ac_hw_dev, "iac")))
		return err;

	return 0;
}

PCI_DRIVER("Intel Corporation 82801AA AC'97 Audio Controller", intel_ac_init, INTEL_AC_VID, INTEL_AC_PID);


static void intel_ac_dev_start(struct audio_dev *dev) {
	uint8_t tmp;
	int i;

	out32((uint32_t)&pcm_out_buff_list, intel_ac_hw_dev.base_addr_nam + INTEL_AC_PO_BUF);

	/* Setup buffers, currently just zeroes */
	for (i = 0; i < INTEL_AC_BUFFER_SZ; i++) {
		intel_ac_buf_init(i, dev);
	}

	/* Setup Last Valid Index */
	out8(INTEL_AC_BUFFER_SZ - 1, intel_ac_hw_dev.base_addr_nam + INTEL_AC_PO_LVI);

	/* Set run bit */
	tmp = in8(intel_ac_hw_dev.base_addr_nam + INTEL_AC_PO_CR);
	tmp |= 0xf;
	out8(tmp, intel_ac_hw_dev.base_addr_nam + INTEL_AC_PO_CR);

}

static void intel_ac_dev_pause(struct audio_dev *dev) {
	out8(0x0, intel_ac_hw_dev.base_addr_nam + INTEL_AC_PO_CR);
}

static void intel_ac_dev_resume(struct audio_dev *dev) {

}

static void intel_ac_dev_stop(struct audio_dev *dev) {
	out8(0x0, intel_ac_hw_dev.base_addr_nam + INTEL_AC_PO_CR);
}

static int intel_ac_ioctl(struct audio_dev *dev, int cmd, void *args) {
	switch(cmd) {
	case ADIOCTL_SUPPORT:
		return AD_STEREO_SUPPORT |
		       AD_16BIT_SUPPORT;
	}
	SET_ERRNO(EINVAL);
	return -1;
}

static const struct audio_dev_ops intel_ac_dev_ops = {
	.ad_ops_start = intel_ac_dev_start,
	.ad_ops_pause = intel_ac_dev_pause,
	.ad_ops_resume = intel_ac_dev_resume,
	.ad_ops_stop = intel_ac_dev_stop,
	.ad_ops_ioctl = intel_ac_ioctl
};

static uint8_t dac1_out_buf[INTEL_AC_MAX_BUF_LEN] __attribute__ ((aligned(0x1000)));

static struct intel_ac_dev_priv intel_ac_dac1 = {
	.hw_dev = &intel_ac_hw_dev,
	.devid  = 0,
	.out_buf = dac1_out_buf,
	.out_buf_len = sizeof(dac1_out_buf)
};

static struct intel_ac_dev_priv intel_ac_dac2 = {
	.hw_dev = &intel_ac_hw_dev,
	.devid  = 1
};

static struct intel_ac_dev_priv intel_ac_adc1 = {
	.hw_dev = &intel_ac_hw_dev,
	.devid  = 2
};

AUDIO_DEV_DEF("intel_ac_dac1", (struct audio_dev_ops *)&intel_ac_dev_ops, &intel_ac_dac1);
AUDIO_DEV_DEF("intel_ac_dac2", (struct audio_dev_ops *)&intel_ac_dev_ops, &intel_ac_dac2);
AUDIO_DEV_DEF("intel_ac_adc1", (struct audio_dev_ops *)&intel_ac_dev_ops, &intel_ac_adc1);


uint8_t *audio_dev_get_out_cur_ptr(struct audio_dev *audio_dev) {
	struct intel_ac_dev_priv *priv;

	priv = audio_dev->ad_priv;

	priv->cur_buff_offset += audio_dev->samples_per_buffer *
			audio_dev->num_of_chan * 2;
	priv->cur_buff_offset %= priv->out_buf_len;

	return &priv->out_buf[priv->cur_buff_offset];
}

/**
 * @file spi_nor.c
 * @brief Generic SPI NOR flash driver for Embox
 *
 * Placement: src/drivers/flash/spi_nor/spi_nor.c
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <util/log.h>
#include <util/macro.h>
#include <embox/unit.h>
#include <framework/mod/options.h>

#include <drivers/spi.h>
#include <drivers/flash/flash.h>
#include <drivers/flash/flash_cache.h>

/* Opcodes */
#define NOR_OP_WREN        0x06u
#define NOR_OP_WRDI        0x04u
#define NOR_OP_RDSR        0x05u
#define NOR_OP_EWSR        0x50u
#define NOR_OP_WRSR        0x01u
#define NOR_OP_READ        0x03u
#define NOR_OP_FAST_READ   0x0Bu
#define NOR_OP_PP          0x02u
#define NOR_OP_AAI         0xADu
#define NOR_OP_SE          0x20u
#define NOR_OP_BE64        0xD8u
#define NOR_OP_CE          0xC7u
#define NOR_OP_RDID        0x9Fu
#define NOR_OP_RSTEN       0x66u
#define NOR_OP_RST         0x99u

#define SR_WIP             (1u << 0)

#define NOR_FLAG_AAI       (1u << 0)
#define NOR_FLAG_SST_WP    (1u << 1)
#define NOR_FLAG_FAST_READ (1u << 2)
#define NOR_FLAG_PAGE_PROG (1u << 3)

#define NOR_POLL_RETRIES 100u

/* Chip table */
struct spi_nor_info {
	uint8_t    mfr, type, dev;
	const char *name;
	uint32_t   total_size, sector_size, page_size, word_size, flags;
};

static const struct spi_nor_info spi_nor_table[] = {
	{ 0xBF, 0x25, 0x41, "SST25VF016B",
	  2*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_SST_WP|NOR_FLAG_FAST_READ },
	{ 0xBF, 0x25, 0x8D, "SST25VF040B",
	  512*1024, 4096, 256, 2,
	  NOR_FLAG_AAI|NOR_FLAG_SST_WP|NOR_FLAG_FAST_READ },
	{ 0xBF, 0x25, 0x8E, "SST25VF080B",
	  1024*1024, 4096, 256, 2,
	  NOR_FLAG_AAI|NOR_FLAG_SST_WP|NOR_FLAG_FAST_READ },
	{ 0xBF, 0x25, 0x4A, "SST25VF032B",
	  4*1024*1024, 4096, 256, 2,
	  NOR_FLAG_AAI|NOR_FLAG_SST_WP|NOR_FLAG_FAST_READ },
	{ 0xEF, 0x40, 0x14, "W25Q80BL",
	  1*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0xEF, 0x40, 0x15, "W25Q16DV",
	  2*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0xEF, 0x40, 0x16, "W25Q32BV",
	  4*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0xEF, 0x40, 0x17, "W25Q64FV",
	  8*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0xC2, 0x20, 0x15, "MX25L1606E",
	  2*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0xC2, 0x20, 0x16, "MX25L3206E",
	  4*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0x01, 0x02, 0x15, "S25FL116K",
	  2*1024*1024, 4096, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0x20, 0x20, 0x15, "M25P16",
	  2*1024*1024, 65536, 256, 1,
	  NOR_FLAG_PAGE_PROG|NOR_FLAG_FAST_READ },
	{ 0, 0, 0, NULL, 0, 0, 0, 0, 0 }
};

struct spi_nor_priv {
	struct spi_device         *spi;
	int                        cs;
	const struct spi_nor_info *info;
	uint8_t aligned_word[4];
};

#define MOD_SPI_BUS  OPTION_GET(NUMBER, spi_bus)
#define MOD_SPI_CS   OPTION_GET(NUMBER, spi_cs)

EMBOX_UNIT_INIT(spi_nor_unit_init);

static struct spi_nor_priv    g_nor_priv;
static const struct flash_dev_drv spi_nor_drv;

FLASH_CACHE_DEF(spi_nor, 2, 4096);

/* SPI helpers
 *
 * imx6_ecspi transfer: in=TX out=RX, full-duplex byte-by-byte.
 * CS is asserted for the entire duration of one spi_transfer() call.
 * For cmd+data reads we must send cmd+dummy in one call and capture
 * the response bytes from the same call (full-duplex).
 *
 * We use a 260-byte combined buffer: tx_buf = cmd bytes + zero padding,
 * rx_buf = captured response (first tx_len bytes discarded, rest is data).
 */
#define NOR_MAX_XFER (5 + 256)  /* max cmd(5) + page(256) */

static int nor_xfer(struct spi_nor_priv *priv,
                    const uint8_t *tx, size_t tx_len,
                    uint8_t *rx, size_t rx_len) {
	struct spi_device *spi = priv->spi;
	uint8_t txbuf[NOR_MAX_XFER];
	uint8_t rxbuf[NOR_MAX_XFER];
	size_t total = tx_len + rx_len;
	int ret;

	if (total > NOR_MAX_XFER) {
		log_error("spi_nor: xfer too large %zu", total);
		return -EINVAL;
	}

	/* Build combined TX buffer: cmd bytes then zeros for rx phase */
	memcpy(txbuf, tx, tx_len);
	if (rx_len > 0) {
		memset(txbuf + tx_len, 0x00, rx_len);
	}

	spi->spid_flags = SPI_CS_ACTIVE | SPI_CS_INACTIVE;
	ret = spi_transfer(spi, txbuf, rxbuf, (int)total);
	if (ret < 0) { return ret; }

	/* Extract received data — skip the tx_len echo bytes */
	if (rx && rx_len > 0) {
		memcpy(rx, rxbuf + tx_len, rx_len);
	}
	return 0;
}

static inline int nor_cmd(struct spi_nor_priv *p, uint8_t op) {
	return nor_xfer(p, &op, 1, NULL, 0);
}
static int nor_read_sr(struct spi_nor_priv *p, uint8_t *sr) {
	uint8_t cmd = NOR_OP_RDSR;
	return nor_xfer(p, &cmd, 1, sr, 1);
}
static int nor_wait_ready(struct spi_nor_priv *p) {
	uint8_t sr;
	unsigned n = NOR_POLL_RETRIES;
	while (n--) {
		int ret = nor_read_sr(p, &sr);
		if (ret < 0) { return ret; }
		if (!(sr & SR_WIP)) { return 0; }
	}
	log_error("spi_nor: timeout waiting for WIP to clear");
	return -ETIMEDOUT;
}
static inline int nor_wren(struct spi_nor_priv *p) { return nor_cmd(p, NOR_OP_WREN); }
static inline int nor_wrdi(struct spi_nor_priv *p) { return nor_cmd(p, NOR_OP_WRDI); }

static int nor_unprotect(struct spi_nor_priv *p) {
	uint8_t cmd[2] = { NOR_OP_WRSR, 0x00 };
	int ret;
	if (p->info->flags & NOR_FLAG_SST_WP) {
		ret = nor_cmd(p, NOR_OP_EWSR);
	} else {
		ret = nor_wren(p);
	}
	if (ret < 0) { return ret; }
	ret = nor_xfer(p, cmd, 2, NULL, 0);
	/* No wait_ready here - WRSR completes quickly and
	 * SR polling is unreliable before we verify SPI RX works */
	return ret;
}

/* flash_dev_drv ops */
static int spi_nor_flash_init(struct flash_dev *dev, void *arg) {
	/* flash_create() is called from spi_nor_unit_init() after SPI is up.
	 * This function is called by flash_devs_init() and must be a no-op
	 * since the device is already fully initialised. */
	return 0;
}

static int spi_nor_flash_read(struct flash_dev *dev, uint32_t base,
                              void *data, size_t len) {
	struct spi_nor_priv *priv = dev->privdata;
	uint8_t cmd[5];
	size_t cmd_len;

	if (base + len > dev->size) {
		log_error("spi_nor: read out of range");
		return -EINVAL;
	}
	if (priv->info->flags & NOR_FLAG_FAST_READ) {
		cmd[0] = NOR_OP_FAST_READ;
		cmd[1] = (uint8_t)(base >> 16);
		cmd[2] = (uint8_t)(base >>  8);
		cmd[3] = (uint8_t)(base >>  0);
		cmd[4] = 0x00;
		cmd_len = 5;
	} else {
		cmd[0] = NOR_OP_READ;
		cmd[1] = (uint8_t)(base >> 16);
		cmd[2] = (uint8_t)(base >>  8);
		cmd[3] = (uint8_t)(base >>  0);
		cmd_len = 4;
	}
	return nor_xfer(priv, cmd, cmd_len, data, len);
}

static int spi_nor_flash_erase_block(struct flash_dev *dev, uint32_t block) {
	struct spi_nor_priv *priv = dev->privdata;
	uint32_t byte_addr = block * dev->block_info[0].block_size;
	uint8_t  cmd[4];
	uint8_t  op;
	int ret;

	op = ((byte_addr & (65536u - 1u)) == 0) ? NOR_OP_BE64 : NOR_OP_SE;

	ret = nor_wren(priv);
	if (ret < 0) { return ret; }
	cmd[0] = op;
	cmd[1] = (uint8_t)(byte_addr >> 16);
	cmd[2] = (uint8_t)(byte_addr >>  8);
	cmd[3] = (uint8_t)(byte_addr >>  0);
	ret = nor_xfer(priv, cmd, 4, NULL, 0);
	if (ret < 0) { return ret; }
	/* Block erase takes up to 25ms (sector) or 2s (64K); poll WIP */
	return nor_wait_ready(priv);
}

static int nor_program_aai(struct spi_nor_priv *priv, uint32_t base,
                           const uint8_t *src, size_t len) {
	uint8_t cmd[6];
	int ret;

	ret = nor_wren(priv);
	if (ret < 0) { return ret; }

	cmd[0] = NOR_OP_AAI;
	cmd[1] = (uint8_t)(base >> 16);
	cmd[2] = (uint8_t)(base >>  8);
	cmd[3] = (uint8_t)(base >>  0);
	cmd[4] = src[0];
	cmd[5] = src[1];
	ret = nor_xfer(priv, cmd, 6, NULL, 0);
	if (ret < 0) { goto done; }
	ret = nor_wait_ready(priv);
	if (ret < 0) { goto done; }
	src += 2; len -= 2;

	while (len >= 2) {
		cmd[0] = NOR_OP_AAI;
		cmd[1] = src[0];
		cmd[2] = src[1];
		ret = nor_xfer(priv, cmd, 3, NULL, 0);
		if (ret < 0) { goto done; }
		ret = nor_wait_ready(priv);
		if (ret < 0) { goto done; }
		src += 2; len -= 2;
	}
done:
	nor_wrdi(priv);
	return ret;
}

static int nor_program_page(struct spi_nor_priv *priv, uint32_t base,
                            const uint8_t *src, size_t len) {
	uint32_t page_size = priv->info->page_size;
	int ret = 0;
	while (len > 0) {
		uint32_t chunk = page_size - (base % page_size);
		if (chunk > len) { chunk = len; }
		uint8_t cmd[4] = {
			NOR_OP_PP,
			(uint8_t)(base >> 16),
			(uint8_t)(base >>  8),
			(uint8_t)(base >>  0)
		};
		ret = nor_wren(priv);
		if (ret < 0) { return ret; }
		/* cmd(4) + data(chunk) must be in ONE spi_transfer call
		 * because imx6 deasserts CS after every call */
		{
			static uint8_t pagebuf[4 + 256];
			memcpy(pagebuf, cmd, 4);
			memcpy(pagebuf + 4, src, chunk);
			priv->spi->spid_flags = SPI_CS_ACTIVE | SPI_CS_INACTIVE;
			ret = spi_transfer(priv->spi, pagebuf, NULL, (int)(4 + chunk));
		}
		if (ret < 0) { return ret; }
		/* Page program takes up to 3ms; poll WIP before next WREN/PP */
		ret = nor_wait_ready(priv);
		if (ret < 0) { return ret; }

		base += chunk; src += chunk; len -= chunk;
	}
	return ret;
}

static int spi_nor_flash_program(struct flash_dev *dev, uint32_t base,
                                 const void *data, size_t len) {
	struct spi_nor_priv *priv = dev->privdata;
	const uint8_t *src = (const uint8_t *)data;

	if (base + len > dev->size) {
		log_error("spi_nor: program out of range");
		return -EINVAL;
	}
	if (len == 0) { return 0; }

	if (priv->info->flags & NOR_FLAG_AAI) {
		if ((base & 1u) || (len & 1u)) {
			log_error("spi_nor: AAI needs 2-byte alignment");
			return -EINVAL;
		}
		return nor_program_aai(priv, base, src, len);
	}
	return nor_program_page(priv, base, src, len);
}

static const struct flash_dev_drv spi_nor_drv = {
	.flash_init        = spi_nor_flash_init,
	.flash_read        = spi_nor_flash_read,
	.flash_erase_block = spi_nor_flash_erase_block,
	.flash_program     = spi_nor_flash_program,
};

FLASH_DEV_DEF("spi_nor", &spi_nor_drv);

/* EMBOX_UNIT_INIT: runs at Runlevel 1 after ECSPI is up.
 * Probes chip, unprotects, then calls flash_create() directly. */
static int spi_nor_unit_init(void) {
	struct spi_nor_priv *priv = &g_nor_priv;
	const struct spi_nor_info *info;
	struct flash_dev *flash;
	uint8_t cmd = NOR_OP_RDID;
	uint8_t id[3] = {0, 0, 0};
	int i, ret;

	priv->spi = spi_dev_by_id(MOD_SPI_BUS);
	if (!priv->spi) {
		log_error("spi_nor: SPI bus %d not found", MOD_SPI_BUS);
		return -ENODEV;
	}
	priv->cs = MOD_SPI_CS;

	/* Select the chip-select line on the controller */
	spi_select(priv->spi, priv->cs);

	/* Reset */
	nor_cmd(priv, NOR_OP_RSTEN);
	nor_cmd(priv, NOR_OP_RST);

	/* JEDEC ID */
	ret = nor_xfer(priv, &cmd, 1, id, 3);
	if (ret < 0) {
		log_error("spi_nor: JEDEC ID read failed (%d)", ret);
		return ret;
	}
	log_info("spi_nor: JEDEC ID %02x:%02x:%02x", id[0], id[1], id[2]);

	/* Debug: read SR directly to verify RX works */
	{
		uint8_t sr = 0xFF;
		nor_read_sr(priv, &sr);
		log_info("spi_nor: SR after reset = 0x%02x (expect 0x00 or low)", sr);
	}

	/* Chip lookup */
	info = NULL;
	for (i = 0; spi_nor_table[i].name != NULL; i++) {
		if (spi_nor_table[i].mfr  == id[0] &&
		    spi_nor_table[i].type == id[1] &&
		    spi_nor_table[i].dev  == id[2]) {
			info = &spi_nor_table[i];
			break;
		}
	}
	if (info) {
		log_info("spi_nor: found %s", info->name);
	} else {
		log_warning("spi_nor: unknown chip, using SST25VF016B defaults");
		info = &spi_nor_table[0];
	}
	priv->info = info;

	/* Unprotect */
	ret = nor_unprotect(priv);
	if (ret < 0) {
		log_error("spi_nor: unprotect failed (%d)", ret);
		return ret;
	}

	/* Create flash_dev directly here — SPI must be up first */
	flash = flash_create("spi_nor", info->total_size);
	if (!flash) {
		log_error("spi_nor: flash_create() failed");
		return -ENOMEM;
	}
	flash->drv             = &spi_nor_drv;
	flash->privdata        = priv;
	flash->size            = info->total_size;
	flash->num_block_infos = 1;
	flash->block_info[0]   = (struct flash_block_info){
		.fbi_start_id = 0,
		.block_size   = info->sector_size,
		.blocks       = info->total_size / info->sector_size,
	};
	flash->fld_word_size    = info->word_size;
	flash->fld_aligned_word = priv->aligned_word;
	flash->fld_cache        = FLASH_CACHE_GET(flash, spi_nor);

	log_info("spi_nor: %s ready: %uKiB, %u x %uKiB sectors, "
	         "fast_read=%d aai=%d",
	         info->name, info->total_size / 1024,
	         info->total_size / info->sector_size,
	         info->sector_size / 1024,
	         !!(info->flags & NOR_FLAG_FAST_READ),
	         !!(info->flags & NOR_FLAG_AAI));
	return 0;
}

/**
 * @file
 * @brief
 * @author Dmitrii Piliuk
 * @version
 * @date 28.10.2026
 */

#include <errno.h>

#include <drivers/spi.h>
#include <hal/reg.h>

#include "esp32_spi.h"

int esp32_spi_hw_init(struct esp32_spi *dev, unsigned int clock_div) {
	unsigned int pre, n = 0;
	uint32_t reg;

	if (clock_div < 2 || clock_div > 16 * 64) {
		return -EINVAL;
	}

	/* Find an exact factorization for the fixed board-configured divider. */
	for (pre = 1; pre <= 16; pre++) {
		if (clock_div % pre == 0) {
			n = clock_div / pre;
			if (n >= 2 && n <= 64) {
				break;
			}
		}
	}
	if (pre > 16) {
		return -EINVAL;
	}

	/* CLK_EQU_SYSCLK = 0; approximately 50% duty cycle. */
	reg = ((pre - 1) << ESP32_SPI_CLKDIV_PRE_SHIFT)
	    | ((n - 1) << ESP32_SPI_CLKCNT_N_SHIFT)
	    | ((n / 2 - 1) << ESP32_SPI_CLKCNT_H_SHIFT) | (n - 1)
	    | ESP32_SPI_CLK_EDGE_SEL;
	/* Standard RX sampling edge; PCR divides the source clock by two. */
	REG32_STORE(dev->base_addr + ESP32_SPI_CLOCK, reg);

	/* Master, CPU-controlled transfers; no interrupts or DMA. */
	REG32_STORE(dev->base_addr + ESP32_SPI_SLAVE, 0);
	REG32_STORE(dev->base_addr + ESP32_SPI_DMA_INT_ENA, 0);
	REG32_STORE(dev->base_addr + ESP32_SPI_DMA_CONF, 0);

	/* Single-line transfers and MSB first in both directions. */
	REG32_STORE(dev->base_addr + ESP32_SPI_CTRL,
	    ESP32_SPI_CTRL_Q_POL | ESP32_SPI_CTRL_D_POL
	        | ESP32_SPI_CTRL_HOLD_POL | ESP32_SPI_CTRL_WP_POL);

	/* Mode 0, full duplex, buffer starts at W0 in both directions.
	 * Command/address/dummy phases and SIO/dual/quad modes stay disabled.
	 * CS setup/hold are enabled, each lasting one SPI clock cycle.
	 */
	REG32_STORE(dev->base_addr + ESP32_SPI_USER,
	    ESP32_SPI_USER_DOUTDIN | ESP32_SPI_USER_MOSI | ESP32_SPI_USER_MISO
	        | ESP32_SPI_USER_CS_SETUP | ESP32_SPI_USER_CS_HOLD);
	REG32_STORE(dev->base_addr + ESP32_SPI_USER1, 0);
	REG32_STORE(dev->base_addr + ESP32_SPI_USER2, 0);

	/* Active-low CS0, released between transactions; SCLK idle low. */
	REG32_STORE(dev->base_addr + ESP32_SPI_MISC, ESP32_SPI_MISC_CS1_5_DIS);

	/* Apply UPDATE and transaction length before starting each transfer. */
	return 0;
}

static int esp32_spi_select(struct spi_controller *spi_c, int cs) {
	if (cs != 0) {
		return -EINVAL;
	}

	/* CS0 is the only connected hardware chip select, selected at init. */
	return 0;
}

static int esp32_spi_set_mode(struct spi_controller *spi_c, bool is_master) {
	if (!is_master) {
		return -ENOSUPP;
	}

	/* Hardware is configured as master during initialization. */
	spi_c->is_master = true;
	return 0;
}

static int esp32_spi_apply_config(struct esp32_spi *dev) {
	unsigned int timeout;

	REG32_ORIN(dev->base_addr + ESP32_SPI_CMD, ESP32_SPI_CMD_UPDATE);
	for (timeout = ESP32_SPI_POLL_LIMIT; timeout; timeout--) {
		if (!(REG32_LOAD(dev->base_addr + ESP32_SPI_CMD)
		    & ESP32_SPI_CMD_UPDATE)) {
			return 0;
		}
	}
	return -ETIMEDOUT;
}

static int esp32_spi_wait_done(struct esp32_spi *dev) {
	unsigned int timeout;
	uint32_t status;

	for (timeout = ESP32_SPI_POLL_LIMIT; timeout; timeout--) {
		status = REG32_LOAD(dev->base_addr + ESP32_SPI_DMA_INT_RAW);
		if (status & ESP32_SPI_INT_ERRORS) {
			return -EIO;
		}
		if (status & ESP32_SPI_INT_TRANS_DONE) {
			return 0;
		}
	}
	return -ETIMEDOUT;
}

static void esp32_spi_reset_fifos(struct esp32_spi *dev) {
	uint32_t mask = ESP32_SPI_DMA_RX_AFIFO_RST | ESP32_SPI_DMA_BUF_AFIFO_RST;

	REG32_ORIN(dev->base_addr + ESP32_SPI_DMA_CONF, mask);
	REG32_CLEAR(dev->base_addr + ESP32_SPI_DMA_CONF, mask);
}

static void esp32_spi_abort(struct esp32_spi *dev) {
	REG32_CLEAR(dev->base_addr + ESP32_SPI_MISC,
	    ESP32_SPI_MISC_CS_KEEP_ACTIVE);
	/* Reset the transfer state and external lines, including held CS. */
	REG32_ORIN(dev->base_addr + ESP32_SPI_SLAVE, ESP32_SPI_SLAVE_SOFT_RESET);
	REG32_CLEAR(dev->base_addr + ESP32_SPI_SLAVE, ESP32_SPI_SLAVE_SOFT_RESET);
	esp32_spi_reset_fifos(dev);
	REG32_STORE(dev->base_addr + ESP32_SPI_DMA_INT_CLR,
	    ESP32_SPI_INT_TRANS_DONE | ESP32_SPI_INT_ERRORS);
}

static int esp32_spi_transfer(struct spi_controller *spi_c, uint8_t *inbuf,
    uint8_t *outbuf, int count) {
	struct esp32_spi *dev = spi_c->spic_priv;
	uint32_t word, misc;
	int chunk, i, j, ret;

	if (count < 0) {
		return -ENOSUPP; /* Interrupt-driven transfers are not supported. */
	}
	if (!count) {
		return 0;
	}
	if (!inbuf && !outbuf) {
		return -EINVAL;
	}
	if (spi_c->flags & ~(SPI_CS_ACTIVE | SPI_CS_INACTIVE)) {
		return -ENOSUPP; /* Fixed mode 0, clock, polling and no DMA. */
	}
	if (REG32_LOAD(dev->base_addr + ESP32_SPI_CMD)
	    & (ESP32_SPI_CMD_USR | ESP32_SPI_CMD_UPDATE)) {
		return -EBUSY;
	}

	while (count) {
		chunk = count > (int)ESP32_SPI_BUFFER_SIZE
		    ? (int)ESP32_SPI_BUFFER_SIZE : count;
		esp32_spi_reset_fifos(dev);

		/* Pack bytes explicitly: no alignment or padded-buffer requirement. */
		for (i = 0; i < chunk; i += 4) {
			word = 0;
			for (j = 0; j < 4 && i + j < chunk; j++) {
				word |= (uint32_t)(inbuf ? inbuf[i + j] : 0) << (8 * j);
			}
			REG32_STORE(dev->base_addr + ESP32_SPI_W(i / 4), word);
		}

		misc = REG32_LOAD(dev->base_addr + ESP32_SPI_MISC);
		if (count > chunk) {
			misc |= ESP32_SPI_MISC_CS_KEEP_ACTIVE;
		}
		else {
			misc &= ~ESP32_SPI_MISC_CS_KEEP_ACTIVE;
		}
		REG32_STORE(dev->base_addr + ESP32_SPI_MISC, misc);
		REG32_STORE(dev->base_addr + ESP32_SPI_MS_DLEN, chunk * 8 - 1);
		REG32_STORE(dev->base_addr + ESP32_SPI_DMA_INT_CLR,
		    ESP32_SPI_INT_TRANS_DONE | ESP32_SPI_INT_ERRORS);

		ret = esp32_spi_apply_config(dev);
		if (ret) {
			goto error;
		}
		REG32_ORIN(dev->base_addr + ESP32_SPI_CMD, ESP32_SPI_CMD_USR);
		ret = esp32_spi_wait_done(dev);
		if (ret) {
			goto error;
		}

		if (outbuf) {
			for (i = 0; i < chunk; i += 4) {
				word = REG32_LOAD(dev->base_addr + ESP32_SPI_W(i / 4));
				for (j = 0; j < 4 && i + j < chunk; j++) {
					outbuf[i + j] = word >> (8 * j);
				}
			}
			outbuf += chunk;
		}
		if (inbuf) {
			inbuf += chunk;
		}
		count -= chunk;
	}
	return 0;

error:
	esp32_spi_abort(dev);
	return ret;
}

struct spi_controller_ops esp32_spic_ops = {
	.select = esp32_spi_select,
	.set_mode = esp32_spi_set_mode,
	.transfer = esp32_spi_transfer,
};

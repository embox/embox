/**
 * @file dw_spi.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.12.2019
 */

#ifndef DW_SPI_H
#define DW_SPI_H

#define DW_SPI_CTRLR0            0x00
# define DW_SPI_CTRLR0_SRL       (1 << 11) /* Self-test mode */
# define DW_SPI_CTRLR0_TR        (0 << 8)
# define DW_SPI_CTRLR0_T         (1 << 8)
# define DW_SPI_CTRLR0_R         (2 << 8)
# define DW_SPI_CTRLR0_EEPROM    (3 << 8)
# define DW_SPI_CTRLR0_SCPOL     (1 << 7)
# define DW_SPI_CTRLR0_SCPH      (1 << 6)
# define DW_SPI_CTRLR0_FRF_MOTO  (0 << 4)
# define DW_SPI_CTRLR0_FRF_TI    (1 << 4)
# define DW_SPI_CTRLR0_FRF_MIC   (2 << 4)
#define DW_SPI_CTRLR1            0x04
#define DW_SPI_ENR               0x08
#define DW_SPI_MWCR              0x0C
# define DW_SPI_MWCR_EN          (1 << 0)
#define DW_SPI_SER               0x10
#define DW_SPI_BAUDR             0x14
#define DW_SPI_TXFTLR            0x18
#define DW_SPI_RXFTLR            0x1C
#define DW_SPI_TXFLR             0x20
#define DW_SPI_RXFLR             0x24
#define DW_SPI_SR                0x28
# define DW_SPI_SR_BUSY          (1 << 0)
# define DW_SPI_SR_RFNE          (1 << 3)
#define DW_SPI_IMR               0x2C
#define DW_SPI_ISR               0x30
#define DW_SPI_RISR              0x34
#define DW_SPI_TXOICR            0x38
#define DW_SPI_RXOICR            0x3C
#define DW_SPI_RXUICR            0x40
#define DW_SPI_ICR               0x48
#define DW_SPI_DMACR             0x4C
# define DW_SPI_DMACR_RXEN       (1 << 0)
# define DW_SPI_DMACR_TXEN       (1 << 1)
#define DW_SPI_DMATDLR           0x50
#define DW_SPI_DMARDLR           0x54
#define DW_SPI_IDR               0x58
# define DW_SPI_IDR_VAL          0x5510000
#define DW_SPI_SPI_VERSION_ID    0x5C
# define DW_SPI_VERSION          0x3332302A
#define DW_SPI_DR                0x60
#define DW_SPI_DLY               0xF0

struct dw_spi_priv {
	uintptr_t base_addr;
};

#endif /* DW_SPI_H */

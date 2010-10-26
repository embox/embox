/**
 * @file
 * @brief Testing lego mildstorm display output
 *
 * @date 23.07.2010
 * @author Darya Dzendzik
*/
#include <types.h>
#include <drivers/at91sam7s256.h>
#include <embox/test.h>
#include "graphics_test.h"

EMBOX_TEST(run_graphic);

static HEADER **pHeaders;

static int SPIInit(void) {
	*AT91C_PMC_PCER    = (1L << AT91C_ID_SPI); /* Enable MCK clock     */
	*AT91C_PIOA_PER    = AT91C_PIO_PA12;       /* Enable A0 on PA12    */
	*AT91C_PIOA_OER    = AT91C_PIO_PA12;
	*AT91C_PIOA_CODR   = AT91C_PIO_PA12;
	*AT91C_PIOA_PDR    = AT91C_PA14_SPCK;      /* Enable SPCK on PA14  */
	*AT91C_PIOA_ASR    = AT91C_PA14_SPCK;
	*AT91C_PIOA_ODR    = AT91C_PA14_SPCK;
	*AT91C_PIOA_OWER   = AT91C_PA14_SPCK;
	*AT91C_PIOA_MDDR   = AT91C_PA14_SPCK;
	*AT91C_PIOA_PPUDR  = AT91C_PA14_SPCK;
	*AT91C_PIOA_IFDR   = AT91C_PA14_SPCK;
	*AT91C_PIOA_CODR   = AT91C_PA14_SPCK;
	*AT91C_PIOA_IDR    = AT91C_PA14_SPCK;
	*AT91C_PIOA_PDR    = AT91C_PA13_MOSI;      /* Enable mosi on PA13*/
	*AT91C_PIOA_ASR    = AT91C_PA13_MOSI;
	*AT91C_PIOA_ODR    = AT91C_PA13_MOSI;
	*AT91C_PIOA_OWER   = AT91C_PA13_MOSI;
	*AT91C_PIOA_MDDR   = AT91C_PA13_MOSI;
	*AT91C_PIOA_PPUDR  = AT91C_PA13_MOSI;
	*AT91C_PIOA_IFDR   = AT91C_PA13_MOSI;
	*AT91C_PIOA_CODR   = AT91C_PA13_MOSI;
	*AT91C_PIOA_IDR    = AT91C_PA13_MOSI;
	*AT91C_PIOA_PDR    = AT91C_PA10_NPCS2;      /* Enable npcs0 on PA11*/
	*AT91C_PIOA_BSR    = AT91C_PA10_NPCS2;
	*AT91C_PIOA_ODR    = AT91C_PA10_NPCS2;
	*AT91C_PIOA_OWER   = AT91C_PA10_NPCS2;
	*AT91C_PIOA_MDDR   = AT91C_PA10_NPCS2;
	*AT91C_PIOA_PPUDR  = AT91C_PA10_NPCS2;
	*AT91C_PIOA_IFDR   = AT91C_PA10_NPCS2;
	*AT91C_PIOA_CODR   = AT91C_PA10_NPCS2;
	*AT91C_PIOA_IDR    = AT91C_PA10_NPCS2;
	*AT91C_SPI_CR      = AT91C_SPI_SWRST;        /* Soft reset   */
	*AT91C_SPI_CR      = AT91C_SPI_SPIEN;        /* Enable spi   */
	*AT91C_SPI_MR      = AT91C_SPI_MSTR  | AT91C_SPI_MODFDIS | (0xB << 16);
	AT91C_SPI_CSR[2]   = ((CONFIG_SYS_CLOCK / SPI_BITRATE) << 8) | AT91C_SPI_CPOL;
	return(0);
}

static void DrawString(char pString[], uint32_t X, uint32_t Y) {
	uint8_t   *pSource;
	uint8_t   *pDestination;
	FONT    *pFont;
	uint32_t   FontWidth;
	uint32_t   Items;
	uint32_t   Item;
	uint32_t   Line;

	//Get current font information
	pFont = (pMapDisplay->pFont);
	Items = pFont->ItemsX * pFont->ItemsY;

	//Invert Y coordinate to match display buffer
	Y = TRANSLATE_Y(Y);
	Line = (Y & 0xF8) / 8;

	//If text line is out of bounds, do nothing.
	if (Line >= TEXTLINES) return;

	//Calculate pointer to first byte of drawing destination
	pDestination = DISP_BUFFER_P[0];//Line*DISPLAY_WIDTH + X];

	while (*pString) {
		FontWidth = pFont->ItemPixelsX;
		//Calculate X coordinate of the right edge of this character.
		//If it will extend past the right edge, clip the string.
		X += FontWidth;
		if (X >= DISPLAY_WIDTH) break;

		//If Item is defined by the font, display it.  Else, ignore it.
		Item = *pString - ' ';
		if (Item < Items) {
    			pSource = (uint8_t*)&(pFont->Data[Item * FontWidth]);
    			while (FontWidth--) {
				*pDestination = *pSource;
				pDestination++;
				pSource++;
			}
		}
		pString++;
	}
}

static int run_graphic(void){
	SPIInit();
	DrawString( "A", (uint32_t)20, (uint32_t)60);
	return(0);
}

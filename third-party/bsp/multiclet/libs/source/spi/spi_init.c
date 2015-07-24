#include "spi.h"

void spi_init(SPI_TypeDef *SPI, SPI_InitTypeDef* SPI_InitStruct)
{
	int control;
	//Check parameters and set default value
	if((SPI_InitStruct->Three_order_tx) != 0 && (SPI_InitStruct->Three_order_tx) != 1) SPI_InitStruct->Three_order_tx = 0;
	if((SPI_InitStruct->SCK_gap) != 0 && (SPI_InitStruct->SCK_gap) != 1) SPI_InitStruct->SCK_gap = 0;
	if((SPI_InitStruct->FACT) != 0 && (SPI_InitStruct->FACT) != 1) SPI_InitStruct->FACT = 0;
	if((SPI_InitStruct->Three_wire_en) != 0 && (SPI_InitStruct->Three_wire_en) != 1) SPI_InitStruct->Three_wire_en = 0;
	if((SPI_InitStruct->PM) != 0 && (SPI_InitStruct->PM) != 1) SPI_InitStruct->PM = 0;
	if((SPI_InitStruct->Word_len) != 0 && (SPI_InitStruct->Word_len) != 1) SPI_InitStruct->Word_len = 0;
	if((SPI_InitStruct->SPI_en) != 0 && (SPI_InitStruct->SPI_en) != 1) SPI_InitStruct->SPI_en = 0;
	if((SPI_InitStruct->Mode_select) != 0 && (SPI_InitStruct->Mode_select) != 1) SPI_InitStruct->Mode_select = 0;
	if((SPI_InitStruct->Reverse_data) != 0 && (SPI_InitStruct->Reverse_data) != 1) SPI_InitStruct->Reverse_data = 0;
	if((SPI_InitStruct->Divide16) != 0 && (SPI_InitStruct->Divide16) != 1) SPI_InitStruct->Divide16 = 0;
	if((SPI_InitStruct->Clock_phase) != 0 && (SPI_InitStruct->Clock_phase) != 1) SPI_InitStruct->Clock_phase = 0;
	if((SPI_InitStruct->Clock_polarity) != 0 && (SPI_InitStruct->Clock_polarity) != 1) SPI_InitStruct->Clock_polarity = 0;
	if((SPI_InitStruct->Loop_mode) != 0 && (SPI_InitStruct->Loop_mode) != 1) SPI_InitStruct->Loop_mode = 0;

	control = ((SPI_InitStruct->Three_order_tx & 0x01) << 3)|((SPI_InitStruct->SCK_gap & 0x1F) << 7)|((SPI_InitStruct->FACT & 0x01) << 13)|
	((SPI_InitStruct->PM & 0x0F) << 16)|((SPI_InitStruct->Word_len & 0x0F) << 20)|((SPI_InitStruct->SPI_en & 0x01) << 24)|((SPI_InitStruct->Mode_select & 0x01) << 25)|
	((SPI_InitStruct->Reverse_data & 0x01) << 26)|((SPI_InitStruct->Divide16 & 0x01) << 27)|((SPI_InitStruct->Clock_phase & 0x01) << 28)|
	((SPI_InitStruct->Clock_polarity & 0x01) << 29)|((SPI_InitStruct->Loop_mode & 0x01) << 30); //make control reg

	SPI->CR = control; //set control register
}

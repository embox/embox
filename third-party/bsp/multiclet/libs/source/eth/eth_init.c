#include "eth.h"
void eth_init(ETH_InitTypeDef* ETH_InitStruct)
{
	int control;
	//Check parameters and set default value
	if((ETH_InitStruct->Transmit_En) != 0 && (ETH_InitStruct->Transmit_En) != 1) ETH_InitStruct->Transmit_En = 0;
	if((ETH_InitStruct->Receive_En) != 0 && (ETH_InitStruct->Receive_En) != 1) ETH_InitStruct->Receive_En = 0;
	if((ETH_InitStruct->TX_Int_En) != 0 && (ETH_InitStruct->TX_Int_En) != 1) ETH_InitStruct->TX_Int_En = 0;
	if((ETH_InitStruct->RX_Int_En) != 0 && (ETH_InitStruct->RX_Int_En) != 1) ETH_InitStruct->RX_Int_En = 0;
	if((ETH_InitStruct->Full_Duplex) != 0 && (ETH_InitStruct->Full_Duplex) != 1) ETH_InitStruct->Full_Duplex = 0;
	if((ETH_InitStruct->RX_All_Mode) != 0 && (ETH_InitStruct->RX_All_Mode) != 1) ETH_InitStruct->RX_All_Mode = 0;
	if((ETH_InitStruct->Reset) != 0 && (ETH_InitStruct->Reset) != 1) ETH_InitStruct->Reset = 0;
	if((ETH_InitStruct->Speed) != 0 && (ETH_InitStruct->Speed) != 1) ETH_InitStruct->Speed = 0;
	if((ETH_InitStruct->PHY_Int_En) != 0 && (ETH_InitStruct->PHY_Int_En) != 1) ETH_InitStruct->PHY_Int_En = 0;
	if((ETH_InitStruct->Multicast_En) != 0 && (ETH_InitStruct->Multicast_En) != 1) ETH_InitStruct->Multicast_En = 0;

	control = (ETH_InitStruct->Transmit_En & 0x01)|((ETH_InitStruct->Receive_En & 0x01) << 1)|((ETH_InitStruct->TX_Int_En & 0x01) << 2)|
	((ETH_InitStruct->RX_Int_En & 0x01) << 3)|((ETH_InitStruct->Full_Duplex & 0x01) << 4)|((ETH_InitStruct->RX_All_Mode & 0x01) << 5)|
	((ETH_InitStruct->Reset & 0x01) << 6)|((ETH_InitStruct->Speed & 0x01) << 7)|((ETH_InitStruct->PHY_Int_En & 0x01) << 10)|
	((ETH_InitStruct->Multicast_En & 0x01) << 11); //make control reg

	ETH->CR = control; //set control register
}

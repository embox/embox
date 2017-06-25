#include "pwm.h"

void pwm_init(PWM_InitTypeDef* PWM_InitStruct)
{
	int control;
	//Check parameters and set default value
	if((PWM_InitStruct->Single_mode_en) != 0 && (PWM_InitStruct->Single_mode_en) != 1) PWM_InitStruct->Single_mode_en = 0;
	if((PWM_InitStruct->Change_period_en) != 0 && (PWM_InitStruct->Change_period_en) != 1) PWM_InitStruct->Change_period_en = 0;
	if((PWM_InitStruct->Select_mode) != 0 && (PWM_InitStruct->Select_mode) != 1 && (PWM_InitStruct->Select_mode) != 2) PWM_InitStruct->Select_mode = 0;
	if((PWM_InitStruct->Irq_full_en) != 0 && (PWM_InitStruct->Irq_full_en) != 1) PWM_InitStruct->Irq_full_en = 0;
	if((PWM_InitStruct->CH0_en) != 0 && (PWM_InitStruct->CH0_en) != 1) PWM_InitStruct->CH0_en = 0;
	if((PWM_InitStruct->CH1_en) != 0 && (PWM_InitStruct->CH1_en) != 1) PWM_InitStruct->CH1_en = 0;
	if((PWM_InitStruct->CH2_en) != 0 && (PWM_InitStruct->CH2_en) != 1) PWM_InitStruct->CH2_en = 0;
	if((PWM_InitStruct->CH3_en) != 0 && (PWM_InitStruct->CH3_en) != 1) PWM_InitStruct->CH3_en = 0;
	if((PWM_InitStruct->Set_na_off_lvl_CH0) != 0 && (PWM_InitStruct->Set_na_off_lvl_CH0) != 1) PWM_InitStruct->Set_na_off_lvl_CH0 = 0;
	if((PWM_InitStruct->Set_na_off_lvl_CH1) != 0 && (PWM_InitStruct->Set_na_off_lvl_CH1) != 1) PWM_InitStruct->Set_na_off_lvl_CH1 = 0;
	if((PWM_InitStruct->Set_na_off_lvl_CH2) != 0 && (PWM_InitStruct->Set_na_off_lvl_CH2) != 1) PWM_InitStruct->Set_na_off_lvl_CH2 = 0;
	if((PWM_InitStruct->Set_na_off_lvl_CH3) != 0 && (PWM_InitStruct->Set_na_off_lvl_CH3) != 1) PWM_InitStruct->Set_na_off_lvl_CH3 = 0;
	if((PWM_InitStruct->Irq_CH0_en) != 0 && (PWM_InitStruct->Irq_CH0_en) != 1) PWM_InitStruct->Irq_CH0_en = 0;
	if((PWM_InitStruct->Irq_CH1_en) != 0 && (PWM_InitStruct->Irq_CH1_en) != 1) PWM_InitStruct->Irq_CH1_en = 0;
	if((PWM_InitStruct->Irq_CH2_en) != 0 && (PWM_InitStruct->Irq_CH2_en) != 1) PWM_InitStruct->Irq_CH2_en = 0;
	if((PWM_InitStruct->Irq_CH3_en) != 0 && (PWM_InitStruct->Irq_CH3_en) != 1) PWM_InitStruct->Irq_CH3_en = 0;
	if((PWM_InitStruct->Set_a_on_lvl_CH0) != 0 && (PWM_InitStruct->Set_a_on_lvl_CH0) != 1) PWM_InitStruct->Set_a_on_lvl_CH0 = 0;
	if((PWM_InitStruct->Set_a_on_lvl_CH1) != 0 && (PWM_InitStruct->Set_a_on_lvl_CH1) != 1) PWM_InitStruct->Set_a_on_lvl_CH1 = 0;
	if((PWM_InitStruct->Set_a_on_lvl_CH2) != 0 && (PWM_InitStruct->Set_a_on_lvl_CH2) != 1) PWM_InitStruct->Set_a_on_lvl_CH2 = 0;
	if((PWM_InitStruct->Set_a_on_lvl_CH3) != 0 && (PWM_InitStruct->Set_a_on_lvl_CH3) != 1) PWM_InitStruct->Set_a_on_lvl_CH3 = 0;

	control = ((PWM_InitStruct->Single_mode_en & 0x01))|((PWM_InitStruct->Change_period_en & 0x01) << 1)|((PWM_InitStruct->Select_mode & 0x03) << 2)|
	((PWM_InitStruct->Irq_full_en & 0x01) << 4)|((PWM_InitStruct->CH0_en & 0x01) << 8)|((PWM_InitStruct->CH1_en & 0x01) << 9)|((PWM_InitStruct->CH2_en & 0x01) << 10)|
	((PWM_InitStruct->CH3_en & 0x01) << 11)|((PWM_InitStruct->Set_na_off_lvl_CH0 & 0x01) << 12)|((PWM_InitStruct->Set_na_off_lvl_CH1 & 0x01) << 13)|
	((PWM_InitStruct->Set_na_off_lvl_CH2 & 0x01) << 14)|((PWM_InitStruct->Set_na_off_lvl_CH3 & 0x01) << 15)|((PWM_InitStruct->Irq_CH0_en & 0x01) << 16)|
	((PWM_InitStruct->Irq_CH1_en & 0x01) << 17)|((PWM_InitStruct->Irq_CH2_en & 0x01) << 18)|((PWM_InitStruct->Irq_CH3_en & 0x01) << 19)|
	((PWM_InitStruct->Set_a_on_lvl_CH0 & 0x01) << 20)|((PWM_InitStruct->Set_a_on_lvl_CH1 & 0x01) << 21)|((PWM_InitStruct->Set_a_on_lvl_CH2 & 0x01) << 22)|
	((PWM_InitStruct->Set_a_on_lvl_CH3 & 0x01) << 23); //make control reg

	PWM->CR = control; //set control register
}

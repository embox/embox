#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <kernel/irq.h>

#include "K1921VG015.h"

#define GPIOA_ALL_Msk 0xFFFF
#define GPIOB_ALL_Msk 0xFFFF

#define LEDS_MSK 0xFF00
#define LED0_MSK (1 << 8)
#define LED1_MSK (1 << 9)
#define LED2_MSK (1 << 10)
#define LED3_MSK (1 << 11)
#define LED4_MSK (1 << 12)
#define LED5_MSK (1 << 13)
#define LED6_MSK (1 << 14)
#define LED7_MSK (1 << 15)

volatile uint32_t led_shift;

void BSP_led_init() {
	//Разрешаем тактирование GPIOA
	RCU->CGCFGAHB_bit.GPIOAEN = 1;
	//Включаем  GPIOA
	RCU->RSTDISAHB_bit.GPIOAEN = 1;
	GPIOA->OUTENSET = LEDS_MSK;
	GPIOA->DATAOUTSET = LEDS_MSK;
}

void TMR1_PWN_init(uint16_t period) {
	RCU->CGCFGAPB_bit.TMR1EN = 1;
	RCU->RSTDISAPB_bit.TMR1EN = 1;

	//Настраиваем альтернативную функцию для GPIOA.8 и GPIOA.9
	RCU->CGCFGAHB_bit.GPIOAEN = 1;
	//Включаем  GPIOA
	RCU->RSTDISAHB_bit.GPIOAEN = 1;
	// Выбираем льтернативную функцию №2 для GPIOA.8 и GPIOA.9
	GPIOA->ALTFUNCNUM_bit.PIN8 = 2;
	GPIOA->ALTFUNCNUM_bit.PIN9 = 2;
	GPIOA->ALTFUNCSET = (1 << 8) | (1 << 9);

	//Записываем значение периода в CAPCOM[0]
	TMR1->CAPCOM[0].VAL = period - 1;

	//Настраиваем режим сравнения для CAPCOM[2] - управление выводом GPIOA.8
	TMR1->CAPCOM[2].CTRL_bit.CAP = 0; // Режим сравнения
	TMR1->CAPCOM[2].CTRL_bit.OUTMODE =
	    7; /* Выходной сигнал сбрасывается, когда таймер отсчитывает значение CAPCOM[n].VAL.
  Он устанавливается, когда таймер отсчитывает значение CAPCOM[0].VAL.*/
	TMR1->CAPCOM[2].VAL = 9000;

	//Настраиваем режим сравнения для CAPCOM[3] - управление выводом GPIOA.9
	TMR1->CAPCOM[3].CTRL_bit.CAP = 0; // Режим сравнения
	TMR1->CAPCOM[3].CTRL_bit.OUTMODE =
	    3; /* Выходной сигнал устанавливается, когда таймер отсчитывает значение CAPCOM[n].VAL.
  Он сбрасывается, когда таймер отсчитывает значение CAPCOM[0].VAL.*/
	TMR1->CAPCOM[3].VAL = 9000;

	TMR1->CTRL_bit.DIV = 3; // Делитель частоты на 8
	//Выбираем режим счета от 0 до значения CAPCOM[0]
	TMR1->CTRL_bit.MODE = TMR_CTRL_MODE_Up;

	//Разрешаем прерывание по совпадению значения счетчика и CAPCOM[0]
	TMR1->IM = 2;
}

void TMR1_PWN_init1(uint16_t period) {
	RCU->CGCFGAPB_bit.TMR1EN = 1;
	RCU->RSTDISAPB_bit.TMR1EN = 1;

	//Настраиваем альтернативную функцию для GPIOA.8 и GPIOA.9
	RCU->CGCFGAHB_bit.GPIOAEN = 1;
	//Включаем  GPIOA
	RCU->RSTDISAHB_bit.GPIOAEN = 1;
	// Выбираем льтернативную функцию №2 для GPIOA.8 и GPIOA.9
	GPIOA->ALTFUNCNUM_bit.PIN8 = 2;
	GPIOA->ALTFUNCNUM_bit.PIN9 = 2;
	GPIOA->ALTFUNCSET = (1 << 8) | (1 << 9);

	//Записываем значение периода в CAPCOM[0]
	TMR1->CAPCOM[0].VAL = period - 1;

	//Настраиваем режим сравнения для CAPCOM[2] - управление выводом GPIOA.8
	TMR1->CAPCOM[2].CTRL_bit.CAP = 0; // Режим сравнения
	TMR1->CAPCOM[2].CTRL_bit.OUTMODE =
	    7; /* Выходной сигнал сбрасывается, когда таймер отсчитывает значение CAPCOM[n].VAL.
  Он устанавливается, когда таймер отсчитывает значение CAPCOM[0].VAL.*/
	TMR1->CAPCOM[2].VAL = 0x20;

	//Настраиваем режим сравнения для CAPCOM[3] - управление выводом GPIOA.9
	TMR1->CAPCOM[3].CTRL_bit.CAP = 0; // Режим сравнения
	TMR1->CAPCOM[3].CTRL_bit.OUTMODE =
	    3; /* Выходной сигнал устанавливается, когда таймер отсчитывает значение CAPCOM[n].VAL.
  Он сбрасывается, когда таймер отсчитывает значение CAPCOM[0].VAL.*/
	TMR1->CAPCOM[3].VAL = 0x20;

	TMR1->CTRL_bit.DIV = 3; // Делитель частоты на 8
	//Выбираем режим счета от 0 до значения CAPCOM[0]
	TMR1->CTRL_bit.MODE = TMR_CTRL_MODE_Up;

	//Разрешаем прерывание по совпадению значения счетчика и CAPCOM[0]
	TMR1->IM = 2;
}

void TMR32_init(uint32_t period) {
	RCU->CGCFGAPB_bit.TMR32EN = 1;
	RCU->RSTDISAPB_bit.TMR32EN = 1;

	//Записываем значение периода в CAPCOM[0]
	TMR32->CAPCOM[0].VAL = period - 1;
	//Выбираем режим счета от 0 до значения CAPCOM[0]
	TMR32->CTRL_bit.MODE = 1;

	//Разрешаем прерывание по совпадению значения счетчика и CAPCOM[0]
	TMR32->IM = 2;
}

static irq_return_t tmr_int_handler(unsigned int irq_nr, void *dev_id) {
	GPIOA->DATAOUTTGL = led_shift;
	led_shift = led_shift << 1;
	if (led_shift > LED7_MSK) {
		led_shift = LED2_MSK;
		// Изменяем скважность сигнала ШИМ
		TMR1->CAPCOM[2].VAL += 0x1000;
		TMR1->CAPCOM[3].VAL -= 0x1000;
	}
	// Сбрасываем флаг прерывания таймера
	TMR32->IC = 3;

	return IRQ_HANDLED;
}

int main(int argc, char const *argv[]) {
	led_shift = LED2_MSK;

	BSP_led_init();
	TMR32_init(10000);
	TMR1_PWN_init(10000);
	irq_attach(IsrVect_IRQ_TMR32, tmr_int_handler, 0, NULL, "TMR32");

	sleep(4);

	irq_detach(IsrVect_IRQ_TMR32, NULL);

	BSP_led_init();
	TMR32_init(10000);
	TMR1_PWN_init1(10000);
	irq_attach(IsrVect_IRQ_TMR32, tmr_int_handler, 0, NULL, "TMR32");

	sleep(4);

	irq_detach(IsrVect_IRQ_TMR32, NULL);

	return 0;
}

/**
 * @file
 *
 * @date 17.10.10
 * @author Darya Dzendzik
 */

#include <types.h>
#include <unistd.h>

#include <embox/example.h>
#include <drivers/nxt/lcd.h>
#include <drivers/nxt/buttons.h>

/* picture "embox"*/

EMBOX_EXAMPLE(run_picture);

/* WARNING: function too long */
static int run_picture(void) {
/*Part of "E" "M" "B" "O" */
	uint8_t pict_buff1[8] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t pict_buff2[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0};
	uint8_t pict_buff4[8] = {0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t pict_buff3[8] = {0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF};
/*part of "X"*/
	uint8_t pict_buff5[8] = {0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3};
	uint8_t pict_buff8[8] = {0x00, 0x00, 0x00, 0x00, 0xC0, 0xF0, 0xFC, 0xFF};
	uint8_t pict_buff9[8] = {0xFF, 0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00};
	uint8_t pict_buff7[8] = {0xFF, 0xFC, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00};
	uint8_t pict_buff6[8] = {0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x3F, 0xFF};
	uint8_t pict_buff10[8] = {0xFF, 0x3F, 0x0F, 0x03, 0x03, 0x0F, 0x3F, 0xFF};
	uint8_t pict_buff11[8] = {0xFF, 0xFC, 0xF0, 0xC0, 0xC0, 0xF0, 0xFC, 0xFF};

	uint32_t b;

	display_clear_screen();

	b = nxt_buttons_pressed();
	if (b != 0) {
		return 0;
	}

#if 0
/*e*/
	/*2*/	display_draw(2, 2, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 2, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 3, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 4, 1, 8, &pict_buff2[0]);
/*m*/
	/*4*/	display_draw(18, 2, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 2, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 2, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 3, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 3, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
/*b*/
	/*1*/	display_draw(42, 1, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 3, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 4, 1, 8, &pict_buff4[0]);
/*o*/
	/*8*/	display_draw(58, 2, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 2, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 3, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 4, 1, 8, &pict_buff4[0]);
/*x*/
	/*10*/	display_draw(74, 2, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 2, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 2, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 3, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 4, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 4, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 4, 1, 8, &pict_buff7[0]);
#else
	for (int i = 0; i < 6; i++) {
		/*e*/
		/*2*/	display_draw(2, i-2, 1, 8, &pict_buff1[0]);
		/*3*/	display_draw(10, i-2, 1, 8, &pict_buff3[0]);
		/*13*/	display_draw(2, i-1, 1, 8, &pict_buff1[0]);
		/*14*/	display_draw(10, i-1, 1, 8, &pict_buff4[0]);
		/*23*/	display_draw(2, i, 1, 8, &pict_buff1[0]);
		/*24*/	display_draw(10, i, 1, 8, &pict_buff2[0]);


		/*m*/
		/*4*/	display_draw(18, i-2, 1, 8, &pict_buff1[0]);
		/*5*/	display_draw(26, i-2, 1, 8, &pict_buff2[0]);
		/*6*/	display_draw(34, i-2, 1, 8, &pict_buff2[0]);
		/*15*/	display_draw(18, i-1, 1, 8, &pict_buff1[0]);
		/*16*/	display_draw(26, i-1, 1, 8, &pict_buff1[0]);
		/*17*/	display_draw(34, i-1, 1, 8, &pict_buff1[0]);
		/*25*/	display_draw(18, i, 1, 8, &pict_buff1[0]);
		/*26*/	display_draw(26, i, 1, 8, &pict_buff1[0]);
		/*27*/	display_draw(34, i, 1, 8, &pict_buff1[0]);

		/*b*/
		/*1*/	display_draw(42, i-3, 1, 8, &pict_buff1[0]);
		/*7*/	display_draw(42, i-2, 1, 8, &pict_buff1[0]);
		/*18*/	display_draw(42, i-1, 1, 8, &pict_buff1[0]);
		/*19*/	display_draw(50, i-1, 1, 8, &pict_buff3[0]);
		/*28*/	display_draw(42, i, 1, 8, &pict_buff1[0]);
		/*29*/	display_draw(50, i, 1, 8, &pict_buff4[0]);

		/*o*/
		/*8*/	display_draw(58, i-2, 1, 8, &pict_buff1[0]);
		/*9*/	display_draw(66, i-2, 1, 8, &pict_buff3[0]);
		/*20*/	display_draw(58, i-1, 1, 8, &pict_buff1[0]);
		/*21*/	display_draw(66, i-1, 1, 8, &pict_buff1[0]);
		/*30*/	display_draw(58, i, 1, 8, &pict_buff1[0]);
		/*31*/	display_draw(66, i, 1, 8, &pict_buff4[0]);

		/*x*/
		/*10*/	display_draw(74, i-2, 1, 8, &pict_buff6[0]);
		/*11*/	display_draw(82, i-2, 1, 8, &pict_buff11[0]);
		/*12*/	display_draw(90, i-2, 1, 8, &pict_buff9[0]);
		/*22*/	display_draw(82, i-1, 1, 8, &pict_buff5[0]);
		/*32*/	display_draw(74, i, 1, 8, &pict_buff8[0]);
		/*33*/	display_draw(82, i, 1, 8, &pict_buff10[0]);
		/*34*/	display_draw(90, i, 1, 8, &pict_buff7[0]);
		usleep(250);
		display_clear_screen();

	}
	for (int i = 0; i < 5; i++) {
	/*stay*/
		b = nxt_buttons_pressed();
		if (b!=0) {
			return 0;
		}


	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);

	/*try to jamp*/
	display_clear_screen();
/*e*/
	/*e*/
	/*2*/	display_draw(2, 2, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 2, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 3, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 4, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();

/*stay*/

	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();

/*m*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 2, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 2, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 2, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 3, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 3, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();


/*stay*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();

/*b*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 1, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 3, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 4, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();

/*stay*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();

/*o*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 2, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 2, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 3, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 4, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();


/*stay*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();


/*x*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 2, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 2, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 2, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 3, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 4, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 4, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 4, 1, 8, &pict_buff7[0]);
	usleep(250);
	display_clear_screen();


/*stay*/
	/*e*/
	/*2*/	display_draw(2, 3, 1, 8, &pict_buff1[0]);
	/*3*/	display_draw(10, 3, 1, 8, &pict_buff3[0]);
	/*13*/	display_draw(2, 4, 1, 8, &pict_buff1[0]);
	/*14*/	display_draw(10, 4, 1, 8, &pict_buff4[0]);
	/*23*/	display_draw(2, 5, 1, 8, &pict_buff1[0]);
	/*24*/	display_draw(10, 5, 1, 8, &pict_buff2[0]);

	/*m*/
	/*4*/	display_draw(18, 3, 1, 8, &pict_buff1[0]);
	/*5*/	display_draw(26, 3, 1, 8, &pict_buff2[0]);
	/*6*/	display_draw(34, 3, 1, 8, &pict_buff2[0]);
	/*15*/	display_draw(18, 4, 1, 8, &pict_buff1[0]);
	/*16*/	display_draw(26, 4, 1, 8, &pict_buff1[0]);
	/*17*/	display_draw(34, 4, 1, 8, &pict_buff1[0]);
	/*25*/	display_draw(18, 5, 1, 8, &pict_buff1[0]);
	/*26*/	display_draw(26, 5, 1, 8, &pict_buff1[0]);
	/*27*/	display_draw(34, 5, 1, 8, &pict_buff1[0]);

	/*b*/
	/*1*/	display_draw(42, 2, 1, 8, &pict_buff1[0]);
	/*7*/	display_draw(42, 3, 1, 8, &pict_buff1[0]);
	/*18*/	display_draw(42, 4, 1, 8, &pict_buff1[0]);
	/*19*/	display_draw(50, 4, 1, 8, &pict_buff3[0]);
	/*28*/	display_draw(42, 5, 1, 8, &pict_buff1[0]);
	/*29*/	display_draw(50, 5, 1, 8, &pict_buff4[0]);

	/*o*/
	/*8*/	display_draw(58, 3, 1, 8, &pict_buff1[0]);
	/*9*/	display_draw(66, 3, 1, 8, &pict_buff3[0]);
	/*20*/	display_draw(58, 4, 1, 8, &pict_buff1[0]);
	/*21*/	display_draw(66, 4, 1, 8, &pict_buff1[0]);
	/*30*/	display_draw(58, 5, 1, 8, &pict_buff1[0]);
	/*31*/	display_draw(66, 5, 1, 8, &pict_buff4[0]);

	/*x*/
	/*10*/	display_draw(74, 3, 1, 8, &pict_buff6[0]);
	/*11*/	display_draw(82, 3, 1, 8, &pict_buff11[0]);
	/*12*/	display_draw(90, 3, 1, 8, &pict_buff9[0]);
	/*22*/	display_draw(82, 4, 1, 8, &pict_buff5[0]);
	/*32*/	display_draw(74, 5, 1, 8, &pict_buff8[0]);
	/*33*/	display_draw(82, 5, 1, 8, &pict_buff10[0]);
	/*34*/	display_draw(90, 5, 1, 8, &pict_buff7[0]);
	}

#endif
	return 0;
}

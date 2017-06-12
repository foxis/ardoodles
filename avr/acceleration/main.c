#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include "nokia3310.h"

int main(void)
{
	DDRD = 0x00;
	DDRB = 0xFF;

	LcdInit();
	
	LcdClear();
	
	while (1)
	{
//		LcdGotoXY(1,1);
//		LcdChr(FONT_2X, 'A');
//		LcdUpdate();
		
		PORTB ^= 0x01;
		PORTB ^= 0x02;
		PORTB ^= 0x04;
		PORTB ^= 0x08;
		PORTB ^= 0x10;
		PORTB ^= 0x20;
		PORTB ^= 0x40;
		PORTB ^= 0x80;
	}
	
	while (1) ;

	return 0;
}


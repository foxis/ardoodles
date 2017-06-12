#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>

typedef union
{
	struct
	{
		unsigned char hi;
		unsigned char lo;
	} bits8;
	unsigned short bits16;
} short_t;

static volatile signed char gs_c = 0;
static volatile unsigned short gs_j = 0;
static volatile signed char gs_d = 1;
static volatile unsigned short gs_off = 0;
static volatile unsigned short gs_num = 1;

#define MAX_FRAMES 96
#define FRAME_SIZE

static volatile unsigned short g_pixels[MAX_FRAMES*FRAME_SIZE]={0,};


unsigned short g_columns[16] = 
{
/**/
  0b0000000100000001U,
  0b0000001000000010U,
  0b0000010000000100U,
  0b0000100000001000U,
  0b0001000000010000U,
  0b0010000000100000U,
  0b0100000001000000U,
  0b1000000010000000U,
  0b1000000010000000U,
  0b0100000001000000U,
  0b0010000000100000U,
  0b0001000000010000U,
  0b0000100000001000U,
  0b0000010000000100U,
  0b0000001000000010U,
  0b0000000100000001U,/**/

/** /
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111,
  0b1111111111111111, /**/

/** /
  0b1U<<0,
  0b1U<<1,
  0b1U<<2,
  0b1U<<3,
  0b1U<<4,
  0b1U<<5,
  0b1U<<6,
  0b1U<<7,
  0b1U<<15,
  0b1U<<14,
  0b1U<<13,
  0b1U<<12,
  0b1U<<11,
  0b1U<<10,
  0b1U<<9,
  0b1U<<8,
  0b1U<<8,
  /**/
/** /
  0b1U<<0,
  0b1U<<1,
  0b1U<<0,
  0b1U<<1,
  0b1U<<0,
  0b1U<<1,
  0b1U<<0,
  0b1U<<1,
  0b1U<<0,
  0b1U<<1,
  0b1U<<0,
  0b1U<<1,
  0b1U<<0,
  0b1U<<1,
  0b1U<<0,
  0b1U<<1,
  /**/
};

// tranzu bazes:
// E - nuo 7
// F - nuo 0

// ledai:
// A - nuo 0
// C - nuo 7

void delay_ms(short i)
{
	while (i--)
		_delay_ms(1);
}

int main(void)
{
	signed char c;
	unsigned char j;
	signed char d;
	short_t w;
	DDRE = 0xFF;
	DDRF = 0xff;
	DDRA = 0xff;
	DDRC = 0xff;
	c = 0;
	j = 0;
	d = 1;
	while (1)
	{
		PORTC = 0;
		PORTA = 0;

		w = ((short_t)g_columns[j]);
		if ( ++j==16 )
			j = 0;

		if ( d==1 )
		{
			PORTE = 0x80>>c;
			PORTF = 0;

			c += d;
			if ( c==8 )
			{
				d = -1;
				c = 7;
			}
		}
		else
		{
			PORTF = 0x80>>c;
			PORTE = 0;

			c += d;
			if ( c==-1 )
			{
				d = 1;
				c = 0;
			}
		}
		
		PORTC = w.bits8.hi;
		PORTA = w.bits8.lo;
		
		_delay_us(10);
	}
	
	return 0;
}

ISR()
{
}
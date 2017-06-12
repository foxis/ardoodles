#include <inttypes.h>
#include <stdlib.h>
#include <avr\io.h>
#include <util\delay.h>
#include <math.h>

void delay(int i)
{
	while (i--)
	{
		_delay_us(1);
	}
}
unsigned char logtable[256] =
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 
 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 
 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 
 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 
 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 
 22, 22, 23, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 
 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 
 46, 47, 48, 49, 51, 52, 53, 54, 55, 56, 58, 59, 60, 62, 63, 64, 66, 
 67, 69, 70, 72, 73, 75, 77, 78, 80, 82, 84, 86, 87, 89, 91, 93, 95, 
 98, 100, 102, 104, 106, 109, 111, 114, 116, 119, 121, 124, 127, 130,
 132, 135, 138, 141, 144, 148, 151, 154, 158, 161, 165, 168, 172, 176, 
 180, 184, 188, 192, 196, 200, 205, 209, 214, 219, 223, 228, 233, 238, 244, 249, 255};


void init_rgb(void)
{
	unsigned short i;
	DDRD = 0xFF;
	ICR1 = 0xFF;
	TCNT1 = 0;
	OCR1A = 0x09;
	OCR1B = 0x2F;
	TCCR1A = _BV(WGM10) | _BV(COM1A1) | _BV(COM1B1);
	TCCR1B = _BV(CS11) | _BV(WGM12);

	TCCR2 = _BV(WGM20)     // timer 2 fast PWM
      | _BV(WGM21)     // ditto
      | _BV(COM21)     // clear OC2 on compare match, set OC2 at TOP
      | _BV(CS21);

//	for ( i=0;i<256;i++ )
//	{
//		logtable[i] = (unsigned char)pow(2.718282, log(255)*i/255.0);
//	}

}

void hsv2rgb(unsigned char h, unsigned char s, unsigned char v, unsigned char* r, unsigned char* g, unsigned char* b)

{
  if ( s == 0 )
  {
     *r = v;
     *g = v;
     *b = v;
  }
  else
  {
     float var_h = (h/255.0) * 6.0;
     unsigned char var_i = floor( var_h );
     float var_1 = v * ( 1.0 - s/255.0 );
     float var_2 = v * ( 1.0 - s * ( var_h - var_i )/255.0 );
     float var_3 = v * ( 1.0 - s * ( 1.0 - ( var_h - var_i ) )/255.0 );
 
     if      ( var_i == 0 )	{ *r = v     ; *g = var_3 ; *b = var_1; }
     else if ( var_i == 1 )	{ *r = var_2 ; *g = v     ; *b = var_1; }
     else if ( var_i == 2 )	{ *r = var_1 ; *g = v     ; *b = var_3; }
     else if ( var_i == 3 )	{ *r = var_1 ; *g = var_2 ; *b = v;     }
     else if ( var_i == 4 )	{ *r = var_3 ; *g = var_1 ; *b = v;     }
     else					{ *r = v     ; *g = var_1 ; *b = var_2; }
  }
}

void set_rgb(unsigned char r, unsigned char g, unsigned char b)
{
	OCR1A = 255-logtable[r];
	OCR1B = 255-logtable[g];
	OCR2 = 255 -logtable[b];
}

void main(void)
{
	unsigned char r=0, g=0, b=0, h=0;
	
	init_rgb();
	
	
	while (1)
	{
		PORTD ^= 1;
		
		hsv2rgb(h, 255, 255, &r, &g, &b);
		set_rgb(r, g, b);
		
		delay(10000);
		
		r++;
		r &= 0x7F;
		g += (g==0);
		g &= 0x7F;
		
		h++;
	}
}

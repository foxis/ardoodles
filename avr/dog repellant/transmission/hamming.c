/*  hamming.c - contains code for generating and checking
 *              (12,8) Hamming codes
 *
 *  Copyright (C) 2000  Jack Klein
 *                      Macmillan Computer Publishing
 *
 *  This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General
 *  Public License along with this program; if not, write
 *  to the Free Software Foundation, Inc., 675 Mass Ave,
 *  Cambridge, MA 02139, USA.
 *
 *  Jack Klein may be contacted by email at:
 *     The_C_Guru@yahoo.com
 *
 */ 
 
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "hamming.h"
//#define PROGMEM 
//#define pgm_read_byte(a) (*(a))
const int hamming[256] PROGMEM =
{
  0X00,  0X03,  0X05,  0X06,  0X06,  0X05,  0X03,  0X00,
  0X07,  0X04,  0X02,  0X01,  0X01,  0X02,  0X04,  0X07,
  0X09,  0X0A,  0X0C,  0X0F,  0X0F,  0X0C,  0X0A,  0X09,
  0X0E,  0X0D,  0X0B,  0X08,  0X08,  0X0B,  0X0D,  0X0E,
  0X0A,  0X09,  0X0F,  0X0C,  0X0C,  0X0F,  0X09,  0X0A,
  0X0D,  0X0E,  0X08,  0X0B,  0X0B,  0X08,  0X0E,  0X0D,
  0X03,  0X00,  0X06,  0X05,  0X05,  0X06,  0X00,  0X03,
  0X04,  0X07,  0X01,  0X02,  0X02,  0X01,  0X07,  0X04,
  0X0B,  0X08,  0X0E,  0X0D,  0X0D,  0X0E,  0X08,  0X0B,
  0X0C,  0X0F,  0X09,  0X0A,  0X0A,  0X09,  0X0F,  0X0C,
  0X02,  0X01,  0X07,  0X04,  0X04,  0X07,  0X01,  0X02,
  0X05,  0X06,  0X00,  0X03,  0X03,  0X00,  0X06,  0X05,
  0X01,  0X02,  0X04,  0X07,  0X07,  0X04,  0X02,  0X01,
  0X06,  0X05,  0X03,  0X00,  0X00,  0X03,  0X05,  0X06,
  0X08,  0X0B,  0X0D,  0X0E,  0X0E,  0X0D,  0X0B,  0X08,
  0X0F,  0X0C,  0X0A,  0X09,  0X09,  0X0A,  0X0C,  0X0F,
  0X0C,  0X0F,  0X09,  0X0A,  0X0A,  0X09,  0X0F,  0X0C,
  0X0B,  0X08,  0X0E,  0X0D,  0X0D,  0X0E,  0X08,  0X0B,
  0X05,  0X06,  0X00,  0X03,  0X03,  0X00,  0X06,  0X05,
  0X02,  0X01,  0X07,  0X04,  0X04,  0X07,  0X01,  0X02,
  0X06,  0X05,  0X03,  0X00,  0X00,  0X03,  0X05,  0X06,
  0X01,  0X02,  0X04,  0X07,  0X07,  0X04,  0X02,  0X01,
  0X0F,  0X0C,  0X0A,  0X09,  0X09,  0X0A,  0X0C,  0X0F,
  0X08,  0X0B,  0X0D,  0X0E,  0X0E,  0X0D,  0X0B,  0X08,
  0X07,  0X04,  0X02,  0X01,  0X01,  0X02,  0X04,  0X07,
  0X00,  0X03,  0X05,  0X06,  0X06,  0X05,  0X03,  0X00,
  0X0E,  0X0D,  0X0B,  0X08,  0X08,  0X0B,  0X0D,  0X0E,
  0X09,  0X0A,  0X0C,  0X0F,  0X0F,  0X0C,  0X0A,  0X09,
  0X0D,  0X0E,  0X08,  0X0B,  0X0B,  0X08,  0X0E,  0X0D,
  0X0A,  0X09,  0X0F,  0X0C,  0X0C,  0X0F,  0X09,  0X0A,
  0X04,  0X07,  0X01,  0X02,  0X02,  0X01,  0X07,  0X04,
  0X03,  0X00,  0X06,  0X05,  0X05,  0X06,  0X00,  0X03,
};

const int ham_pos_values [8] PROGMEM=
{ 3, 5, 6, 7, 9, 10, 11, 12 };

unsigned int corrections [16] PROGMEM=
{
  0x00,   /* 0 result means no error  */
  0x00,   /*  C1 is a Hamming bit, not a data bit */
  0x00,   /*  C2 is a Hamming bit, not a data bit */
  0x01,   /*  C3 is data bit D0                   */
  0x00,   /*  C4 is a Hamming bit, not a data bit */
  0x02,   /*  C5 is data bit D1                   */
  0x04,   /*  C6 is data bit D2                   */
  0x08,   /*  C7 is data bit D3                   */
  0x00,   /*  C8 is a Hamming bit, not a data bit */
  0x10,   /*  C9 is data bit D4                   */
  0x20,   /* C10 is data bit D5                   */
  0x40,   /* C11 is data bit D6                   */
  0x80,   /* C12 is data bit D7                   */
}; 


/**
 *  The following functions contain encode and decode algorithms for hamming codes
 *
 *  Copyright(c) 2006, Andrius Mikonis
 * 
 */

unsigned char hamming_encode(unsigned char* out, const unsigned char* in, unsigned char cb)
{
	unsigned char b, j, i, _cb=0;
	unsigned char code;
	unsigned short comb;

	for ( j=0;j<cb;j++ )
	{
		b = in[j];
		code = pgm_read_byte(hamming+b);
		code |= code<<4;
		
		i = 0x80;
		comb = b&i; comb<<=1;
		comb |= code&i;
		i>>=1;
		comb |= b&i; comb<<=1;
		comb |= code&i;
		i>>=1;
		comb |= b&i; comb<<=1;
		comb |= code&i;
		i>>=1;
		comb |= b&i; comb<<=1;
		comb |= code&i;
		i>>=1;
		comb |= b&i; comb<<=1;
		comb |= code&i;
		i>>=1;
		comb |= b&i; comb<<=1;
		comb |= code&i;
		i>>=1;
		comb |= b&i; comb<<=1;
		comb |= code&i;
		i>>=1;
		comb |= b&i; comb<<=1;
		comb |= code&i;
		
		out[_cb++] = (((comb&0xFF00)>>8));
		out[_cb++] = ((comb&0xFF));
	}
	return _cb;
}
//  ham = hamming[value];
//  modified = value ^ (1 << the_bit);
//  mod_ham = hamming[modified];
//  printf("Original value 0x%02X Hamming Code 0x%X\n",
//          value, ham);
//  printf("Modified value 0x%02X Hamming Code 0x%X\n",
//          modified, mod_ham);
//  printf("Exclusive OR of the Hamming Codes is 0x%X\n",
//          ham ^ mod_ham);
//  printf("Correction mask is 0x%02X\n",
//          corrections[ham ^ mod_ham]);
//  printf("Corrected value is 0x%02X\n\n",
//          modified ^ corrections[ham ^ mod_ham]); 
unsigned char hamming_decode(unsigned char* out, const unsigned char* in, unsigned char cb)
{
	unsigned char b, _cb=0;
	unsigned char code, code1, code2;
	unsigned short comb,i;
	unsigned char corr1, corr2;
	unsigned char tmp;

	while ( cb )
	{
		comb = (((unsigned short)in[0])<<8) | in[1];
		in += 2;
		
		i = 0x8000;
		b = code = 0;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		b<<=1;	code<<=1;	i>>=1;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		b<<=1;	code<<=1;	i>>=1;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		b<<=1;	code<<=1;	i>>=1;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		b<<=1;	code<<=1;	i>>=1;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		b<<=1;	code<<=1;	i>>=1;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		b<<=1;	code<<=1;	i>>=1;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		b<<=1;	code<<=1;	i>>=1;
		b |= !!(comb&i);	i>>=1; code |= !!(comb&i);
		
		code1 = code&0x0f;
		code2 = (code&0xf0)>>4;
		tmp = pgm_read_byte(hamming+b);
		corr1 = pgm_read_byte(corrections+ (code1 ^ tmp));
		corr2 = pgm_read_byte(corrections+ (code2 ^ tmp));
		
		if ( code1==code2 && corr1!=0 )
		{	
			*(out++) = b ^ corr1;	
		}
		else if ( corr1==0 )
		{	
			*(out++) = b^corr2;
		}
		else if ( corr2==0 )
		{	
			*(out++) = b^corr1;
		}
		else
		{	
			*(out++) = b;
		}
		_cb++;
	
		cb-=2;
	}
	
	return _cb;
}

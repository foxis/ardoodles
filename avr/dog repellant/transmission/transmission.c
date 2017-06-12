/** 
 * Remote car alarm
 * 2004-2007 (c) Andrius Mikonis, vienas@delfi.lt
 *
 * You are free to use this code or parts of it in your applications
 * as long as you put a credit for me :)
 *
 * Also i cannot be held responsible for any harm caused 
 * by this or any derived code.
 * Use it at your own risk.
 *
 */

#include "hamming.h"
#include "me.h"
#include "transmission.h"

void delay(short n)
{
	for ( ;n>0;n-- )
	{
//		_delay_us(1);
	}
}

void send_short(unsigned short t)
{
	set_b(!!(t&(0x8000)));
	set_b(!!(t&(0x4000)));
	set_b(!!(t&(0x2000)));
	set_b(!!(t&(0x1000)));
	set_b(!!(t&(0x0800)));
	set_b(!!(t&(0x0400)));
	set_b(!!(t&(0x0200)));
	set_b(!!(t&(0x0100)));
	set_b(!!(t&(0x0080)));
	set_b(!!(t&(0x0040)));
	set_b(!!(t&(0x0020)));
	set_b(!!(t&(0x0010)));
	set_b(!!(t&(0x0008)));
	set_b(!!(t&(0x0004)));
	set_b(!!(t&(0x0002)));
	set_b(!!(t&(0x0001)));
}

void send(const unsigned char *b, unsigned char N)
{
	unsigned char i;
	unsigned short tmp;
	send_short(0xFFFF);
	send_short(0x5555);
	set_b(0);
	set_b(0);
	set_b(0);
	set_b(0);
	set_b(0);
	set_b(0);
	set_b(1);
	set_b(0);
	tmp = me_encode('5');
	send_short(tmp);
	for ( i=0;i<N;i++ )
	{
		tmp = me_encode(b[i]);
		send_short(tmp);
	}
	set_b(1);
	set_b(0);
	send_short(0x0000);
}

//void print_bin(unsigned short l)
//{
//	char i;
//	for ( i=0;i<16;i++ )
//	{
//		printf("%d", !!(l&0x8000));
//		l<<=1;
//	}
//	printf(" ");
//}

// assume already in reveibe mode
void recv(unsigned char *b, unsigned char N)
{
//	union {
//		unsigned short l;
//		struct
//		{
//			unsigned char s[2];
//		};
//	} buf;
	unsigned short buf;
	unsigned char B;
	unsigned char K=0, M=0;
	unsigned char found = 0;
	buf = 0;

	while (1)
	{
		K = 0;
		M = 0;
		found = 0;
		buf = 0;
//set_led(0);
		wait_start();
		
		while ( K<N )
		{
			buf <<= 1;
			B = get_b(1);
			if ( B>1 ) break;
			buf |= B;
			M++;
			buf <<= 1;
			B = get_b(0);
			if ( B>1 ) break;
			buf |= B;
			M++;

			if ( M==16 )
			{
				if ( me_valid(buf) )
				{
					b[K] = me_decode16(buf);

					if ( !found && b[K]=='5' )
					{
//set_led(1);
						M = 0;
						K = 0;
						found = 1;
						continue;
					}
					if ( found )
					{
						K ++ ;
						M = 0;
					}
				}
				else
				{
					
	//				found = 0;
					break;
				}
			}
		}
		if ( K>=N )
			break;
	}
}


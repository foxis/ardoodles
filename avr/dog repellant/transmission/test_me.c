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

#include <stdio.h>
#include "me.h"
#include "hamming.h"

unsigned short print_bin(unsigned short t)
{
	int i;
	unsigned short b=0x8000;
	for ( i=0;i<16;i++ )
	{
		printf("%d", (int)((t&b)==b));
		b >>= 1;
	}
	printf("\n");
	return t;
}

int main(void)
{
	char text[1024] = "Hello World!";
	char output[1024] = "";
	char* p=text;
	unsigned short data[1024];
	int N=0, i=0;

	printf("%04x %04x %04x %04x\n",
		print_bin(me_encode(0)),
		print_bin(me_encode(1)),
		print_bin(me_encode(2)),
		print_bin(me_encode(0xFF)));

	printf("%d %d %d %d\n",	
		(int)me_valid(me_encode(0)),
		(int)me_valid(me_encode(1)),
		(int)me_valid(me_encode(2)),
		(int)me_valid(me_encode(0xFF)));

	printf("%d %d %d %d\n",	
		(int)me_decode16(me_encode(0)),
		(int)me_decode16(me_encode(1)),
		(int)me_decode16(me_encode(2)),
		(int)me_decode16(me_encode(0xFF)));


	while ( *p )
	{
		data[N] = me_encode(*p);
		printf("%c,%04X,%d,%02x,%02x,",*p,(int)data[i], (int)me_valid(data[i]), (int)*p, (int)me_decode16(data[i]));
		print_bin(data[i]);
		N++;
		p++;
	}
	printf("\n");

	for ( i=0;i<N;i++ )
	{
	//	printf("%d", (int)(text[i]==me_decode16(data[i])));
		printf("%c", me_decode16(data[i]));
	}
	text[i] = '\0';
	printf("\n");
//	hamming_decode(output, text, N*2);
//	for ( i = 0;i<N;i++ )
//	{
//		printf("%c", output[i]);
//	}

	return 0;
}


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
#include <stdlib.h>
#include "../transmission/transmission.h"

#define         sei()   __asm__ __volatile__ ("sei" ::)
#define         cli()   __asm__ __volatile__ ("cli" ::)

#define BUFLEN 512
unsigned char buffer[BUFLEN];
int ri=0, si=0;

void set_led(char l)
{
}

static unsigned char turn_on_sequence[] = "BS";

inline char get(void)
{
	if ( ri>=BUFLEN )
		exit (1);
	return buffer[ri++];
}

inline char wait_trans(int N)
{
	unsigned char t,t1,i;
	t = get();
	for (i=0;i<N;i++)
	{
		t1 = t;
		t = get();
		if ( t1^t )
		{
//			if ( counter<COUNTER_MIN_VAL );
//				return 3;
			return t;
		}
	}
	return 2;
}

void wait_start(void)
{
	unsigned short w;
	unsigned char t,t1;

	while (1)
	{
//		set_led(0);
		while (1)
		{
			t = get();
			if ( t )
			{
				// wait for preamble
				if ( wait_trans(8)==2 )
					break;
			}
		}
printf("FFFF %d\n", ri);
		// preamble detected, find header && synchronize
		while ( 1 )
		{
			t1 = t;
			t = get();
			if ( t^t1 )
			{
				w<<=1;
				w|= t;
			}

			// if preamble is found...
			if ( w==0x5555 )
			{
printf("preamble found %d\n", ri);
				wait_trans(8);
				// find a start transition
				if ( wait_trans(4*8)==1 )
				if ( wait_trans(8)==0 )
				{
//set_led(1);
					// skip until middle of the bit
printf("start found %d\n", ri);
					get();
					get();
					get();
					get();
					return ;
				}
				// search for new preamble...
				break;
			}
		}	
	}
		
}



char get_b(char trans)
{
	register unsigned char t;
	// read transition bit
	t = get();
printf("+%d ", ri);
	if ( trans )
	{
	// wait for mandatory transition
		if ( wait_trans(8)>1 )
			return 2;	// too early or too late
		else
		{
printf("!%d ", ri);
			return t;
		}
	}
	else
	{
get();
get();
get();
get();
		return t;
	}
}

void set_b(char b)
{
	buffer[si++] = b;
	buffer[si++] = b;
	buffer[si++] = b;
	buffer[si++] = b;
}

void set_dir(char dir)
{
}


int main(void)
{
	unsigned char in[3];
	unsigned char out[3] = "BS";
	int i;


	for ( i=0;i<64;i++ )
	{
		buffer[si++] = !!(rand()&2);
	}

	send(out, 2);

	for ( i=0;i<BUFLEN;i++ )
	{
		if ( i%32==0 )
			printf("\n %03d: ", i);
		if ( i%4==0 )
			printf(" ");
		printf("%d", (int)buffer[i]);
	}

	printf("\n\n");

	for ( i=0;i<2;i++ ) printf("%d", (int)out[i]);
	printf("\n");

	recv(in, 2);
	printf("received!\n");
	for ( i=0;i<2;i++ ) printf("%d", (int)in[i]);
	printf("\n");
}



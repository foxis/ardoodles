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
#include "transmission.h"
#include "hamming.h"

char bits[2048];
int ri, wi;


char get_b(void)
{
	return bits[(ri++)%2048];
}

void set_b(char b)
{
	bits[wi++] = !!b;
	printf("%d",(int)!!b);
}

void set_dir(char dir)
{
}

int main(void)
{
	char text[] = "Hello World!";
	unsigned char data[1024];
	unsigned char in[1024];
	int i;


	ri = wi = 0;

	for ( i=0;i<sizeof(text);i++ )
		set_b(!!(text[i]&8));
	for ( i=0;i<30;i++ )
		set_b(i&1);

	send(text, sizeof(text));

	recv(in, sizeof(text));

//	hamming_encode(data, text, sizeof(text));
//	hamming_decode(in, data, sizeof(text)*2);

	printf("\n\n");
	for ( i=0;i<sizeof(text);i++ )
		printf("%02x", (int)text[i]);
	printf("\n");
	for ( i=0;i<sizeof(text);i++ )
		printf("%02x", (unsigned int)in[i]);
	printf("\n");

	return 0;
}


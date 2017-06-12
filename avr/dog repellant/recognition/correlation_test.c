#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define PROGMEM
#define pgm_read_byte(a) (*(a)) 

#include "correlation.h"

int triuksmas(int i, int N)
{
	double r= rand()/(double)RAND_MAX;
//	if ( i<N )
//		return (r*.2)*255;

	return r+.8*(127+127*sin(i*6.28/2000.0));
} 

int main(void)
{
	int i;
	g_buffer_index = 0;
	 long sum_yy;

//	for ( i=0;i<BUFFER_LENGTH;i++ )
//	{
//		g_bark[i] = (unsigned char)(255*exp(-2*i/(double)BUFFER_LENGTH));
//	}

//	for ( i=0,sum_yy=0;i<BUFFER_LENGTH;i++ )
//	{
//		sum_yy += g_bark[i];
//		fprintf(stderr, "%d\n", (int)g_bark[i]);
//	}
//	fprintf(stderr, "-------sum %d\n", (int)sum_yy);

//	sum_Y = 0;
//	Y = 0;
//	sum_yy=0;
//	for ( i=0;i<BUFFER_LENGTH;i++ )
//	{
//		unsigned char y = g_bark[i];
//		sum_Y += y;
//		sum_yy += y*y;
//	}
//	sum_Y /= BUFFER_LENGTH;
//	Y = sum_yy/BUFFER_LENGTH - sum_Y*sum_Y;
//	fprintf(stderr, "%d, %d\n", (int)Y, (int)sum_Y);

	for (i=0;i<4024;i++ )
	{
		int tmp;
		
		tmp = triuksmas(i, 80);
tmp = i==100?255:0;
	
		buffer_put(tmp);

		fprintf(stdout, "%d\t  %d %d\n", i, (int)tmp, (int)g_correlation);
	}
}


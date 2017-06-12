
#define BUFFER_LENGTH 32

volatile unsigned char g_buffer[BUFFER_LENGTH]="";
volatile unsigned char g_buffer_index = 0;
volatile signed char	g_correlation = 0;
volatile unsigned short g_mean = 0;
volatile unsigned char g_mean_N = 0;
signed char g_bark[BUFFER_LENGTH] PROGMEM = 
{
-12, -7, 8, 20, -72, -21, 126, -21, -72, 20, 6, 20, 8, -7, -11,
};

/* fir.c - FIR filter in direct form */
#define M 16
void buffer_put(unsigned char x)
{
	unsigned char i;
	signed short y;         
	signed short h, w;

	g_buffer[g_buffer_index] = x;  

	for (y=0, i=0; i<M; i++)
	{
		register unsigned char j;
		j = (i+g_buffer_index+BUFFER_LENGTH-M+1)%BUFFER_LENGTH;
		h = (signed short)pgm_read_byte(g_bark+i);
		w = (unsigned short)g_buffer[j];
		y +=  (signed char)((unsigned char)((h * w)>>8));
	}
	g_buffer_index = (g_buffer_index+1)%BUFFER_LENGTH;

	if ( g_mean_N>32 )
	{
		g_correlation = g_mean/32;
	}
	g_mean_N ++ ;
	g_mean += (signed char)y;
}



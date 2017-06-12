/* 	
	Nokia 3310 LCD Demo. Codes are partially modified from Michel 
	Bavin's CCS C Codes for Nokia 3310 Graphical LCD 
	(LPH7779) routines version 3 from http://users.skynet.e/bk317494/

	This is a HiTech C demo code for the PCD8544 controller	based 
	Graphical LCD which is implemented on Nokia 3310 Cellphones.
*/

#include <pic.h>
#include <stdlib.h>
#include <math.h>
#include "delay.h"

#define SCK 	RD7					// Connections between PIC and LCD.
#define SDI 	RD6
#define D_C		RD5
#define _SCE 	RD4
#define _RES 	RD3

#define pi 3.14159					// Constant pi.
#define num_of_points 64			// Number of points to plot in a graph.

void initlcd(void);					// Initializes the LCD.
void writecom(char);				// Writes a command.
void writedata(char);				// Writes data to DDRAM to illuminate the pixels.
void clockdata(char);				// Clocks in data into the PCD8544 controller.
void clearram(void);				// Erase the DDRAM contents.
void cursorxy(char,char);			// Position cursor to x,y.
void putchar(char);					// Write one character.
void putstr(const char *);			// Writes a string of characters.
void plot1bar(char, char);			// Plot 1 bar. Pass value to plot and the maximum value to it.Variable type may be changed to char, float...etc.
void plotbars(char[], int, char, char, char);	// Plot an assembly of bars in one bargraph. Pass the same parameters as Plotline to it.
void plotline(char[], int, char, char, char);	// Plots 1 smooth(er) line. Pass the points to plot in the array, number of elements, maxvalue and x,y coordinates for location.

__CONFIG(PWRTDIS & DUNPROT & UNPROTECT & BORDIS & WDTDIS & LVPDIS & HS);

const char table[480] = {	0x00,0x00,0x00,0x00,0x00,	// 20 space		ASCII table for NOKIA LCD: 96 rows * 5 bytes= 480 bytes
							0x00,0x00,0x5f,0x00,0x00,	// 21 !			Note that this is the same set of codes for character you
							0x00,0x07,0x00,0x07,0x00,	// 22 "			would find on a HD44780 based character LCD. :)
							0x14,0x7f,0x14,0x7f,0x14,	// 23 #			Also, given the size of the LCD (84 pixels by 48 pixels),
							0x24,0x2a,0x7f,0x2a,0x12,	// 24 $			the maximum number of characters per row is only 14. :)
							0x23,0x13,0x08,0x64,0x62,	// 25 %
							0x36,0x49,0x55,0x22,0x50,	// 26 &
							0x00,0x05,0x03,0x00,0x00,	// 27 '
							0x00,0x1c,0x22,0x41,0x00,	// 28 (
							0x00,0x41,0x22,0x1c,0x00,	// 29 )
							0x14,0x08,0x3e,0x08,0x14,	// 2a *
							0x08,0x08,0x3e,0x08,0x08,	// 2b +
							0x00,0x50,0x30,0x00,0x00,	// 2c ,
							0x08,0x08,0x08,0x08,0x08,	// 2d -
							0x00,0x60,0x60,0x00,0x00,	// 2e .
							0x20,0x10,0x08,0x04,0x02,	// 2f /
							0x3e,0x51,0x49,0x45,0x3e,	// 30 0
							0x00,0x42,0x7f,0x40,0x00,	// 31 1
							0x42,0x61,0x51,0x49,0x46,	// 32 2
							0x21,0x41,0x45,0x4b,0x31,	// 33 3
							0x18,0x14,0x12,0x7f,0x10,	// 34 4
							0x27,0x45,0x45,0x45,0x39,	// 35 5
							0x3c,0x4a,0x49,0x49,0x30,	// 36 6
							0x01,0x71,0x09,0x05,0x03,	// 37 7
							0x36,0x49,0x49,0x49,0x36,	// 38 8
							0x06,0x49,0x49,0x29,0x1e,	// 39 9
							0x00,0x36,0x36,0x00,0x00,	// 3a :
							0x00,0x56,0x36,0x00,0x00,	// 3b ;
							0x08,0x14,0x22,0x41,0x00,	// 3c <
							0x14,0x14,0x14,0x14,0x14,	// 3d =
							0x00,0x41,0x22,0x14,0x08,	// 3e >
							0x02,0x01,0x51,0x09,0x06,	// 3f ?
							0x32,0x49,0x79,0x41,0x3e,	// 40 @
							0x7e,0x11,0x11,0x11,0x7e,	// 41 A
							0x7f,0x49,0x49,0x49,0x36,	// 42 B
							0x3e,0x41,0x41,0x41,0x22,	// 43 C
							0x7f,0x41,0x41,0x22,0x1c,	// 44 D
							0x7f,0x49,0x49,0x49,0x41,	// 45 E
							0x7f,0x09,0x09,0x09,0x01,	// 46 F
							0x3e,0x41,0x49,0x49,0x7a,	// 47 G
							0x7f,0x08,0x08,0x08,0x7f,	// 48 H
							0x00,0x41,0x7f,0x41,0x00,	// 49 I
							0x20,0x40,0x41,0x3f,0x01,	// 4a J
							0x7f,0x08,0x14,0x22,0x41,	// 4b K
							0x7f,0x40,0x40,0x40,0x40,	// 4c L
							0x7f,0x02,0x0c,0x02,0x7f,	// 4d M
							0x7f,0x04,0x08,0x10,0x7f,	// 4e N
							0x3e,0x41,0x41,0x41,0x3e,	// 4f O
							0x7f,0x09,0x09,0x09,0x06,	// 50 P
							0x3e,0x41,0x51,0x21,0x5e,	// 51 Q
							0x7f,0x09,0x19,0x29,0x46,	// 52 R
							0x46,0x49,0x49,0x49,0x31,	// 53 S
							0x01,0x01,0x7f,0x01,0x01,	// 54 T
							0x3f,0x40,0x40,0x40,0x3f,	// 55 U
							0x1f,0x20,0x40,0x20,0x1f,	// 56 V
							0x3f,0x40,0x38,0x40,0x3f,	// 57 W
							0x63,0x14,0x08,0x14,0x63,	// 58 X
							0x07,0x08,0x70,0x08,0x07,	// 59 Y
							0x61,0x51,0x49,0x45,0x43,	// 5a Z
							0x00,0x7f,0x41,0x41,0x00,	// 5b [
							0x02,0x04,0x08,0x10,0x20,	// 5c Yen Currency Sign
							0x00,0x41,0x41,0x7f,0x00,	// 5d ]
							0x04,0x02,0x01,0x02,0x04,	// 5e ^
							0x40,0x40,0x40,0x40,0x40,	// 5f _
							0x00,0x01,0x02,0x04,0x00,	// 60 `
							0x20,0x54,0x54,0x54,0x78,	// 61 a
							0x7f,0x48,0x44,0x44,0x38,	// 62 b
							0x38,0x44,0x44,0x44,0x20,	// 63 c
							0x38,0x44,0x44,0x48,0x7f,	// 64 d
							0x38,0x54,0x54,0x54,0x18,	// 65 e
							0x08,0x7e,0x09,0x01,0x02,	// 66 f
							0x0c,0x52,0x52,0x52,0x3e,	// 67 g
							0x7f,0x08,0x04,0x04,0x78,	// 68 h
							0x00,0x44,0x7d,0x40,0x00,	// 69 i
							0x20,0x40,0x44,0x3d,0x00,	// 6a j 
							0x7f,0x10,0x28,0x44,0x00,	// 6b k
							0x00,0x41,0x7f,0x40,0x00,	// 6c l
							0x7c,0x04,0x18,0x04,0x78,	// 6d m
							0x7c,0x08,0x04,0x04,0x78,	// 6e n
							0x38,0x44,0x44,0x44,0x38,	// 6f o
							0x7c,0x14,0x14,0x14,0x08,	// 70 p
							0x08,0x14,0x14,0x18,0x7c,	// 71 q
							0x7c,0x08,0x04,0x04,0x08,	// 72 r
							0x48,0x54,0x54,0x54,0x20,	// 73 s
							0x04,0x3f,0x44,0x40,0x20,	// 74 t
							0x3c,0x40,0x40,0x20,0x7c,	// 75 u
							0x1c,0x20,0x40,0x20,0x1c,	// 76 v
							0x3c,0x40,0x30,0x40,0x3c,	// 77 w
							0x44,0x28,0x10,0x28,0x44,	// 78 x
							0x0c,0x50,0x50,0x50,0x3c,	// 79 y
							0x44,0x64,0x54,0x4c,0x44,	// 7a z
							0x00,0x08,0x36,0x41,0x00,	// 7b <
							0x00,0x00,0x7f,0x00,0x00,	// 7c |
							0x00,0x41,0x36,0x08,0x00,	// 7d >
							0x10,0x08,0x08,0x10,0x08,	// 7e Right Arrow 	->
							0x78,0x46,0x41,0x46,0x78};	// 7f Left Arrow	<-

/* These are global variables accessible by all functions within the firmware. */
bank1 char plot_pt[num_of_points];		// Assign 64 bytes of RAM of Bank 1 for graphing usage.
bank2 char plot_pt2[num_of_points];		// Additional RAM for the use of plotline function.

void main(void)
{
	int i;

	TRISD = 0b00000111;				// All SCK, SDI, D_C, _SCE and _RES are output pins.
	initlcd();	
	putstr(" PCD-8544 LCD ");		// A test message.... Also a new year's greeting. :)
	cursorxy(0,1);					// Next line.
	putstr("Graphical Demo");

	for(;;)
	{
		for(i=0;i<num_of_points;i++)
		{
			plot_pt[i] = (char)(rand() % 256);			// Generate random numbers. Default is 0-255.
		}
		plotline(plot_pt, num_of_points, 255, 10, 2);	// Plot the smooth(er) line graph.
		plotbars(plot_pt, num_of_points, 255, 10, 4);	// Plot the bar graph version with the same points.

//		DelayS(5);	// Remark this line if you want to see some really blazing fast plotting. :)
	}
}

void initlcd(void)
{
	_RES = 1;		// Set _RES HIGH.
	_SCE = 1;		// Disable Chip.
	_RES = 0;		// Reset the LCD.
	DelayMs(100);	// Wait 100ms.
	_RES = 1;		// Awake LCD from RESET state.

	writecom(0x21);		// Activate Chip and H=1.
	writecom(0xC2);		// Set LCD Voltage to about 7V.
	writecom(0x13);		// Adjust voltage bias.
	writecom(0x20);		// Horizontal addressing and H=0.
	writecom(0x09);		// Activate all segments.
	clearram();			// Erase all pixel on the DDRAM.
	writecom(0x08);		// Blank the Display.
	writecom(0x0C);		// Display Normal.
	cursorxy(0,0);		// Cursor Home.
}

void writecom(char command_in)
{
	D_C = 0;				// Select Command register.
	_SCE = 0;				// Select Chip.
	clockdata(command_in);	// Clock in command bits.
	_SCE = 1;				// Deselect Chip.	
}

void writedata(char data_in)
{
	D_C = 1;				// Select Data register.
	_SCE = 0;				// Select Chip.
	clockdata(data_in);		// Clock in data bits.
	_SCE = 1;				// Deselect Chip.	
}

void clockdata(char bits_in)
{
	int bitcnt;
	for (bitcnt=8; bitcnt>0; bitcnt--)
		{
			SCK = 0;								// Set Clock Idle level LOW.
			if ((bits_in&0x80)==0x80) {SDI=1;}		// PCD8544 clocks in the MSb first.
			else {SDI=0;}
			SCK = 1;								// Data is clocked on the rising edge of SCK.
			bits_in=bits_in<<1;						// Logical shift data by 1 bit left.
		}
}

void clearram(void)
{
	int ddram;
	cursorxy(0,0);											// Cursor Home.
	for (ddram=504;ddram>0;ddram--)	{writedata(0x00);}		// 6*84 = 504 DDRAM addresses.
}
	

void cursorxy(char x, char y)
{
	writecom(0x40|(y&0x07));	// Y axis
	writecom(0x80|(x&0x7f));	// X axis
}

void putchar(char character)
{
	int tablept, count, char_column;

	if ((character<0x20)||(character>0x7f)) {return;}		// Exit function if character is not found.
	tablept = ((5*character) - 160);						// Point to the columns of the character in the table.

	for (count=5;count>0;count--)							// Draw the columns to print the character.
	{
		char_column = table[tablept];
		writedata(char_column);
		tablept++;
	}
	writedata(0x00);										// 1 pixel spacing per character.
}

void putstr(const char *s)
{
	while(*s) {putchar(*s++);}								// Points to one ASCII to be written one at a time.
}

void plot1bar(char value, char max_value)
{
	unsigned int plot_value;
	int i;
	char hibyte_value, lobyte_value;

	lobyte_value = 0;				// Initial values.
	hibyte_value = 0;
	plot_value = (int)(value*15/max_value);		// Computes how many points to plot. Since we are using 2 vertical
												// 8 bit blocks, we can have 1-16 points for a bar graph.
	if((plot_value>0)&&(plot_value<=7))			// Plots bar graph which are below 8 points.
	{
		for(i=plot_value;i>=0;i--)
		{
			hibyte_value=hibyte_value>>1;
			hibyte_value=hibyte_value+0x80;
		}

		lobyte_value=0x00;
	}

	else if((plot_value>7)&&(plot_value<=15))	// Plots bar graph which are above 8 points up to 15 points.
	{
		plot_value = plot_value - 7;
		
		for(i=plot_value;i>=0;i--)
		{
			lobyte_value=lobyte_value>>1;
			lobyte_value=lobyte_value+0x80;
		}

		hibyte_value = 0xFF;
	}

	else if(plot_value>15)
	{
		lobyte_value = 0xFF;					// If beyond 15 points than cap the points at the maximum of 15.
		hibyte_value = 0xFF;
	}
	
	else
	{
		lobyte_value = 0x00;					// If below 0 then cap it to 0(which is a unit of pixel on the bargraph).
		hibyte_value = 0x80;
	}
			
	writedata(lobyte_value);								// A 16 point plotting. Plots the higher 8 bits (which is the low byte)
	writedata(hibyte_value);								// and then the lower 8 bits (which is the high byte) of the graph.
}

void plotbars(char points2[], int num_points2, char max_value2, char x_pos2, char y_pos2)
{
	int i;
	
	writecom(0x22);					// Vertical addressing for graphs.

	for(i=0;i<num_points2;i++)
	{
		cursorxy(i+x_pos2, y_pos2);
		plot1bar(points2[i], max_value2);
	}

	writecom(0x20);					// Back to Horizontal Addresing.
}

void plotline(char points[], int num_points, char max_value, char x_pos, char y_pos)
{
	int i,j,diff;
	unsigned int hex_value, write_value;
	char hibyte_value, lobyte_value;

	writecom(0x22);			// Vertical addressing for graphs.

	for(i=0;i<num_points;i++)
	{
		plot_pt2[i] = (char)(points[i]*15/max_value);
	}

	for(i=0;i<num_points-1;i++)
	{
		hex_value = 0x8000 >> plot_pt2[i];
		write_value = hex_value;
		diff = plot_pt2[i] - plot_pt2[i+1];

		cursorxy(i+x_pos,y_pos);	// Location for plotting the graph.

		if(abs(diff)>=2)
		{
			if(diff>=2)
			{
				for(j=0;j<diff-1;j++)
				{
					write_value = write_value<<1;
					write_value = write_value + hex_value;
				}
			}
		
			else if(diff<=-2)
			{
				for(j=0;j<abs(diff)-1;j++)
				{
					write_value = write_value>>1;
					write_value = write_value + hex_value;
				}
			}
		}
		
		else
		{
			write_value = hex_value;
		}

		hibyte_value = (write_value & 0xFF00)>>8;
		lobyte_value = write_value & 0x00FF;

		writedata(lobyte_value);
		writedata(hibyte_value);
	}

	writecom(0x20);			// Back to Horizontal Addressing.
}

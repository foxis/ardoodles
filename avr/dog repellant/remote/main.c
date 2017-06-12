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

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../transmission/transmission.h"

#define         sei()   __asm__ __volatile__ ("sei" ::)
#define         cli()   __asm__ __volatile__ ("cli" ::)

#define BITRATE		9600L
#define BIT_DELAY_uS	( 1000000L / BITRATE )
#define HALF_BIT_DELAY_uS	( BIT_DELAY_uS/2 )

#define LED (1<<3)
#define REDBUTTON (1<<1)
#define BEEPOFF (1<<0)

#define TXRX (1<<2)
#define DATA (1<<5)

#define TCNT_RELOAD 0
#define TIMER_TICK	(1000000L*64L/F_CPU)

// least length of a bit
#define COUNTER_MIN_VAL	((BIT_DELAY_uS*2/TIMER_TICK))
// least length of preamble
#define COUNTER_VAL1 (COUNTER_MIN_VAL*2)

void set_led(char l)
{
	if ( l )
		PORTA |= LED;
	else
		PORTA &= ~LED;
}

static unsigned char turn_on_sequence[] = "BU";
static unsigned char turn_on_sequence1[] = "BS";
static unsigned char turn_on_sequence2[] = "BO";
volatile unsigned short counter = 0;
volatile unsigned char state=0;

void delay_us(unsigned short n)
{
	while (n--) _delay_us(1);
}

void check_keys(void)
{
	DDRA |= BEEPOFF;
	_delay_us(5);
	PORTA |= BEEPOFF;
	_delay_us(5);
	if ( (PINA&BEEPOFF)==0 )
	{
		state = 0;
	}
	else 
	{
		PORTA &= ~BEEPOFF;
		_delay_us(5);
		if ( (PINA&BEEPOFF)==BEEPOFF )
		{
			state = 1;
		}
		else
			state = 2;
	}

	PORTA &= ~BEEPOFF;
	DDRA &= ~BEEPOFF;
}

inline char get(void)
{
	register unsigned char t;
	char i;
	t = PINA;
	check_keys();
	if ( !(t&REDBUTTON) )
	{
		set_led(1);
		set_dir(DIR_SEND);
		for ( i=0;i<5;i++ )
		{
			if ( state==0 )
				send(turn_on_sequence, 2);
			else if ( state==1 )
				send(turn_on_sequence1, 2);
			else
				send(turn_on_sequence2, 2);
			delay_us(1000);
		}
		set_dir(DIR_RECV);
		set_led(0);
		return 0;
	}
	return !!(t&DATA);
}

inline char wait_trans(unsigned short val)
{
	unsigned char t,t1;
	t = get();
	counter =0 ;
	while (counter<val)
	{
		t1 = t;
		t = get();
		if ( t1^t )
		{
			return t;
		}
	}
	return 2;
}

void wait_start(void)
{
	unsigned short w=0;
	unsigned char t,t1;
	counter = 0;
	sei();

	while (1)
	{
		while (1)
		{
			t = get();
			if ( t )
			{
				// wait for preamble
				if ( wait_trans(COUNTER_VAL1)==2 )
					break;
			}
		}
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

				wait_trans(COUNTER_MIN_VAL*3);
				// find a start transition
				if ( wait_trans(COUNTER_MIN_VAL*28)==1 )
				if ( wait_trans(COUNTER_MIN_VAL*3)==0 )
				{
					// skip until middle of the bit
					delay_us(BIT_DELAY_uS-60);
					return ;
				}
				// search for new preamble...
				break;
			}
		}	
	}
		

	cli();
}


char get_b(char trans)
{
	register unsigned char t;
	// read transition bit
	t = get();
	if ( trans )
	{
	// wait for mandatory transition
		if ( wait_trans(COUNTER_MIN_VAL*16)>1 )
		{
//			set_led(1);
			return 2;	// too early or too late
		}
		else
		{
			delay_us(HALF_BIT_DELAY_uS-25);
			return t;
		}
	}
	else
	{
		delay_us(BIT_DELAY_uS-70);
		return t;
	}
}

void set_b(char b)
{
	if ( b )
		PORTA |= DATA;
	else
		PORTA &= ~DATA;
	delay_us(BIT_DELAY_uS+10);
}

void set_dir(char dir)
{
	if ( dir==DIR_SEND )
	{
		PORTA &= ~(TXRX|DATA);
		DDRA |= DATA;
	}
	else
	{
		PORTA |= TXRX;
		DDRA &= ~DATA;
	}
	delay_us(50000);
}


void beep(void)
{
	unsigned short i;
//	unsigned char t=1;
	PORTA |= LED;
	PORTA &= ~0xc0;
//	if ( (PINA&(BEEPOFF))==BEEPOFF )
//	{
//		t = 0;
//	}
//	else
//	{
//		PORTA |= 0x40;
//	}
//	for (i=0;i<1000;i++)
//	{
//		delay_us(200);
//		if ( t )
//			PORTA ^= 0xc0;
//		delay_us(200);
//		if ( t )
//			PORTA ^= 0xc0;
//	}
//	PORTA &= ~0xc0;
	PORTA |= 0x40;
	delay_us(500);
	PORTA ^= 0xc0;
	delay_us(500);
	PORTA &= ~0xc0;
	for (i=0;i<10;i++) delay_us(1000);
	PORTA |= 0x40;
	delay_us(500);
	PORTA ^= 0xc0;
	delay_us(500);
	PORTA &= ~0xc0;

	for (i=0;i<500;i++) delay_us(1000);
	PORTA &= ~(LED);
	for (i=0;i<500;i++) delay_us(1000);
}

void blink(void)
{
	set_led(1);
	delay_us(3535);
	set_led(0);
	delay_us(3535);
}

int main(void)
{
	unsigned char in[2];
	PORTA = 2;
	DDRA = (1<<6)|(1<<7)|(1<<2)|(LED)|4|1;
	PORTA = 2;
/*char t=0;	
int i;
while (1)
{
check_keys();

if ( state==0 )
{
set_led(1);
}
else
if ( state==1 )
{
set_led(t);
t^=1;
for (i=0;i<100;i++) delay_us(1000);
}
else
{
set_led(0);
}

}*/

        TCCR1A = 0;
        TCCR1B = 33;
        TCNT1 = TCNT_RELOAD;
	TIMSK |= _BV(TOIE1) | _BV(TOV1);

	set_dir(DIR_RECV);

	while (1)
	{
		recv(in, 2);
	//	blink();
//		if ( in[0]=='6' && in[1]=='6'  )
//			blink();
//	beep();
		if ( in[0] == turn_on_sequence[0]
		     && in[1]==turn_on_sequence[1] )
		{
//			beep();
			beep();
		}
	}  

}

ISR(TIMER1_OVF1_vect)
{
        counter ++;
        TCNT1 = TCNT_RELOAD;
}



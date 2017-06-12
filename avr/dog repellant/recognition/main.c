#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define         sei()   __asm__ __volatile__ ("sei" ::)
#define         cli()   __asm__ __volatile__ ("cli" ::)
#define GENERATOR (1<<7)
#define LED	(1<<6)
#define NOT_ON  (1<<5)
#define ULTRASONIC (1<<4)

#define N 128L
volatile unsigned short P=0;
volatile char I=0;


void delay_us(unsigned short n)
{
	while (n--) _delay_us(1);
}

void delay_ms(unsigned short n)
{
	while (n--)delay_us(1000);
}

int main(void)
{
	PORTA = 0;
	DDRA = (1<<7)|(1<<6);
	PORTA = GENERATOR;		// turn off the generator
	DDRB = 0;
	PORTB = 0;
//PORTA = 0;

	ADMUX = _BV(REFS1) | _BV(ADLAR);
	ADCSR = _BV(ADEN) | _BV(ADSC)| _BV(ADFR) | _BV(ADIE) | 6;

//	TIMSK |= _BV(TOIE1) | _BV(TOV1);

 	sei();

	while (1)
	{
		register char p;
		p = PINB;
		if ( I>N || (p&(1<<6)) )
		{
			if ( (P>64*N || (p&(1<<6))) && ((PINA&NOT_ON)==0) )
			{
				cli();
				PORTA |= LED;//|GENERATOR;
				PORTA &= ~GENERATOR;
				if ( (PINA&ULTRASONIC)==0 )
					delay_ms(5000);
				else
					delay_ms(2000);
				PORTA |= GENERATOR;
				delay_ms(500);
				PORTA &= ~LED;
				delay_ms(5000);
				sei();
			}
			I = 0;
			P = 0;
		}
	}

	return 0;
}

ISR(ADC_vect)
{
	register char tmp;
	tmp = ADCH;
	P += tmp;
	I ++;
//	if ( PORTA&GENERATOR==GENERATOR )
//	{
//		PORTA ^= LED;
//		buffer_put(tmp);
//	}
}



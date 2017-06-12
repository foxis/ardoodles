/*
 * MultiStepperController.c
 *
 * Created: 9/19/2015 17:47:30
 *  Author: FoxIS
 */ 


#include <avr/io.h>
#include <stdlib.h>

struct 
{
	unsigned char iterations[8];
	unsigned long current[8];
	unsigned long target[8];
	
	// need for speed
	unsigned short count[8];
	unsigned short threshold[8];
	
	// delay
	unsigned long delay[8];
	
	union
	{
		struct
		{
			#define DECLARE_STEPPER(id) unsigned char dir##id : 1; unsigned char clock##id : 1;
			#define DECLARE_STEPPER_1(id) unsigned char clock##id : 1; unsigned char dir##id : 1;
			DECLARE_STEPPER(3)
			DECLARE_STEPPER(2)
			DECLARE_STEPPER(1)
			DECLARE_STEPPER(0)
		} pins;
		unsigned char port;
	} bank1;
	union
	{
		struct
		{
			DECLARE_STEPPER(7)
			DECLARE_STEPPER(6)
			DECLARE_STEPPER(5)
			DECLARE_STEPPER(4)
		} pins;
		unsigned char port;
	} bank2;
} Motors;

void step()
{
	unsigned char i = 0;
	
	#define MOTORS_PROBE_ENDSTOP(id) ((PINB & ((unsigned char)0x01 << id)) != 0)
	#define MOTORS_SET_DIR(_bank_, id) Motors.bank##_bank_.pins.dir##id = Motors.current[id] < Motors.target[id];
	#define MOTORS_SET_CLOCK(_bank_, id, val) Motors.bank##_bank_.pins.clock##id = val && (Motors.current[id] != Motors.target[id]) && (Motors.count[id] == 0);
	
	for (i = 0; i < 8; i++)
	{
		Motors.count[i] = (Motors.count[i] + 1) % Motors.threshold[i];
		if (Motors.target[i] != 0)
			Motors.current[i] += (Motors.count[i] == 0 && Motors.current[i] != Motors.target[i]) ? (Motors.current[i] < Motors.target[i] ? 1 : -1) : 0;
		else if (MOTORS_PROBE_ENDSTOP(i))
			Motors.current[i] = 0;
	}
	
	MOTORS_SET_DIR(1, 0);	MOTORS_SET_DIR(1, 1);	MOTORS_SET_DIR(1, 2);	MOTORS_SET_DIR(1, 3);
	MOTORS_SET_DIR(2, 4);	MOTORS_SET_DIR(2, 5);	MOTORS_SET_DIR(2, 6);	MOTORS_SET_DIR(2, 7);

	MOTORS_SET_CLOCK(1, 0, 1);	MOTORS_SET_CLOCK(1, 1, 1);	MOTORS_SET_CLOCK(1, 2, 1);	MOTORS_SET_CLOCK(1, 3, 1);
	MOTORS_SET_CLOCK(2, 4, 1);	MOTORS_SET_CLOCK(2, 5, 1);	MOTORS_SET_CLOCK(2, 6, 1);	MOTORS_SET_CLOCK(2, 7, 1);
	
	PORTC = Motors.bank1.port;	PORTD = Motors.bank2.port;

	MOTORS_SET_CLOCK(1, 0, 0);	MOTORS_SET_CLOCK(1, 1, 0);	MOTORS_SET_CLOCK(1, 2, 0);	MOTORS_SET_CLOCK(1, 3, 0);
	MOTORS_SET_CLOCK(2, 4, 0);	MOTORS_SET_CLOCK(2, 5, 0);	MOTORS_SET_CLOCK(2, 6, 0);	MOTORS_SET_CLOCK(2, 7, 0);
	
	PORTC = Motors.bank1.port;	PORTD = Motors.bank2.port;
}

unsigned char readADC(unsigned char channel)
{
	ADMUX = (1 << REFS0) | (1 << ADLAR) | channel;
	ADCSRA |= (1<<ADSC);
	while (ADCSRA & (1<<ADSC));
	return ADCH;
}

int main(void)
{
	unsigned char i;
	unsigned char maxDelay;
	unsigned char maxSpeed;
	DDRC = 0xFF;
	DDRD = 0xFF;
	DDRB = 0x00;
	PORTB = 0xFF;
	PORTC = 0;
	PORTD = 0;
	
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

	for (i = 0; i < 8; i++)
	{
		Motors.iterations[i] = 0;
		Motors.current[i] = 10;
		Motors.target[i] = 0;
		Motors.count[i] = 0;
		Motors.threshold[i] = 8;
		Motors.delay[i] = 100;
	}

    while(1)
    {
        // Read ADC
		maxSpeed = readADC(0);
		//maxDelay = readADC(1);
		//maxSpeed = 10;
		// randomize
		for (i = 0; i < 8; i++)
		{
			Motors.threshold[i] = (maxSpeed >> 3) & 0x3f;//RND(10, 32);
			if (Motors.current[i] == Motors.target[i])
			{
				if (--(Motors.delay[i]) == 0)
				{
					unsigned long tmp = 0;
					
					#define RND(min, max) (long)(min + random() % (max - min))
					
					++(Motors.iterations[i]);
					Motors.delay[i] = RND(10000, 65530);
					
					if (Motors.iterations[i] > 7)
					{
						Motors.target[i] = 0;
						Motors.delay[i] = 5000;
						Motors.iterations[i] = 0;
					}
					else
					{
						do {
							tmp = RND(0, 97000);
						} while(abs(tmp - Motors.target[i]) < 5000 || abs(tmp - Motors.target[i]) > 50000);
						Motors.target[i] = tmp;
					}
				}
			}
		}
		
		step();  
    }
}

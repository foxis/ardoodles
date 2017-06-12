#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#if defined(ATMEGA16)
#include "../common/hsv.h"
#endif
#include "../common/me.h"
#include "../common/protocol.h"

/**
 *  statically defined device ID
 *
 */
static unsigned char ext_gs_address = 0x01;


void delay(int i)
{
    while (i--)
    {
        delay_us(1);
    }
}
unsigned char logtable[256] PROGMEM =
{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 
 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 
 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 
 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 
 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 19, 19, 20, 20, 20, 21, 21, 
 22, 22, 23, 23, 24, 24, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 
 32, 33, 33, 34, 35, 36, 36, 37, 38, 39, 40, 41, 41, 42, 43, 44, 45, 
 46, 47, 48, 49, 51, 52, 53, 54, 55, 56, 58, 59, 60, 62, 63, 64, 66, 
 67, 69, 70, 72, 73, 75, 77, 78, 80, 82, 84, 86, 87, 89, 91, 93, 95, 
 98, 100, 102, 104, 106, 109, 111, 114, 116, 119, 121, 124, 127, 130,
 132, 135, 138, 141, 144, 148, 151, 154, 158, 161, 165, 168, 172, 176, 
 180, 184, 188, 192, 196, 200, 205, 209, 214, 219, 223, 228, 233, 238, 244, 249, 255};


void init_rgb(void)
{

#if defined(ATMEGA16)
    DDRD = 0xFF;
    ICR1 = 0xFF;
    TCNT1 = 0;
    OCR1A = 0x09;
    OCR1B = 0x2F;
    TCCR1A = _BV(WGM10) | _BV(COM1A1) | _BV(COM1B1);
    TCCR1B = _BV(CS11) | _BV(WGM12);
    TCCR2 = _BV(WGM20)|_BV(WGM21)|_BV(COM21)|_BV(CS21);
#else
    DDRB = (1<<0)|(1<<1)|(1<<4);
    OCR0A = 0;
    OCR0B = 0;
    OCR1B = 0;
    OCR1C = 255;
    TCCR0A = _BV(COM0A1)|_BV(COM0B1)|_BV(WGM00)|_BV(WGM01);
    TCCR0B = _BV(CS00)|_BV(CS01);
    TCCR1 = _BV(COM1B1)|_BV(PWM1B)|_BV(CS12)|_BV(CS10)|_BV(CS11);
#endif
}

void set_rgb(unsigned char r, unsigned char g, unsigned char b)
{
#if defined(ATMEGA16)
    OCR1A = 255-pgm_read_char(&logtable[r]);
    OCR1B = 255-pgm_read_char(&logtable[g]);
    OCR2  = 255-pgm_read_char(&logtable[b]);
#else
    OCR0A = 255-pgm_read_char(&logtable[g]);
    OCR0B = 255-pgm_read_char(&logtable[r]);
    OCR1B = 255-pgm_read_char(&logtable[b]);
#endif
}

/**
 * databus
 *
 */
static volatile unsigned char gs_databuf[32];
static volatile unsigned char gs_datapw;
static volatile unsigned char gs_datapr;
static volatile unsigned char gs_datalen;

inline void putb(char b)
{
    unsigned char i, m;
    i = gs_dataw>>3;
    m = 1<<(gs_dataw&0x07);
    gs_databuf[i] = (gs_databuf[i]&(~m))| (m&b);
    gs_dataw++;
    gs_datalen++;
}

inline char getb(void)
{
    unsigned char i, m, t;
    i = gs_datar>>3;
    m = 1<<(gs_datar&0x07);
    t = (gs_databuf[i]&(~m));

    gs_datar++;
    gs_datalen--;

    return t;
}

inline char peekb(unsigned char _i)
{
    unsigned char i, m;
    _i += gs_datar;
    i = _i>>3;
    m = 1<<(_i&0x07);
    return gs_databuf[i]&(~m);
}

void init_databus(void)
{
    MCUCR |= (unsigned char)_BV(ISC00);
    GIMSK |= (unsigned char)_BV(INT0);
}

void databus_sleep(void)
{
}

void databus_interpret(void)
{
    if ( gs_length>=PACKET_ENC_NUM_BYTES )
    {
        packet_t p;
        unsigned short buf[PACKET_NUM_BYTES];

        buf[0] = (((unsigned short)peekb(0))<<8)|peekb(1);
        buf[1] = (((unsigned short)peekb(2))<<8)|peekb(3);
        buf[2] = (((unsigned short)peekb(4))<<8)|peekb(5);
        buf[3] = (((unsigned short)peekb(6))<<8)|peekb(7);
        buf[4] = (((unsigned short)peekb(8))<<8)|peekb(9);

        getb();
    }
}

ISR(INT0_vect)
{
    putb( PINB2 );
    GIFR &= (unsigned char)~_BV(INTF0);
}

/**
 * Main routine
 *
 */
void main(void)
{
#if defined(ATMEGA16)
    unsigned char r=0, g=0, b=0;
    unsigned short h=0;

    init_rgb();

    while (1)
    {
        PORTD ^= 1;

        hsv2rgb(h, 255, 255, &r, &g, &b);
        set_rgb(r, g, b);

        delay(10000);

        r++;
        r &= 0x7F;
        g += (g==0);
        g &= 0x7F;

        h++;
        h &= 0x1ff;
    }
#else
    init_rgb();
    init_databus();

    while (1)
    {
        databus_interpret();
        databus_sleep();
    }
#endif
}


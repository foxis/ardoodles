
#include <msp430.h>
#include "periph.h"
#include "filter.h"

#include "settings_7695.h"


//#define MONO


void pause (unsigned p);
unsigned char i2c_wr (unsigned char i2c_addr, unsigned char cmd, unsigned char data);
unsigned char ov7695_wr (unsigned short regaddr, unsigned char data);
unsigned char stmipi_wr (unsigned char regaddr, unsigned char data);
void c_puts (const char *str);
int fujitsu_cmd (const char* cmd);
int fujitsu_rd (char* data);
void ee_shift_out (unsigned char byte);

volatile unsigned long i;


char freply1 [8], freply2 [8], freply3 [8], freply4 [8];


void main (void)
{
    #ifdef PRG
        //P2DIR = BIT0;
        //FPGA_RST_LO;
        for (;;);
    #endif

    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    P1DIR = BIT4 | BIT3 | BIT1;

    P2DIR = BIT7 | BIT2 | BIT1 | BIT0;
    P2SEL = 0x40;

    P3DIR = BIT4 | BIT3 | BIT1 | BIT0;
    P3SEL = BIT5 | BIT4; // RXD TXD

    P4DIR  = BIT6 | BIT4 | BIT3 | BIT1;
    P4OUT &=~(BIT4 | BIT3);

    CMD_HI;
    EE_CS_HI;
    EE_SCK_LO;
    MP2TS_EN_LO;

    SCL_RLS;
    SDA_RLS;

    MIPI_RST_LO;
    TLV_RST_LO;
    CPU_RST_LO;
    FPGA_RST_LO;

    __bic_SR_register (OSCOFF);
    BCSCTL1 |= XTS;                 // HF mode
    BCSCTL3 = LFXT1S1 | LFXT1S0;    // External

    for (;;)
    {
        IFG1 &= ~OFIFG;
        for (i=0; i<250; i++);
        if ((IFG1 & OFIFG) == 0) break;
    }

    BCSCTL2 = SELM1 | SELS;  //  MCLK = XT2CLK; SMCLK = XT2CLK

    // Timer A
    TACTL = BIT9 | BIT7 | BIT6 | BIT5; // SMCLK/8

    // UART
    UCA0CTL0 = 0;
    UCA0CTL1 = BIT7; //(SMCLK)
    UCA0BR0 = 139;   // 115200 (SMCLK = 16 MHz)
    UCA0BR1 = 0;

    // Грузим Fujitsu ---------------------------------------------------------
    EE_CS_LO;
    ee_shift_out (0x04);  // WRDI
    EE_CS_HI;
    pause (us20);

    EE_CS_LO;
    ee_shift_out (0x03);  // READ 0x0
    ee_shift_out (0x00);
    ee_shift_out (0x00);
    ee_shift_out (0x00);

    CPU_RST_HI;
    pause (ms10);
    while (!FUJ_INT);
    pause (us20);

    P2SEL |= BIT1; // SMCLK

    for (i=0; i<30; i++) pause (ms25);

    EE_CS_HI;
    P2SEL &= ~BIT1; // SMCLK OFF
    // ------------------------------------------------------------------------

    // Грузим FPGA ------------------------------------------------------------
    EE_CS_LO;
    ee_shift_out (0x04);  // WRDI
    EE_CS_HI;
    pause (us20);

    EE_CS_LO;
    ee_shift_out (0x03);  // READ 0x180000
    ee_shift_out (0x18);
    ee_shift_out (0x00);
    ee_shift_out (0x00);

    FPGA_RST_HI;
    pause (ms10);

    P2SEL |= BIT1; // SMCLK

    for (i=0; i<3; i++) pause (ms25);
    //pause (ms10);
    //while (!FPGA_DONE);

    EE_CS_HI;
    P2SEL &= ~BIT1; // SMCLK OFF
    P2DIR &= ~BIT1; // EE_SO OFF
    // ------------------------------------------------------------------------

    // MIPI -------------------------------------------------------------------

    ov7695_wr (0x0103, 0x01); // RESET SENSOR
	ov7695_wr (0x0100, 0x00); // SLEEP
	pause (ms1);

    MIPI_RST_HI;
    pause (ms25);  pause (ms25);

    stmipi_wr (0x22, 0x10);
    stmipi_wr (0x02, 0x29);
    stmipi_wr (0x04, 0x1E);
    stmipi_wr (0x05, 0x03);
    stmipi_wr (0x06, 0x0F);
    stmipi_wr (0x14, 0x40);
    stmipi_wr (0x15, 0x40);
    stmipi_wr (0x17, 0x2b);
    stmipi_wr (0x18, 0x2b);

    // ------------------------------------------------------------------------

    // Камера -----------------------------------------------------------------

    for (i = 0; i < sizeof(ov7695_init_settings_array)/sizeof(ov7695_init_settings_array[0]); i++)
	{
	    ov7695_wr (ov7695_init_settings_array[i].reg_addr, ov7695_init_settings_array[i].reg_val);
	}

	// ------------------------------------------------------------------------

	// TLV --------------------------------------------------------------------
	TLV_RST_HI;
	pause (ms10);

    i2c_wr (0x30,  0, 0x00);    // Set register Page to 0
    i2c_wr (0x30,  1, 0x01);    // Initiate SW Reset
    i2c_wr (0x30, 18, 0x81);    // NADC = 1, divider powered on
    i2c_wr (0x30, 19, 0x82);    // MADC = 2, divider powered on
    i2c_wr (0x30, 61, 2);       // Program the processing block to be used

    i2c_wr (0x30,  0, 1);       // Set register Page to 1
    i2c_wr (0x30, 51, 0x08);    // MICBIAS = 2.0V
    i2c_wr (0x30, 59, 2*20);    // Left  Analog PGA Seeting
    i2c_wr (0x30, 60, 2*20);    // Right Analog PGA Seeting


    i2c_wr (0x30, 52, 0xFC);    // Left  ADC Input selection for Left  PGA = IN1L(P) as Single-Ended
    i2c_wr (0x30, 55, 0xFC);    // Right ADC Input selection for Right PGA = IN1R(M) as Single-Ended

    i2c_wr (0x30,  0, 4);       // Set register Page to 4

    // 1-order high-pass ---------------------------------------------
    i2c_wr (0x30,  8, 0x78);  //  1 * Gain        0.94211960761267499
    i2c_wr (0x30,  9, 0x97);
    i2c_wr (0x30, 10, 0x87);  // -1 * Gain
    i2c_wr (0x30, 11, 0x69);
    i2c_wr (0x30, 12, 0x71);  // -coeff           -0.88423921522534987
    i2c_wr (0x30, 13, 0x2E);

    i2c_wr (0x30, 72, 0x78);
    i2c_wr (0x30, 73, 0x97);
    i2c_wr (0x30, 74, 0x87);
    i2c_wr (0x30, 75, 0x69);
    i2c_wr (0x30, 76, 0x71);
    i2c_wr (0x30, 77, 0x2E);

    // 5 biquads low-pass -------------------------------------------
    for (i=0; i<50; i++) i2c_wr (0x30, 14 + i, filter [i]);
    for (i=0; i<50; i++) i2c_wr (0x30, 78 + i, filter [i]);


    i2c_wr (0x30,  0, 0x00);  // Set register Page to 0
    i2c_wr (0x30, 81, 0xC2);  // Power up ADC channel
    i2c_wr (0x30, 82, 0x00);  // UNMUTE
    // ------------------------------------------------------------------------


    fujitsu_cmd ("CHK");//  < 0) goto restart;

    fujitsu_cmd ("RST");//  < 0) goto restart;

    fujitsu_cmd ("DP");

    fujitsu_cmd ("ENC0");

    #ifdef MONO
        // Аудио: моно
        fujitsu_cmd ("WA01830");
        fujitsu_cmd ("WD0007");
    #endif

    //fujitsu_cmd ("WA01546");  // VIN_MAX_HCOUNT
    //fujitsu_cmd ("WD0614");

    //fujitsu_cmd ("WA01548");  // VIN_MAX_VCOUNT
    //fujitsu_cmd ("WD01E2");

    fujitsu_cmd ("WA0154A");  // VIN_H_VALID_POS
    fujitsu_cmd ("WD0117");

    fujitsu_cmd ("WA0154C");  // VIN_TOP_VALID_LINE
    fujitsu_cmd ("WD000A");

    // SCALING --------------------------------------------
    fujitsu_cmd ("WA01520"); // SCALE_FLAG & V_SCALE_FORMAT
    fujitsu_cmd ("WD8005");  //

    fujitsu_cmd ("WA01522"); // SCALE_H_START_POS_SOURCE
    fujitsu_cmd ("WD0000");  //

    fujitsu_cmd ("WA01524"); // SCALE_V_START_POS_SOURCE
    fujitsu_cmd ("WD0000");  //

    fujitsu_cmd ("WA01526"); // SCALE_H_SIZE_SOURCE
    fujitsu_cmd ("WD0280");  // 640

    fujitsu_cmd ("WA01528"); // SCALE_V_SIZE_SOURCE
    fujitsu_cmd ("WD01E0");  // 480

    fujitsu_cmd ("WA0152E"); // SCALE_H_SIZE_DESTINATE
    fujitsu_cmd ("WD02D0");  // 720

    fujitsu_cmd ("WA01530"); // SCALE_V_SIZE_DESTINATE
    fujitsu_cmd ("WD0240");  // 576

    fujitsu_cmd ("WA015A2");  // V_FRAME_WIDTH_IN_MBS
    fujitsu_cmd ("WD002D");

    fujitsu_cmd ("WA015A4");  // V_FRAME_HEIGHT_IN_MBS
    fujitsu_cmd ("WD0024");

    fujitsu_cmd ("WA015AA"); // V_TIME_SCALE_L
    fujitsu_cmd ("WD0032");
    //-----------------------------------------------------

    fujitsu_cmd ("WA01406");
    //fujitsu_cmd ("WD01F4"); // ~750 kbps
    //fujitsu_cmd ("WD02EE"); // ~1000 kbps
    //fujitsu_cmd ("WD06D6");   // ~2000 kbps
    //fujitsu_cmd ("WD08CA");   // ~2500 kbps
    fujitsu_cmd ("WD0A50");   // ~3000 kbps
    //fujitsu_cmd ("WD1770");   // ~6500 kbps

    fujitsu_cmd ("ENC0");// < 0) goto restart;

    fujitsu_cmd ("EST");//  < 0) goto restart;

    MP2TS_EN_HI;

    for (i=0; i<40; i++) pause (ms25);

    for (;;)
    {/*
        if (FPGA_MISO)
        {
            ov7695_wr (0x12, 0x80); // RESET SENSOR
        	ov7695_wr (0x0E, 0x08); // SLEEP
        	MIPI_RST_LO;
        	pause (ms10);

            MIPI_RST_HI;
            pause (ms25);  pause (ms25);

            stmipi_wr (0x22, 0x10);
            stmipi_wr (0x02, 0x29);
            stmipi_wr (0x04, 0x1E);
            stmipi_wr (0x05, 0x03);
            stmipi_wr (0x06, 0x0F);
            stmipi_wr (0x14, 0x40);
            stmipi_wr (0x15, 0x40);
            stmipi_wr (0x17, 0x2b);
            stmipi_wr (0x18, 0x2b);

            for (i = 0; i < sizeof(ov7695_init_settings_array)/sizeof(ov7695_init_settings_array[0]); i++)
        	{
        	    ov7695_wr (ov7695_init_settings_array[i].reg_addr, ov7695_init_settings_array[i].reg_val);
        	}

            for (i=0; i<4; i++) pause (ms25);
        }*/
    }

}


void pause (unsigned p)
{
    TACTL |= BIT2;
    while (TAR < p);
}


int c_putc (int ch)
{
    UCA0TXBUF = ch;
    while (!(IFG2 & UCA0TXIFG));
    return ch;
}


void c_puts (const char *str)
{
    while (*str) c_putc (*(str++));
}

int fujitsu_cmd (const char* cmd)
{
        unsigned char ch[2] = {0,0};

    ch [0] = UCA0RXBUF;

    c_puts (cmd);

    while (!(IFG2 & UCA0RXIFG));
    ch [0] = UCA0RXBUF;

    while (!(IFG2 & UCA0RXIFG));
    ch [1] = UCA0RXBUF;

    if (ch [0] == 'O'  &&  ch [1] == 'K') return 0;
    else                                  return -1;
}


int fujitsu_rd (char* data)
{
        unsigned i;

    for (i=0; i<5; i++)
    {
        while (!(IFG2 & UCA0RXIFG));
        data [i] = UCA0RXBUF;
    }

    return 0;
}


void ee_shift_out (unsigned char byte)
{
        char bit;

    for (bit=0; bit < 8; bit ++)
    {
        if (byte & 0x80) EE_SI_HI;
        else             EE_SI_LO;

        EE_SCK_HI;
        EE_SCK_LO;

        byte = byte << 1;
    }
}

int i2c_in (char devaddr, unsigned char *data, int len, unsigned char dly)
{
        int byte, bit, shift, ack = 0;

	// Ia?aeuiia ninoiyiea
	SCL_RLS;
	SDA_RLS;
	pause (dly);

	// START
	SDA_DRV;
	pause (dly);

	SCL_DRV;
	pause (dly);

	// ADDR
	for (bit=0;  bit<8;  bit++)
	{
        if (devaddr & 0x80) SDA_RLS;
        else                SDA_DRV;
        pause (dly);

        SCL_RLS;
        pause (dly);

        while (!SCL);
        pause (dly);

        SCL_DRV;
        pause (dly);

        devaddr <<= 1;
	}

	// ACK?
	SDA_RLS;
	pause (dly);

	SCL_RLS;
    pause (dly);

    while (!SCL);
    pause (dly);

    if (SDA) ack = -1;

    SCL_DRV;
    pause (dly);

    if (ack == -1) goto stop;

    // DATA RD
	for (byte=0;  byte<len;  byte++)
	{
    	for (bit=0;  bit<8;  bit++)
    	{
	        SCL_RLS;
	        pause (dly);

	        while (!SCL);
	        pause (dly);

	        shift <<= 1;

	        if (SDA) shift |=  1;
	        else     shift &= ~1;

	        SCL_DRV;
	        pause (dly);
    	}

    	data [byte] = shift;

    	// ACK!
    	SDA_RLS;
    	pause (dly);

    	SCL_RLS;
        pause (dly);

        while (!SCL);
        pause (dly);

        SCL_DRV;
        pause (dly);
	}

stop:
	// STOP
	SDA_DRV;
    pause (dly);

    SCL_RLS;
    pause (dly);

    while (!SCL);
    pause (dly);

    SDA_RLS;

	return ack;
}


int i2c_out (char devaddr, char *data, int len, unsigned char dly)
{
		int byte, bit, shift, ack = 0;

    len ++;

    // Ia?aeuiia ninoiyiea
	SCL_RLS;
	SDA_RLS;
	pause (dly);

	// START
	SDA_DRV;
	pause (dly);

	SCL_DRV;
	pause (dly);

	for (byte=0;  byte<len;  byte++)
	{
	    if (byte) shift = data [byte - 1];
	    else      shift = devaddr;

    	for (bit=0;  bit<8;  bit++)
    	{
	        if (shift & 0x80) SDA_RLS;
	        else              SDA_DRV;
	        pause (dly);

	        SCL_RLS;
	        pause (dly);

	        while (!SCL);
	        pause (dly);

	        SCL_DRV;
	        pause (dly);

	        shift <<= 1;
    	}

    	// ACK?
    	SDA_RLS;
    	pause (dly);

    	SCL_RLS;
        pause (dly);

        while (!SCL);
        pause (dly);

        if (SDA) ack = -1;

        SCL_DRV;
        pause (dly);

        if (ack == -1) break;
	}

	// STOP
	SDA_DRV;
    pause (dly);

    SCL_RLS;
    pause (dly);

    while (!SCL);
    pause (dly);

    SDA_RLS;

	return ack;
}


unsigned char stmipi_wr (unsigned char regaddr, unsigned char data)
{
        char buf [3];

    buf [0] = 0;
    buf [1] = regaddr;
    buf [2] = data;

    if (i2c_out (40, buf, 3, I2C_DLY_MIPI) != 0) return -1;

    return 0;
}


unsigned char stmipi_rd (unsigned char regaddr, unsigned char *data)
{
        char buf [2];

    buf [0] = 0;
    buf [1] = regaddr;

    if (i2c_out (40, buf, 2, I2C_DLY_MIPI) != 0) return -1;

    return i2c_in (41, data, 1, I2C_DLY_MIPI);
}


unsigned char i2c_wr (unsigned char devaddr, unsigned char regaddr, unsigned char data)
{
        char buf [2];

    buf [0] = regaddr;
    buf [1] = data;

	return i2c_out (devaddr, buf, 2, I2C_DLY_TLV);
}


unsigned char ov7695_wr (unsigned short regaddr, unsigned char data)
{
        char buf [2];

    buf [0] = regaddr;
    buf [1] = data;

    if (i2c_out (0x78, buf, 2, I2C_DLY_CAM) != 0) return -1;

    return 0;
}

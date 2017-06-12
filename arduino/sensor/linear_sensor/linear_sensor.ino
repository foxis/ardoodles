#include <digitalWriteFast.h>

#define NumPixels 128
#define SyncPin 3
#define ClockPin 2
#define DataPin A0
#define FlashPin 4
#define MaxExposure 100
#define BaudRate 19200
#define TestPin 9
// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define ERROR_OK 0
#define ERROR_LED 1
#define ERROR_CODE 2
#define ERROR_PANIC 0xff

#define PIN_RESET 12  // LCD RST .... Pin 1
                     // LCD Gnd .... Pin 2
#define PIN_SCE   11  // LCD CS  .... Pin 3
#define PIN_SCLK 8  // LCD SPIClk . Pin 4
#define PIN_DC    10  // LCD Dat/Com. Pin 5
#define PIN_SDIN  9  // LCD SPIDat . Pin 6
                     // LCD Vlcd ... Pin 7
                     // LCD Vcc .... Pin 8

#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48


byte _flash = 1;
unsigned short data[NumPixels];
byte error = 0;
unsigned short mCode = 0;
unsigned short cInterpolator = 0;
uint32_t lPosition = 0;

static const byte ASCII[][5] =
{
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20  
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ←
,{0x00, 0x06, 0x09, 0x09, 0x06} // 7f →
};


void setup() {
  unsigned short _min, _max, m = 0;
  int i;
  
  // put your setup code here, to run once:
  pinMode(ClockPin, OUTPUT);
  pinMode(SyncPin, OUTPUT);
  pinMode(FlashPin, OUTPUT);
  
  // debugging
  Serial.begin(BaudRate);
  Serial.println("M-Code reader v0.1a demonstration");

  // SSI

  // setup ccd array
  digitalWrite(ClockPin, HIGH);
  digitalWrite(SyncPin, LOW);
  digitalWrite(FlashPin, LOW);

  // diagnostic pin
  pinMode(13, OUTPUT);

  pinModeFast(TestPin, OUTPUT);

  // analog read setup
  // set prescale to 16
  sbi(ADCSRA, ADPS2) ;
  cbi(ADCSRA, ADPS1) ;
  cbi(ADCSRA, ADPS0) ;

  // calibrate strobe
  for (i = 1; i < MaxExposure; i++)
  {
    read_array(i);
  
    _min = MIN(data, 128);
    _max = MAX(data, 128);
    if (_max - _min > m && _max < 983)
    {
      m = _max - _min;
      _flash = i;
    }
  }
 
  check_error(_min, _max, m); 

  if (error == ERROR_OK)
    process_code(data + 32, 64, (_max - _min) >> 1);

  LcdInit();
}

void check_error(unsigned short _min, unsigned short _max, unsigned short m)
{
  error = ERROR_OK;
  
  if (_min >= _max)
  {
    error = ERROR_PANIC;
  }
  
  if (_min > 255 || _max < (1024-255))
  {
    error |= ERROR_LED;
  }
  
  if (m < 512)
  {
    error |= ERROR_CODE;
  }
}

unsigned short MIN(unsigned short * data, byte len)
{
  unsigned short m = 65000;
  while (len)
  {
    m = min(m, *data);
    data ++;
    len --;
  }
  return m;
}
unsigned short MAX(unsigned short * data, byte len)
{
  unsigned short m = 0;
  while (len)
  {
    m = max(m, *data);
    data ++;
    len --;
  }
  return m;
}

void _delay(unsigned int us)
{
	__asm__ __volatile__ (
		"1: sbiw %0,1" "\n\t" // 2 cycles
		"brne 1b" : "=w" (us) : "0" (us) // 2 cycles
	); 
}

void read_array(byte flash)
{
  digitalWriteFast(SyncPin, HIGH);
  digitalWriteFast(ClockPin, LOW);
  digitalWriteFast(ClockPin, HIGH);
  digitalWriteFast(SyncPin, LOW);
  digitalWriteFast(ClockPin, LOW);
  for (byte j = 0; j < NumPixels; j++)
  {
    digitalWriteFast(ClockPin, HIGH);
    digitalWriteFast(ClockPin, LOW);
  }
  noInterrupts();
  digitalWriteFast(FlashPin, HIGH);
  _delay(flash);
  digitalWriteFast(FlashPin, LOW);
  interrupts();
  
  digitalWriteFast(SyncPin, HIGH);
  digitalWriteFast(ClockPin, LOW);
  digitalWriteFast(ClockPin, HIGH);
  digitalWriteFast(SyncPin, LOW);
  digitalWriteFast(ClockPin, LOW);
  for (byte j = 0; j < NumPixels; j++)
  {
    data[j] = analogRead(DataPin);
    digitalWriteFast(ClockPin, HIGH);
    digitalWriteFast(ClockPin, LOW);
  }
  digitalWriteFast(ClockPin, HIGH);
}

const float um[64] = 
  { 0.0, 63.5, 127.0, 190.5, 254.0, 317.5, 381.0, 
    444.5, 508.0, 571.5, 635.0, 698.5, 762.0, 825.5, 
    889.0, 952.5, 1016.0, 1079.5, 1143.0, 1206.5, 
    1270.0, 1333.5, 1397.0, 1460.5, 1524.0, 1587.5, 
    1651.0, 1714.5, 1778.0, 1841.5, 1905.0, 1968.5, 
    2032.0, 2095.5, 2159.0, 2222.5, 2286.0, 2349.5, 
    2413.0, 2476.5, 2540.0, 2603.5, 2667.0, 2730.5, 
    2794.0, 2857.5, 2921.0, 2984.5, 3048.0, 3111.5, 
    3175.0, 3238.5, 3302.0, 3365.5, 3429.0, 3492.5, 
    3556.0, 3619.5, 3683.0, 3746.5, 3810.0, 3873.5, 
    3937.0, 4000.5 
 };
float edge[64] = {0,};
byte bits[64] = {0,};
byte code[64] = {0,};

byte read_bits(const byte *bits, byte * code, byte * num_bits, byte * num_code)
{
  byte first_bit = *bits;
  byte nb = 0, nc = 0;
  
  nb = 0;
  while (*bits == first_bit)
  {
    nb++;
    bits ++;
  }
  
  if (first_bit)
    *num_code = (byte)(nb / 2.5);
  else
    *num_code = (byte)(nb / 2.0);
    
  *num_bits = nb;
  return first_bit;
}

uint32_t process_code(unsigned short * data, byte len, unsigned short mid)
{
  // 12 * 3
  // pixel 56.4um
  // pitch 63.5um
  // gap = (63.5 - 56.4) / 2 = 3.55
  float pos;
  byte i, first_i = 0;
  unsigned short d, d1;
  byte *c, *b, nc;

  mid -= mid /4;

  for (i = 1; i < len - 2; i++)
  {
    unsigned short A = min(data[i + 1], data[i - 1]);
    unsigned short B = max(data[i + 1], data[i - 1]);
    unsigned short C = min(data[i + 1], data[i + 2]);
    unsigned short D = max(data[i + 1], data[i + 2]);
    d = B - A;
    d1 = D - C;
    if (d > mid && data[i] > A && d1 < mid)
    {
      bits[i] = data[i - 1] < data[i + 1];
      edge[i] = (data[i] - A) / (float)d;
    }
    else
    {
      bits[i] = bits[i - 1];
      edge[i] = 0.0;
    }
  }

  for (i = 0; i < len - 1; i++)
  {
    if (edge[i] > edge[i + 1])
      edge[i + 1] = 0;
    else if (edge[i] < edge[i + 1])
      edge[i] = 0;
  }

  // find nearest code starting boundary
  for (i = 0; i < len; i++)
  {
      if (bits[i])
      {
        first_i = i;
        pos = um[i];
        break;
      }
  }
//  return first_i;
  // edge correction
  // TODO: average all available edges
  // TODO: take into account the gap
  pos += 3.55 + edge[first_i] * 56.4;
  
  // read the code
  nc = 0;
  c = code;
  b = bits+first_i;
  while (nc < 12)
  {
    byte _nb, _nc;
    byte f = read_bits(b, c, &_nb, &_nc);
    for (i = 0; i < _nc; i++)
      c[i] = f;
    c += _nc;
    b += _nb;
    nc += _nc;
  }
  
  // transform the code to position
  
  return (uint32_t)(pos * 100);
}

void loop()
{
/** /  digitalWrite(TestPin, LOW); _delay(21);
  digitalWrite(TestPin, HIGH); _delay(1);
  digitalWrite(TestPin, LOW); _delay(21);
/**/
/**/
  int i;
  unsigned short _min, _max;
  byte fir[NumPixels];
    
  read_array(_flash);

  _min = MIN(data+32, 64);
  _max = MAX(data+32, 64);  
  uint32_t pos;
      
  pos = process_code(data + 32, 64, (_max - _min) >> 1);
  
  for (i = 0; i < NumPixels; i++)
  {
    fir[i] = map(data[i], 0, 1024, 0, 255);
  }

  for (i = 0; i < NumPixels; i++)
  {
    Serial.print(fir[i] >> 4, HEX);
    Serial.print(fir[i] & 0xF, HEX);
  }
    
  Serial.println("");
  
  check_error(_min, _max, _max - _min);

  if (error & ERROR_CODE)
  {
    digitalWrite(13, digitalRead(13)?LOW:HIGH);
  }
  else if (error & ERROR_LED)
  {
    digitalWrite(13, HIGH);
  }
  
/**/
  {
     static char buf[20] = "";
     int i;
     unsigned short tmp[64];
     sprintf(buf, "%ld.%d   ", pos/100, pos%100); 
     gotoXY(0, 5);
     LcdString(buf);
     
     
     for (i = 0; i < 64; i++)
     {
        tmp[i] = 0x8000 >> map(fir[i + 32], 0, 255, 0, 15);
     }
     gotoXY(0,0);
     for (i = 0; i < 64; i++)
     {
        LcdWrite(LCD_D, tmp[i] & 0xFF);
     } 
     gotoXY(0,1);
     for (i = 0; i < 64; i++)
     {
        LcdWrite(LCD_D, (tmp[i] & 0xFF00) >> 8);
     }

     for (i = 0; i < 64; i++)
     {
        tmp[i] = 0x8000 >> ((byte)(edge[i] * 15));
     }
     gotoXY(0,2);
     for (i = 0; i < 64; i++)
     {
        LcdWrite(LCD_D, tmp[i] & 0xFF);
     } 
     gotoXY(0,3);
     for (i = 0; i < 64; i++)
     {
        LcdWrite(LCD_D, (tmp[i] & 0xFF00) >> 8);
     }

     for (i = 0; i < 64; i++)
     {
        tmp[i] = 0x20 >> ((byte)(bits[i] * 4));
     }
//     gotoXY(0,4);
//     for (i = 0; i < 64; i++)
//     {
//        LcdWrite(LCD_D, tmp[i] & 0xFF);
//     } 
     gotoXY(0,4);
     for (i = 0; i < 12; i++)
     {
        LcdCharacter(code[i] ? '1' : '0');
     } 
  }
}


void LcdCharacter(char character)
{
  LcdWrite(LCD_D, 0x00);
  for (int index = 0; index < 5; index++)
  {
    LcdWrite(LCD_D, ASCII[character - 0x20][index]);
  }
  LcdWrite(LCD_D, 0x00);
}

void LcdClear(void)
{
  gotoXY(0, 0);
  for (int index = 0; index <= 84*7+5; index++)
    LcdWrite(LCD_D, 0x00);
  gotoXY(0, 0);
}

void LcdInit(void)
{
  pinMode(PIN_SCE,   OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC,    OUTPUT);
  pinMode(PIN_SDIN,  OUTPUT);
  pinMode(PIN_SCLK,  OUTPUT);

  digitalWrite(PIN_DC, HIGH);
  digitalWrite(PIN_SCE, HIGH);
  digitalWrite(PIN_RESET, LOW);
  delay(1);
  digitalWrite(PIN_RESET, HIGH);

  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xB1 );  // Set LCD Vop (Contrast). //B1
  LcdWrite(LCD_C, 0x06 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x20);
  LcdWrite(LCD_C, 0x0C);
//  LcdWrite(LCD_C, 0x09);
//  LcdWrite(LCD_C, 0x08);
//  LcdWrite(LCD_C, 0x0C);
  LcdClear();
}

void LcdString(char *characters)
{
  while (*characters)
  {
    LcdCharacter(*characters++);
  }
}

void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

// gotoXY routine to position cursor 
// x - range: 0 to 84
// y - range: 0 to 5

void gotoXY(byte x, byte y)
{
  LcdWrite( 0, 0x80 | x);  // Column.
  LcdWrite( 0, 0x40 | y);  // Row.  
}





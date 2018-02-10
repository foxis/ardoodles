#include <FreqCount.h>
#include <Arduino.h>
#include <U8g2lib.h>

//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED

#define NUM 32
#define SPAN 128
long readings[NUM] = {0,};

#define _RB(b) ((unsigned char)(((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16))

static const unsigned char PROGMEM battery_bmp0[] =
{
  B11111111,B11111000,
  B10000000,B00001000,
  B10000000,B00001110,
  B10000000,B00001110,
  B10000000,B00001110,
  B10000000,B00001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp1[] =
{
  B11111111,B11111000,
  B11100000,B00001000,
  B11100000,B00001110,
  B11100000,B00001110,
  B11100000,B00001110,
  B11100000,B00001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp2[] =
{
  B11111111,B11111000,
  B11101100,B00001000,
  B11101100,B00001110,
  B11101100,B00001110,
  B11101100,B00001110,
  B11101100,B00001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp3[] =
{
  B11111111,B11111000,
  B11101101,B10001000,
  B11101101,B10001110,
  B11101101,B10001110,
  B11101101,B10001110,
  B11101101,B10001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp4[] =
{
  B11111111,B11111000,
  B11101101,B10111000,
  B11101101,B10111110,
  B11101101,B10111110,
  B11101101,B10111110,
  B11101101,B10111000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp5[] =
{
  B11111111,B11111000,
  B11111110,B01111000,
  B11111100,B11111110,
  B10000000,B11111110,
  B11111100,B11111110,
  B11111110,B01111000,
  B11111111,B11111000,
};

void setup(void) {
  u8g2.begin();
  FreqCount.begin(500);  
  //Serial.begin(115200);

  while (!FreqCount.available()) ;
  long f = FreqCount.read();
  for (byte i = 0; i < NUM; i++)
    readings[i] = f;
}

void show_battery()
{
  int vcc = (analogRead(A0) / 1.024);
  //Serial.println(vcc);
  register byte i = 0;
  if(vcc > 290*2) i++;
  if(vcc > 320*2) i++;
  if(vcc > 350*2) i++;
  if(vcc > 380*2) i++;
  if(vcc > 410*2) i++;

  switch (i)
  {
    case 0: u8g2.drawBitmapP(0, 0, 16, 7, battery_bmp0); break;
    case 1: u8g2.drawBitmapP(0, 0, 16, 7, battery_bmp1); break;
    case 2: u8g2.drawBitmapP(0, 0, 16, 7, battery_bmp2); break;
    case 3: u8g2.drawBitmapP(0, 0, 16, 7, battery_bmp3); break;
    case 4: u8g2.drawBitmapP(0, 0, 16, 7, battery_bmp4); break;
    case 5: u8g2.drawBitmapP(0, 0, 16, 7, battery_bmp5); break;
  }
}

void loop() {
  float scale;
  long mi = 2147483647L, ma = 0;
  if (FreqCount.available()) {
    for (byte i = 0; i < NUM - 1; i++)
      readings[i] = readings[i + 1];
    readings[NUM - 1] = FreqCount.read();

    for (byte i = 0; i < NUM; i ++)
    {
      long reading = readings[NUM - i - 1];
      if (reading < mi)
        mi = reading;
      if (reading > ma)
        ma = reading;
    }

    //Serial.println(readings[NUM - 1]);
    //Serial.print(' ');
    //Serial.print(mi);
    //Serial.print(' ');
    //Serial.print(ma);
    //Serial.print(' ');
  
      long mid = (ma + mi) / 2;
    //Serial.println(mid);
    if (ma - mi < SPAN / 2)
    {
      mi = mid - SPAN / 4;
      ma = mid + SPAN / 4;
    }
  
    scale = SPAN / (float)(ma - mi);

    u8g2.firstPage();
      show_battery();
    do {
      for (byte i = 0; i < NUM - 1; i++)
        u8g2.drawLine(min(SPAN-1, max(0, SPAN - (readings[i] - mi) * scale)), i,
            min(SPAN-1, max(0, SPAN - (readings[i + 1] - mi) * scale)), i + 1);
    } while (u8g2.nextPage());
  }
}


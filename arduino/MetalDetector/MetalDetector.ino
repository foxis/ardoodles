#include <FreqCount.h>
#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED

#define NUM 32
#define SPAN 128
long readings[NUM] = {0,};


void setup(void) {
  u8g2.begin();
  FreqCount.begin(500);  
  u8g2.clearBuffer();
  Serial.begin(115200);

  while (!FreqCount.available()) ;
  long f = FreqCount.read();
  for (byte i = 0; i < NUM; i++)
    readings[i] = f;
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

    Serial.println(readings[NUM - 1]);
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

  
    u8g2.clearBuffer();
  
    for (byte i = 0; i < NUM - 1; i++)
      u8g2.drawLine(min(SPAN-1, max(0, SPAN - (readings[i] - mi) * scale)), i,
          min(SPAN-1, max(0, SPAN - (readings[i + 1] - mi) * scale)), i + 1);
    
    u8g2.sendBuffer();
  }
}


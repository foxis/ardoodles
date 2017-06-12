
#include "Adns.h"

// sclk, sdio, pwr, gnd

byte frameBuf[324];  // how much memory does this thing have, anyway?

Mousecam cam( 10, 11, 12, 8);

void setup()
{
  Serial.begin(115200);
  //Serial.println("Testing 123...");
  
  //pinMode(11, OUTPUT);
  //pinMode(10, OUTPUT);
  //pinMode(12, OUTPUT);

  pinMode(6, OUTPUT);  // optional: run an LED
  digitalWrite(6, LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);  
  
  if(cam.adns_init() == 0)
  {
    Serial.println("Cam not found\r\n");
  }
  
}

void loop()
{
    //Serial.print(cam.adns_get_pixel_sum(), HEX);
    //Serial.print("\t");
    //Serial.print(cam.adns_get_delta_x(), HEX);
    //Serial.print("\t");    
    //Serial.print(cam.adns_get_delta_y(), HEX);
    //Serial.print("\n");

    cam.adns_get_frame(frameBuf);
    for(int i=0; i<324; i++)
    {
      Serial.print(frameBuf[i], BYTE);
    }
    
        while (Serial.available() == 0)
    {
    }
    Serial.flush();
  //cam.my_shiftOut(11, 10, MSBFIRST, 0x50);

}


void mymy_shiftOut(int dataPin, int clkPin, byte Ignored, byte data)
{
  // could not find which SPI mode shiftOut assumes (clock idles high/low, etc.) or if it tries to guess based on pin state going in.
  // faster to just write my own than dig for this...
  
byte mydata;  
  
  for (int i=0; i<8; i++)
  {
    //Serial.print(data, HEX);
    mydata = ((data & 0x80) > 0);
    Serial.print(mydata, HEX);
    digitalWrite(dataPin, ((data & 0x80) > 0));
    data = data << 1;
    digitalWrite(clkPin, LOW);
    delayMicroseconds(ADNS_DELAY);
    digitalWrite(clkPin, HIGH);
    delayMicroseconds(ADNS_DELAY);
  }
}

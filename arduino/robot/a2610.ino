#include <nokia5110.h>

// This example reads out the PixArt PAN3101 Optical Navigation Sensor
// It's used in many cheap optical mouses.
//
// For support for the Agilent ADNS-2051, ADNS-2083 or ADNS-2610, move
// the files for your mouse to the folder with the OptiMouse files.
// Then uncomment the right header files and object instances below.
//
// The Arduino will keep track of a (x, y) coordinate by increasing
// or decreasing the x and y variables by dx and respectively dy.
// Every 128th sample it reports the current (x, y) over the Serial.
//
// Written by Martijn The -> post [at] martijnthe.nl
// Tutorial: http://www.martijnthe.nl/optimouse/
// Based on the sketches by Beno�t Rousseau

#include "ADNS2051.h"
#include "ADNS2610.h"
#include "ADNS2083.h"

#define SCLK 2                            // Serial clock pin on the Arduino
#define SDIO 3                            // Serial data (I/O) pin on the Arduino

// ADNS2051 Optical1 = ADNS2051(SCLK, SDIO);
ADNS2610 Optical1 = ADNS2610(SCLK, SDIO);
// ADNS2083 Optical1 = ADNS2083(SCLK, SDIO);

signed long x = 0;                        // Variables for our 'cursor'
signed long y = 0;                        //

int c = 0;// Counter variable for coordinate reporting
Nokia5110 lcd(12,11,13,10,9);

void setup()
{
  Serial.begin(38400);
  Optical1.begin();                       // Resync (not really necessary?)
  lcd.init();
}
static byte data[18 * 18 + 8] = {0,};
static byte bin[18 * 18];
void loop()
{
  int dx;
  int dy;
  int i, j, mi, ma;
  long sum = 0;

  dx = Optical1.dx();
  dy = Optical1.dy();
  
    x += dx;                   // Read the dX register and in/decrease X with that value
    y += dy;                   // Same thing for dY register.....


  // get raw data

  mi  = Optical1.minVal();
  ma = Optical1.maxVal();
  Optical1.pixels(data);
  sum = (mi + ma) / 2;
  for (i = 0; i < 18 * 18; i++)
    bin[i] = 0;
  for (i = 0; i < 18; i++)
  {
    for (j = 0; j < 18; j++)
    {
      byte b = data[i + j * 18] > sum;
      bin[i + (j >>3) * 18] |= b << (j & 7);
    }
  }

  lcd.setPos(0, 0);
  lcd.putSprite(bin, 18, 3);

  lcd.setPos(20, 0);
  lcd.putByte(Optical1.SQUAL()); lcd.putStr("  ");
  lcd.setPos(20, 1);
  lcd.putByte(Optical1.sumVal()); lcd.putStr("  ");
  lcd.setPos(20, 2);
  lcd.putByte(mi); lcd.putStr("  ");
  lcd.setPos(50, 2);
  lcd.putByte(ma); lcd.putStr("  ");
  
  lcd.setPos(0, 3);
  lcd.putInt(sum); lcd.putStr("  ");


  lcd.setPos(0, 4);
  lcd.putInt(x); lcd.putStr("  ");
  lcd.setPos(0, 5);
  lcd.putInt(y); lcd.putStr("  ");

  lcd.setPos(48, 4);
  lcd.putInt(dx); lcd.putStr("  ");
  lcd.setPos(48, 5);
  lcd.putInt(dy); lcd.putStr("  ");



//  }
  
  if (c++ & 0x80)
  {                                       // Report the coordinates once in a while...
    Serial.print("x=");
    Serial.print(x, DEC);
    Serial.print(" y=");
    Serial.print(y, DEC);
    Serial.println();
    c = 0;                                // Reset the report counter
  }
}


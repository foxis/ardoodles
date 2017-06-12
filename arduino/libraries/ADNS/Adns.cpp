/*      $Id: $
  Adns.cpp - Library for interfacing Agilent/Avago ADNS-26xx (ADNS-2610, ADNS-2620, others...?) optical mouse sensors.
  Other sensors from this series may require modifying the Product ID check in adns_init().

  Copyright (c) 2009 Tim R. Gipson (drmn4ea "at" gmail).

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "adns.h"


Mousecam::Mousecam(int sclk, int sdio)
{
  _adns_sclk=sclk;
  _adns_sdio=sdio;
  digitalWrite(_adns_sdio, LOW);
  digitalWrite(_adns_sclk, LOW);
  pinMode(_adns_sclk, OUTPUT);
  // SDIO i/o mode will be determined by whose turn it is to talk...
}

Mousecam::Mousecam(int sclk, int sdio, int pwr, int gnd)
{
  _adns_sclk=sclk;
  _adns_sdio=sdio;
  _adns_pwr=pwr;
  _adns_gnd=gnd;

  digitalWrite(_adns_sdio, LOW);
  digitalWrite(_adns_sclk, LOW);
  digitalWrite(_adns_gnd, LOW);
  digitalWrite(_adns_pwr, HIGH);

  pinMode(_adns_sclk, OUTPUT);
  pinMode(_adns_gnd, OUTPUT);
  pinMode(_adns_pwr, OUTPUT);
  // SDIO i/o mode will be determined by whose turn it is to talk...
}

byte Mousecam::adns_read(byte addr)
{
  pinMode(_adns_sdio, OUTPUT);
  my_shiftOut(_adns_sdio, _adns_sclk, MSBFIRST, addr);  // send Read cmd
  pinMode(_adns_sdio, INPUT);
  //delay?
  return my_shiftIn(_adns_sdio, _adns_sclk, MSBFIRST);
}

void Mousecam::adns_write(byte addr, byte data)
{
  addr = addr | 0x80;  // MSB 1 = Write
  pinMode(_adns_sdio, OUTPUT);
  my_shiftOut(_adns_sdio, _adns_sclk, MSBFIRST, addr);  // send Write cmd
  //delay?
  my_shiftOut(_adns_sdio, _adns_sclk, MSBFIRST, data);  // send data
  pinMode(_adns_sdio, INPUT);
}


byte Mousecam::my_shiftIn(int dataPin, int clkPin, byte Ignored)
{
  // Arduino has a ShiftOut but no corresponding ShiftIn, wtf?!
  // only supports MSB first right now because I'm lazy.
  byte retval=0;
  for (int i=0; i<8; i++)
  {
    digitalWrite(clkPin, LOW);
    delayMicroseconds(ADNS_DELAY);
    retval = (retval << 1) | digitalRead(_adns_sdio);
    digitalWrite(clkPin, HIGH);
    delayMicroseconds(ADNS_DELAY);
  }
  return retval;
}

void Mousecam::my_shiftOut(int dataPin, int clkPin, byte Ignored, byte data)
{
  // could not find which SPI mode shiftOut assumes (clock idles high/low, etc.) or if it tries to guess based on pin state going in, etc.
  // faster to just write my own than dig for this...
  for (int i=0; i<8; i++)
  {
    digitalWrite(dataPin, ((data & 0x80) > 0));
    data = data << 1;
    digitalWrite(clkPin, LOW);
    delayMicroseconds(ADNS_DELAY);
    digitalWrite(clkPin, HIGH);
    delayMicroseconds(ADNS_DELAY);
  }
}



byte Mousecam::adns_init(void)
{
   delay(500);
   adns_write(ADNS_CONFIG, 0x80); // Reset
   delay(500);
   adns_wake();            // force awake until explicitly powered down by the user

   if((adns_read(ADNS_STATUS) & 0xE0) != 0x40)  // read Product ID from Status reg (0b010xxxxx)
   {
     return 0;
   }
   return 1;
}

void Mousecam::adns_wake(void)
{
   adns_write(ADNS_CONFIG, 0x01); // Force Awake
}


char Mousecam::adns_get_delta_x(void)
{
   return(adns_read(ADNS_DELTA_X));
}

char Mousecam::adns_get_delta_y(void)
{
   return(adns_read(ADNS_DELTA_Y));
}


byte Mousecam::adns_get_pixel_sum(void)
{
   return(adns_read(ADNS_PIXEL_SUM));
}

byte Mousecam::adns_get_max_pixel(void)
{
   return(adns_read(ADNS_MAX_PIXEL));
}

byte Mousecam::adns_get_min_pixel(void)
{
   return(adns_read(ADNS_MIN_PIXEL));
}

byte Mousecam::adns_get_surface_quality(void)
{
   return(adns_read(ADNS_SQUAL));
}

void Mousecam::adns_get_frame(byte * buf)
{
 // frame size is always 18*18 = 324 pixels.

 adns_write(ADNS_PIXEL_GRAB, 0x00); // dummy write to arm the pixel grab hardware

 for(int i=0; i<324; i++)
 {
    // According to the datasheet, next read SHOULD be pixel 1 and valid...
    do
    {
       buf[i] = adns_read(ADNS_PIXEL_GRAB);
    } while (!(buf[i] & 0x40));            // don't count unless DATA_VALID flag set (bit 6)
 }
}


/*      $Id: $
  Adns.h - Library for interfacing Agilent/Avago ADNS-26xx (ADNS-2610, ADNS-2620, others...?) optical mouse sensors.
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

#ifndef adns_h
#define adns_h

#include "WProgram.h"
#include "WConstants.h"

#define    ADNS_DELAY  100 /* serial clock delay in microseconds */

//; Register map:
#define		ADNS_CONFIG			0x40
#define		ADNS_STATUS			0x41

#define		ADNS_DELTA_X	        	0x43
#define		ADNS_DELTA_Y		        0x42

#define		ADNS_SQUAL			0x44


#define		ADNS_MAX_PIXEL			0x45
#define		ADNS_MIN_PIXEL			0x46
#define		ADNS_PIXEL_SUM			0x47
#define		ADNS_PIXEL_GRAB		        0x48
#define	        ADNS_PIXEL_DATA		        ADNS_PIXEL_GRAB		//; name inconsistent in some ADNS* parts
#define		ADNS_SHUTTER_UPPER			0x49
#define		ADNS_SHUTTER_LOWER			0x4A
#define		ADNS_FRAME_PERIOD			0x4B

#define		ADNS_DUMMY_REG				0x77		//; Dummy write register (invalid address). After a read, ADNS* will continue driving the data line until the start of the next opcode. Use a dummy write to force ADNS* to float SDIO.

class Mousecam
{
  public:
         Mousecam(int sclk, int sdio);
         Mousecam(int sclk, int sdio, int pwr, int gnd);
         byte adns_read(byte addr);
         void adns_write(byte addr, byte data);
         byte my_shiftIn(int dataPin, int clkPin, byte Ignored);
         void my_shiftOut(int dataPin, int clkPin, byte Ignored, byte data);
         byte adns_init(void);
         void adns_wake(void);
         char adns_get_delta_x(void);
         char adns_get_delta_y(void);
         byte adns_get_pixel_sum(void);
         byte adns_get_max_pixel(void);
         byte adns_get_min_pixel(void);
         byte adns_get_surface_quality(void);
         void adns_get_frame(byte * buf);
  private:
          int _adns_sclk;
          int _adns_sdio;
          int _adns_pwr;
          int _adns_gnd;
};




#endif

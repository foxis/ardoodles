#pragma once
/*
Date: 03/05/2013
Author: Boris Mazic
Abstract:
	The software driver for Avago ADNS-2610 optical mouse sensor.
	Some features of the sensor are:
		- 8-pin staggered dual inline package (DIP)
		- Accurate motion up to 12 ips
		- 400 cpi resolution
		- Single 5.0 volt power supply
		- Power conservation mode during times of no movement
		- Frame rate = 1500 fps
	
	P#	Pin 		Description
	----------------------------------------------------------
	1 	OSC_IN 		Oscillator input
	2 	OSC_OUT 	Oscillator output
	3 	SDIO 		Serial data (input and output)
	4 	SCK 		Serial port clock (Input)
	5 	LED_CNTL 	Digital Shutter Signal Out
	6 	GND 		System Ground
	7 	VDD 		5V DC Input
	8 	REFA 		Internal reference

	Based on the source code by Martijn The.
	http://www.martijnthe.nl/
 
References:
	[1] ADNS-2610__AV02-1184EN.pdf
*/
/*
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

#include <inttypes.h>
#include "OptiMouse.h"

class ADNS2610 : public OptiMouse
{
public:
	ADNS2610(uint8_t sclkPin, uint8_t sdioPin);
	uint8_t frame_x() const;
	uint8_t frame_y() const;
	int8_t dx(void);
	int8_t dy(void);
	void reset(bool b);
	void power_down(bool b);
	void force_awake_mode(bool b);
	uint8_t product_id(void);
	uint8_t is_awake(void);
	uint8_t squal(void);
	uint8_t maximum_pixel(void);
	uint8_t minimum_pixel(void);
	uint8_t pixel_sum(void);
	void pixel_data(uint8_t *frame, int pixels = 0);
	uint16_t shutter(void);
	uint8_t inverse_product(void);
};

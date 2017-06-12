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

#include "OptiMouse.h"
#include "ADNS2610.h"

struct Register {
	enum {
		Configuration = 0x00,	// Reset, Power Down, Forced Awake, etc
		Status = 0x01,			// Product ID, Mouse state of Asleep or Awake
		Delta_Y = 0x02,			// Y Movement
		Delta_X = 0x03,			// X Movement
		SQUAL = 0x04,			// Measure of the number of features visible by the sensor
		Maximum_Pixel = 0x05,
		Minimum_Pixel = 0x06,
		Pixel_Sum = 0x07,
		Pixel_Data = 0x08,		// Actual picture of surface
		Shutter_Upper = 0x09,
		Shutter_Lower = 0x0A,
		Inverse_Product = 0x11 	// Inverse Product ID
	};
};

ADNS2610::ADNS2610(uint8_t sclkPin, uint8_t sdioPin)
: OptiMouse::OptiMouse(sclkPin, sdioPin)
{

}

uint8_t ADNS2610::frame_x() const {
	return 18; // horizontal resolution in pixels
}

uint8_t ADNS2610::frame_y() const {
	return 18; // vertical resolution in pixels
}

// [1] p.20
int8_t ADNS2610::dx(void) {
	return (int8_t)readRegister(Register::Delta_X);
}

// [1] p.20
int8_t ADNS2610::dy(void) {
	return (int8_t)readRegister(Register::Delta_Y);
}

// [1] p.19
void ADNS2610::reset(bool b) {
	uint8_t config = readRegister(Register::Configuration);
	uint8_t mask = (1 << 7);
	if(b) {
		config |= mask;
	} else {
		config &= ~mask;
	}
	writeRegister(Register::Configuration, config);
}

// [1] p.19
void ADNS2610::power_down(bool b) {
	uint8_t config = readRegister(Register::Configuration);
	uint8_t mask = (1 << 6);
	if(b) {
		config |= mask;
	} else {
		config &= ~mask;
	}
	writeRegister(Register::Configuration, config);
}

// [1] p.19
void ADNS2610::force_awake_mode(bool b) {
	uint8_t config = readRegister(Register::Configuration);
	uint8_t mask = (1 << 0);
	if(b) {
		config |= mask;
	} else {
		config &= ~mask;
	}
	writeRegister(Register::Configuration, config);
}

// [1] p.19
uint8_t ADNS2610::product_id(void) {
	return readRegister(Register::Status) >> 5;
}

// [1] p.19
uint8_t ADNS2610::is_awake(void) {
	return readRegister(Register::Status) & 0x01;
}

// [1] p.21
uint8_t ADNS2610::squal(void) {
	return readRegister(Register::SQUAL);
}

// [1] p.22
uint8_t ADNS2610::maximum_pixel(void) {
	return readRegister(Register::Maximum_Pixel);
}

// [1] p.22
uint8_t ADNS2610::minimum_pixel(void) {
	return readRegister(Register::Minimum_Pixel);
}

// [1] p.23
uint8_t ADNS2610::pixel_sum(void) {
	return readRegister(Register::Pixel_Sum);
}

/*
[1] p.23
USAGE: Digital Pixel data. Minimum value = 0, maximum value = 63. Any writes to this register resets the pixel hardware so that the next read from the Pixel Data register will read pixel #1 and the StartOfFrame bit will be set. Subsequent reads will auto increment the pixel number.
To dump a complete image, set the LED to forced awake mode, write anything to this register, then read 324 times where the DataValid bit is set. On the 325th read, the StartOfFrame bit will be set indicating that we have completed one frame of pixels and are starting back at pixel 1.
*/
void ADNS2610::pixel_data(uint8_t *frame, int pixels) {
	uint8_t const SOF = 1 << 7;
	uint8_t const DATA_VALID = 1 << 6;
	uint8_t const MASK = ~(SOF|DATA_VALID);
	static int const FRAME_X = 18; // in pixels
	static int const FRAME_Y = 18; // in pixels
	
	if(pixels == 0) {
		pixels = FRAME_X*FRAME_Y;
	}
	force_awake_mode(true);
	writeRegister(Register::Pixel_Data, 0x5A); 		// write anything to this register to initiate a frame
	for(int count=0; count < pixels;) {
		uint8_t data = readRegister(Register::Pixel_Data);
		if(data & DATA_VALID) {
			frame[count++] = data & MASK;
		}
	}
	force_awake_mode(false);
}

// [1] p.26
uint16_t ADNS2610::shutter(void) {
	uint16_t s = readRegister(Register::Shutter_Upper) << 8;
	s |= readRegister(Register::Shutter_Lower);
	return s;
}

// [1] p.27
uint8_t ADNS2610::inverse_product(void) {
	return readRegister(Register::Inverse_Product) & 0x0F;
}


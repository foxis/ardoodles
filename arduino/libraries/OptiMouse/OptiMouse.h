#pragma once
/*
Date: 03/05/2013
Author: Boris Mazic
Abstract:
	The base class of the optical mouse sensor software driver.

	Based on the source code by Martijn The.
	http://www.martijnthe.nl/
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

#include <Arduino.h>
#include <inttypes.h>

class OptiMouse {
protected:
	uint8_t _sclkPin;
    uint8_t _sdioPin;
	uint8_t readRegister(uint8_t address);
	void writeRegister(uint8_t address, uint8_t data);

public:
    OptiMouse(uint8_t sclkPin, uint8_t sdioPin);
    void init();
	
	// These need to be modified as the support for new sensors gets added.
	// Currently, it is modelled on the ADNS2610 sensor capabilities.
	virtual uint8_t frame_x() const = 0;
	virtual uint8_t frame_y() const = 0;
	virtual int8_t dx(void) = 0;
	virtual int8_t dy(void) = 0;
	virtual void reset(bool b) = 0;
	virtual void power_down(bool b) = 0;
	virtual void force_awake_mode(bool b) = 0;
	virtual uint8_t product_id(void) = 0;
	virtual uint8_t is_awake(void) = 0;
	virtual uint8_t squal(void) = 0;
	virtual uint8_t maximum_pixel(void) = 0;
	virtual uint8_t minimum_pixel(void) = 0;
	virtual uint8_t pixel_sum(void) = 0;
	virtual void pixel_data(uint8_t *frame, int pixels = 0) = 0;
	virtual uint16_t shutter(void) = 0;
	virtual uint8_t inverse_product(void) = 0;
};

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

#include "OptiMouse.h"

OptiMouse::OptiMouse(uint8_t sclkPin, uint8_t sdioPin) {
	_sclkPin = sclkPin;
	_sdioPin = sdioPin;
	pinMode(_sclkPin, OUTPUT);
	pinMode(_sdioPin, INPUT);
}


void OptiMouse::init(void) {
	// Re-sync (see datasheet §5.4):
	// Toggle the SLCK line from high to low to high....
	digitalWrite(_sclkPin, HIGH);                     
	delayMicroseconds(5);
	digitalWrite(_sclkPin, LOW);
	delayMicroseconds(1);
	digitalWrite(_sclkPin, HIGH);
	
	// Wait at least tSIWTT (0.9 second?) for the
	// OptiMouse serial transaction timer to time out:
	delay(1000);
}

uint8_t OptiMouse::readRegister(uint8_t address) {
	// Write the address of the register we want to read:
	pinMode(_sdioPin, OUTPUT);
	for(int i=7; i >= 0; i--) {
		digitalWrite(_sclkPin, LOW);
		digitalWrite(_sdioPin, address & (1 << i));
		digitalWrite(_sclkPin, HIGH);
	}
	
	// Switch data line from OUTPUT to INPUT
	pinMode(_sdioPin, INPUT);
	// Wait (per the datasheet, the chip needs a minimum of 100 µsec to prepare the data)
	delayMicroseconds(100);
	
	// Fetch the data!
	uint8_t r = 0;
	for(int i=7; i >= 0; i--) {                             
		digitalWrite(_sclkPin, LOW);
		digitalWrite(_sclkPin, HIGH);
		r |= digitalRead(_sdioPin) << i;
	}
	// Tailing delay guarantees >100 µsec before next transaction
	delayMicroseconds(100);
	return r;
}

void OptiMouse::writeRegister(uint8_t address, uint8_t data) {
	// Set MSB high, to indicate write operation:
	address |= 0x80;
	// Write the address:
	pinMode(_sdioPin, OUTPUT);
	for(int i=7; i >= 0; i--) {
		digitalWrite(_sclkPin, LOW);
		digitalWrite(_sdioPin, address & (1 << i));
		digitalWrite(_sclkPin, HIGH);
	}
	// Write the data:
	for(int i=7; i >= 0; i--) {
		digitalWrite(_sclkPin, LOW);
		digitalWrite(_sdioPin, data & (1 << i));
		digitalWrite(_sclkPin, HIGH);
	}
}

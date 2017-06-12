/*
 ADNS2083.cpp - Part of optical mouse sensor library for Arduino
 Copyright (c) 2008 Martijn The.  All right reserved.
 http://www.martijnthe.nl/
 
 Based on sketches by Beno�t Rousseau.
 
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

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "WConstants.h"
#include "OptiMouse.h"
#include "ADNS2083.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define Delta_Y				0x42
#define Delta_X				0x43

/******************************************************************************
 * Constructors
 ******************************************************************************/


ADNS2083::ADNS2083(uint8_t sclkPin, uint8_t sdioPin) : OptiMouse::OptiMouse(sclkPin, sdioPin)
{

}

/******************************************************************************
 * User API
 ******************************************************************************/

signed char ADNS2083::dx(void)
{
	return (signed char) readRegister(Delta_X);
}

signed char ADNS2083::dy(void)
{
	return (signed char) readRegister(Delta_Y);
}

// Private Methods /////////////////////////////////////////////////////////////


/*************************************************** 
 *  BounceSimplePCF.h v1.0 - Debouncing for PCF8574
 *
 *  Copyright (c) 2018 Michael Zimmermann <http://www.kruemelsoft.privat.t-online.de>
 *  All rights reserved.
 *
 *  Main code taken from Bounce2Mcp by Thomas O Fredericks (tof@t-o-f.info)
 *  Previous contributions by Eric Lowry, Jim Schimpf and Tom Harkaway
 *
 *  LICENSE
 *  -------
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************/

#ifndef _KS_BounceSimple_h
#define _KS_BounceSimple_h

#include <inttypes.h>

class BounceSimplePcf
{
 public:
    // Create an instance of the bounce library
    BounceSimplePcf();

    // Attach to a PCF object, a pin (and also sets initial state), and set debounce interval.
    void attach(uint8_t pcfAddress, int pin, uint16_t interval_millis);

    // Sets the debounce interval
    void interval(uint16_t interval_millis);

    // Updates the pin
    // Returns 1 if the state changed
    // Returns 0 if the state did not change
    bool update();

    // Returns the updated pin state
    bool read();

    // Returns the falling pin state
    bool fell();

    // Returns the rising pin state
    bool rose();

 protected:
		bool readPins(uint8_t pin);
		
    unsigned long previous_millis;
    uint16_t interval_millis;
    uint8_t state;
    uint8_t pin;
    uint8_t pcfAddress;
};

#endif

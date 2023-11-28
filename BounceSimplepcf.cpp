// Please read BounceSimplepcf.h for information about the licence and authors

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "BounceSimplepcf.h"

extern "C" {
	#include "utility\i2c.h"
}

#define DEBOUNCED_STATE 0
#define UNSTABLE_STATE  1
#define STATE_CHANGED   3


BounceSimplePcf::BounceSimplePcf()
    : previous_millis(0)
    , interval_millis(10)
    , state(0)
    , pin(0)
		, pcfAddress(0)
{}

void BounceSimplePcf::attach(uint8_t pcfAddress, int pin, uint16_t interval_millis) {
    this->pin = pin;
    this->pcfAddress = pcfAddress;
    this->interval_millis = interval_millis;

		state = readPins(pin) ? _BV(DEBOUNCED_STATE) | _BV(UNSTABLE_STATE) : 0;
		previous_millis = millis();
}

void BounceSimplePcf::interval(uint16_t interval_millis)
{
    this->interval_millis = interval_millis;
}

bool BounceSimplePcf::update()
{
    // Read the state of the switch in a temporary variable.
    bool currentState = readPins(pin);
    state &= ~_BV(STATE_CHANGED);

    // If the reading is different from last reading, reset the debounce counter
    if ( currentState != (bool)(state & _BV(UNSTABLE_STATE)) ) {
        previous_millis = millis();
        state ^= _BV(UNSTABLE_STATE);
    } else
        if ( millis() - previous_millis >= interval_millis ) {
            // We have passed the threshold time, so the input is now stable
            // If it is different from last state, set the STATE_CHANGED flag
            if ((bool)(state & _BV(DEBOUNCED_STATE)) != currentState) {
                previous_millis = millis();
                state ^= _BV(DEBOUNCED_STATE);
                state |= _BV(STATE_CHANGED);
            }
        }

    return state & _BV(STATE_CHANGED);
}

bool BounceSimplePcf::read()
{
    return state & _BV(DEBOUNCED_STATE);
}

bool BounceSimplePcf::rose()
{
    return ( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}

bool BounceSimplePcf::fell()
{
    return !( state & _BV(DEBOUNCED_STATE) ) && ( state & _BV(STATE_CHANGED));
}

bool BounceSimplePcf::readPins(uint8_t pin)
{
   register uint8_t iRetValue(0xff);
   if(pcfAddress)
   {
	   i2c_start(pcfAddress + 1);
     iRetValue = i2c_readNAck();
	   i2c_stop();
   }
  return (((~iRetValue) >> pin) & 0x01 ? true : false);	// since buttons are switch to GND, we invert the state
}

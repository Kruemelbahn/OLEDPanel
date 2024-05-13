/*
||
|| @file OLEDPanel.h
|| @version 1.1
|| @author Michael Zimmermann
|| @contact michael.zimmermann.sg@t-online.de
||
|| @description
|| | encapsulates and expands the OLED-library into class 'OLEDPanel'
|| | Original Source written by M.Köhler (https://github.com/Sylaina/oled-display.git)
|| | remark: original source was modified and expanded
|| |
|| | This library includes functionality for button-states (more functionality)
|| | 
|| | routines for I2C-OLED-Display 128*64, 1.3"
|| |   uses: i2c-files	: i2c.c, i2c.h
|| |   uses: oled-files : lcd.h, lcd.c
|| |   uses: font-files : font.h, font.c
|| | 
|| | defines in lcd.h:
|| | displaycontroller   #define SH1106
|| | displaymode         #define TEXTMODE
|| | I2C-ADDR            #define LCD_I2C_ADR 0x78
|| #
||
|| @license
|| |	Copyright (c) 2018 Michael Zimmermann <http://www.kruemelsoft.privat.t-online.de>
|| |	All rights reserved.
|| |	
|| |	This program is free software: you can redistribute it and/or modify
|| |	it under the terms of the GNU General Public License as published by
|| |	the Free Software Foundation, either version 3 of the License, or
|| |	(at your option) any later version.
|| |	
|| |	This program is distributed in the hope that it will be useful,
|| |	but WITHOUT ANY WARRANTY; without even the implied warranty of
|| |	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|| |	GNU General Public License for more details.
|| |	
|| |	You should have received a copy of the GNU General Public License
|| |	along with this program. If not, see <http://www.gnu.org/licenses/>.
|| #
||
*/

#ifndef _KS_OLEDPANEL_H
#define _KS_OLEDPANEL_H

#include <inttypes.h>
#include <Print.h>

extern "C" {
	#include "utility\lcd.h"
}

#include "BounceSimplepcf.h"

#define CHAR_HEIGHT	8
#define CHAR_WIDTH 6
#define COUNT_OF_CHARS (DISPLAY_WIDTH/CHAR_WIDTH)	// = currently 21
#define COUNT_OF_LINES (DISPLAY_HEIGHT/CHAR_HEIGHT)	// = currrenly 8

#define BUTTON_SELECT 0x01
#define BUTTON_RIGHT 0x02	// same as F4
#define BUTTON_DOWN 0x04
#define BUTTON_UP 0x08
#define BUTTON_LEFT 0x10	// same as F0
#define BUTTON_FCT_BACK (BUTTON_LEFT | BUTTON_UP | BUTTON_DOWN)
#define BUTTON_UPDOWN (BUTTON_UP | BUTTON_DOWN)
#define BUTTON_MENU (BUTTON_UP | BUTTON_DOWN | BUTTON_SELECT)

#define DEBOUNCE_TIME 5

#define fontCount 105   // whithout appending specialchar...

/* OLEDPanel is derived from class 'Print'
   to become compatible in function-calls with other display-libraries
	 like 'Adafruit_RGBLCDShield' from adafruit.com
*/
class OLEDPanel : public Print {
	public:
		OLEDPanel();

		uint16_t detect_i2c(uint8_t ui8_keyAddr);

		void begin(uint8_t cols = 0, uint8_t rows = 0);
		void setKeyAddr(uint8_t ui8_keyAddr, bool bInit = true);
		
		void clear();
		void clear(uint8_t x, uint8_t y, uint8_t iCount);
		void clearLine(uint8_t y);
		void clearToEOL(uint8_t x, uint8_t y);

		void setCharMode(bool bDouble, bool bInvert, bool bUnderline);

		void noCursor();
		void cursor(uint8_t x, uint8_t y);
		void refresh();

		void setCursor(uint8_t x, uint8_t y); 

		void printOuterFrame();

		size_t print(const __FlashStringHelper *pText);
		size_t print(const char *pText);
		size_t print(const String& s);
		size_t print(char ch);
		size_t print(uint8_t x, uint8_t y, char ch);
		size_t print(uint8_t ui8Value, int iType);
		size_t print(uint16_t ui16Value, int iType);
		size_t print(unsigned long ulValue, int iType);

		size_t printc(uint8_t y, const __FlashStringHelper *pText);
		size_t printc(uint8_t y, const char *pText);
		size_t printc(uint8_t x, uint8_t y, unsigned long ulValue, int iType);

    size_t printr(uint8_t y, uint8_t iMaxChar, const __FlashStringHelper *pText);

		uint8_t readButtons();

		static char* intToAscii(char *buf, uint8_t len, unsigned long n, uint8_t base);

// write is declared pure virtual in class Print and needs to be implemented:
#if ARDUINO >= 100
		virtual size_t write(uint8_t) { return 0; };
#else
		virtual void write(uint8_t) {};
#endif

		void	updateDebounce();

		BounceSimplePcf debouncer_OK; 
		BounceSimplePcf debouncer_Right; 
		BounceSimplePcf debouncer_Down; 
		BounceSimplePcf debouncer_Up; 
		BounceSimplePcf debouncer_Left; 
		BounceSimplePcf debouncer_F1; 
		BounceSimplePcf debouncer_F2; 
		BounceSimplePcf debouncer_F3; 
		 
	protected:
		void initButtons();
    uint8_t countChar(const char *ps);
    uint8_t countChar(const __FlashStringHelper *ps);
    bool setStartPositionForCenterText(uint8_t y, uint8_t iCount);
    bool setStartPositionForRightText(uint8_t y, uint8_t iMaxChar, uint8_t iCount);

		uint8_t m_ui8KeyAddr;
		bool m_bCursorOn;
		bool m_bBlinken1Hz;

		struct {
			uint8_t x;
			uint8_t y;
		} cursorPos;

	private:
		unsigned long m_ulpreviousMillis;
};

#endif

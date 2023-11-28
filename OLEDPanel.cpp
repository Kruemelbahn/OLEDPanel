/*
||
|| @file OLEDPanel.cpp
|| @version 1.0
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

#include "OLEDPanel.h"

#include <stdio.h>
#include <string.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

extern uint8_t I2C_ErrorCode;

OLEDPanel::OLEDPanel() {
	m_ui8KeyAddr = 0;
	m_bCursorOn = false;
	m_bBlinken1Hz = false;
	m_ulpreviousMillis = 0;
	m_ui8KeyAddr = 0;
	cursorPos.x = 0;
	cursorPos.y = 0;
}

uint16_t OLEDPanel::detect_i2c(uint8_t ui8_keyAddr)
{
	m_ui8KeyAddr = ui8_keyAddr;

	// OLED-Display:
	i2c_init();
	i2c_start(LCD_I2C_ADR << 1);
	if(I2C_ErrorCode)
		return I2C_ErrorCode;

	if (m_ui8KeyAddr)
	{
		// PCF8574 for Buttons:
		i2c_init();
		i2c_start(m_ui8KeyAddr);
		return I2C_ErrorCode << 8;
	}
	return 0;
}

void OLEDPanel::begin(uint8_t /*cols*/, uint8_t /*rows*/)
{
	lcd_init(LCD_DISP_ON);    // init lcd and turn on

	initButtons();
}

void OLEDPanel::initButtons()
{
	// init Buttons:
	if(m_ui8KeyAddr)
	{
	  i2c_init();
	  i2c_start(m_ui8KeyAddr);
	  i2c_byte(0b11111111); // all inputs
	  i2c_stop();

		// After setting up the button, setup the Bounce instances :
		debouncer_OK.attach(m_ui8KeyAddr, 0, DEBOUNCE_TIME);
		debouncer_Right.attach(m_ui8KeyAddr, 1, DEBOUNCE_TIME);
		debouncer_Down.attach(m_ui8KeyAddr, 2, DEBOUNCE_TIME);
		debouncer_Up.attach(m_ui8KeyAddr, 3, DEBOUNCE_TIME);
		debouncer_Left.attach(m_ui8KeyAddr, 4, DEBOUNCE_TIME);
		debouncer_F1.attach(m_ui8KeyAddr, 5, DEBOUNCE_TIME);
		debouncer_F2.attach(m_ui8KeyAddr, 6, DEBOUNCE_TIME);
		debouncer_F3.attach(m_ui8KeyAddr, 7, DEBOUNCE_TIME);
	}
}

void OLEDPanel::updateDebounce()
{
	if (m_ui8KeyAddr)
	{
		// Update the Bounce instances :
		debouncer_OK.update();
		debouncer_Right.update();
		debouncer_Down.update();
		debouncer_Up.update();
		debouncer_Left.update();
		debouncer_F1.update();
		debouncer_F2.update();
		debouncer_F3.update();
	}
}


void OLEDPanel::setKeyAddr(uint8_t ui8_keyAddr, bool bInit)
{
  m_ui8KeyAddr = ui8_keyAddr;

	if(bInit)
		initButtons();
}
 
void OLEDPanel::clear()
{
	lcd_clrscr();
}

void OLEDPanel::clearLine(uint8_t y)
{
	clearToEOL(0, y);
}

void OLEDPanel::clearToEOL(uint8_t x, uint8_t y)
{
	clear(x, y, COUNT_OF_CHARS - x);
}

// clear iCount of chars starting at x, y
void OLEDPanel::clear(uint8_t x, uint8_t y, uint8_t iCount)
{
	if((x + iCount) > COUNT_OF_CHARS || y > (COUNT_OF_LINES - 1))
		return;
	lcd_gotoxy(x, y);

	register uint8_t iSize(6 * iCount);
	uint8_t clearLine[iSize];
	memset(clearLine, 0x00, iSize);
	lcd_data(clearLine, sizeof(clearLine));
	lcd_gotoxy(x/* + iCount*/, y);
}

void OLEDPanel::setCharMode(bool bDouble, bool bInvert, bool bUnderline)
{
  uint8_t uCharMode(NORMALSIZE);
  if (bDouble)
    uCharMode = DOUBLESIZE;
  if (bUnderline)
    uCharMode |= UNDERLINE;
  if (bInvert)
    uCharMode |= INVERT;
	lcd_charMode(uCharMode);
}

void OLEDPanel::noCursor()
{
	m_bCursorOn = false;
	clearToEOL(0, cursorPos.y + 1);
}

void OLEDPanel::cursor(uint8_t x, uint8_t y)
{
	m_bCursorOn = true;
	cursorPos.x = x;
	cursorPos.y = y;
  clearToEOL(0, cursorPos.y + 1);
	lcd_gotoxy(cursorPos.x, cursorPos.y + 1);
	lcd_putc(0xAF);  // (Macron = 'Overline')
	lcd_gotoxy(cursorPos.x, cursorPos.y);
}

//needs to be called cyclic e.g. to generate 1-Hz-puls
void OLEDPanel::refresh()
{
	if (millis() - m_ulpreviousMillis > 500)
	{
		m_ulpreviousMillis = millis();   // keep actual timevalue
		// change status:
		m_bBlinken1Hz = !m_bBlinken1Hz;
	}
}

void OLEDPanel::setCursor(uint8_t x, uint8_t y)
{
	lcd_gotoxy(x, y);
}

size_t OLEDPanel::print(const __FlashStringHelper *pText)
{
	// count chars to display
	PGM_P p1(reinterpret_cast<PGM_P>(pText));
	register uint8_t c;
	register uint8_t iCount(0);
	while ((c = pgm_read_byte(p1++)))
	{
		lcd_putc((unsigned char)(c));
		++iCount;
	}

	return iCount;
}

size_t OLEDPanel::print(const char *pText)
{
	// count chars to display
	register uint8_t iCount(0);
	while (*pText)
	{
		lcd_putc((unsigned char)(*pText++));
		++iCount;
	}

	return iCount;
}

size_t OLEDPanel::print(char ch)
{
	lcd_putc((unsigned char)(ch));
	return 1;
}

size_t OLEDPanel::print(uint8_t x, uint8_t y, char ch)
{
	lcd_gotoxy(x, y);
	lcd_putc((unsigned char)(ch));
	return 1;
}

#define BUF_SIZE 33
size_t OLEDPanel::print(uint8_t ui8Value, int iType)
{
	char buf[BUF_SIZE]; // Assumes 8-bit chars plus zero byte.
	return print(intToAscii((char*)&(buf), BUF_SIZE, (unsigned long)ui8Value, iType));
}

size_t OLEDPanel::print(uint16_t ui16Value, int iType)
{
	char buf[BUF_SIZE]; // Assumes 8-bit chars plus zero byte.
	return print(intToAscii((char*)&(buf), BUF_SIZE, (unsigned long)ui16Value, iType));
}

size_t OLEDPanel::print(unsigned long ulValue, int iType)
{
	char buf[BUF_SIZE]; // Assumes 8-bit chars plus zero byte.
	return print(intToAscii((char*)&(buf), BUF_SIZE, ulValue, iType));
}

// output text centered in line y
size_t OLEDPanel::printc(uint8_t y, const __FlashStringHelper *pText)
{
	if(setStartPositionForCenterText(y, countChar(pText)))
		// output text to display
		return print(pText);

	return 0;
}

// output text centered in line y
size_t OLEDPanel::printc(uint8_t y, const char *pText)
{
	if(setStartPositionForCenterText(y, countChar(pText)))
		// output text to display
		return print(pText);

	return 0;
}

// output number
// x!=255 && y!=255 : use cursorposition x, y
// x==255 && y!=255 : center in line y
size_t OLEDPanel::printc(uint8_t x, uint8_t y, unsigned long ulValue, int iType)
{
	char buf[BUF_SIZE]; // Assumes 8-bit chars plus zero byte.
	char *pText(intToAscii((char*)&(buf), BUF_SIZE, ulValue, iType));
	
	if(y != 255)
	{
		if(x == 255)
		{
			if(!setStartPositionForCenterText(y, countChar(pText)))
				return 0;
		}
		else
			lcd_gotoxy(x, y);
	}
	// output text to display
	return print(pText);
}

// output text rightaligned in line y
size_t OLEDPanel::printr(uint8_t y, uint8_t iMaxChar, const __FlashStringHelper *pText)
{
  if (setStartPositionForRightText(y, iMaxChar, countChar(pText)))
    // output text to display
    return print(pText);

  return 0;
}


// bit is set for each button pressed
uint8_t OLEDPanel::readButtons()
{
   register uint8_t iRetValue(0xff);
   if(m_ui8KeyAddr)
   {
	  i2c_start(m_ui8KeyAddr + 1);
    iRetValue = i2c_readNAck();
	  i2c_stop();
   }
	return ~iRetValue; // since buttons are switch to GND, we invert the state
}

void OLEDPanel::printOuterFrame()
{
	// upper frameline
	lcd_gotoxy(0, 0);
	i2c_start(LCD_I2C_ADR << 1);
	i2c_byte(0x40);	// 0x00 for command, 0x40 for data
	i2c_byte(0xFF);
	for (uint8_t i = 1; i < (DISPLAY_WIDTH - 1); i++)
		i2c_byte(0x01);
	i2c_byte(0xFF);
	i2c_stop();

	// border lines
	for (uint8_t i = 1; i < (COUNT_OF_LINES - 1); i++)
	{
		// left border
		lcd_gotoxy(0, i);
		i2c_start(LCD_I2C_ADR << 1);
		i2c_byte(0x40);	// 0x00 for command, 0x40 for data
		i2c_byte(0xFF);
		i2c_stop();

		// right border
		lcd_gotoxy(COUNT_OF_CHARS - 1, i);
		i2c_start(LCD_I2C_ADR << 1);
		i2c_byte(0x40);	// 0x00 for command, 0x40 for data
		for (uint8_t j = 0; j <= CHAR_WIDTH; j++)
			i2c_byte(0x00);
		i2c_byte(0xFF);
		i2c_stop();
	}

	// lower frameline
	lcd_gotoxy(0, COUNT_OF_LINES - 1);
	i2c_start(LCD_I2C_ADR << 1);
	i2c_byte(0x40);	// 0x00 for command, 0x40 for data
	i2c_byte(0xFF);
	for (uint8_t i = 1; i < (DISPLAY_WIDTH - 1); i++)
		i2c_byte(0x80);
	i2c_byte(0xFF);
	i2c_stop();
}

//=== static functions ========================================================
char* OLEDPanel::intToAscii(char *buf, uint8_t len, unsigned long n, uint8_t base)
{
	char *str(&buf[len-1]);
	*str = '\0';
	// prevent crash if called with base == 1
	if (base < BIN)
		base = DEC;
	do
	{
		char c(n % base);
		n /= base;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
		--len;
	} while(n && (len != 0));
	return str;
}

//=== protected functions =====================================================
uint8_t OLEDPanel::countChar(const char *ps)
{
  // count chars to display
  register uint8_t iCount(0);
  while (*ps++)
    ++iCount;

  return iCount;
}

uint8_t OLEDPanel::countChar(const __FlashStringHelper *pText)
{
  // count chars to display
  PGM_P ps(reinterpret_cast<PGM_P>(pText));
  register uint8_t iCount(0);
  while (pgm_read_byte(ps++))
    ++iCount;

  return iCount;
}

// calculate startposition for centered text
bool OLEDPanel::setStartPositionForCenterText(uint8_t y, uint8_t iCount)
{
  if (y > (COUNT_OF_LINES - 1))
    return false; // out of display

	// TODO ZIM: not shure why "lcd_gotoxy(0, y)" is neccessary
	lcd_gotoxy(0, y);

  // calculate startposition for text
	uint8_t x((DISPLAY_WIDTH - (iCount * CHAR_WIDTH)) / 2);
	// the following 'lcd_command' is similar to 'lcd_gotoxy')
#if defined SSD1306
	uint8_t commandSequence[] = {(uint8_t)(0xb0+y), 0x21, x, 0x7f};
#elif defined SH1106
	uint8_t commandSequence[] = {(uint8_t)(0xb0+(y & 0x0f)), 0x21, (uint8_t)(0x00+((2+x) & (0x0f))), (uint8_t)(0x10+( ((2+x) & (0xf0)) >> 4 )), 0x7f};
#endif
	lcd_command(commandSequence, sizeof(commandSequence));
	return true;
}

bool OLEDPanel::setStartPositionForRightText(uint8_t y, uint8_t iMaxChar, uint8_t iCount)
{
  if (y > (COUNT_OF_LINES - 1))
    return false; // out of display

  // TODO ZIM: not shure why "lcd_gotoxy(0, y)" is neccessary
  lcd_gotoxy(0, y);

  if (iMaxChar && (iMaxChar < iCount))
    iCount = iMaxChar;

  // calculate startposition for text
  uint8_t x((DISPLAY_WIDTH - (iCount * CHAR_WIDTH)));
  // the following 'lcd_command' is similar to 'lcd_gotoxy')
#if defined SSD1306
  uint8_t commandSequence[] = { (uint8_t)(0xb0 + y), 0x21, x, 0x7f };
#elif defined SH1106
  uint8_t commandSequence[] = { (uint8_t)(0xb0 + (y & 0x0f)), 0x21, (uint8_t)(0x00 + ((2 + x) & (0x0f))), (uint8_t)(0x10 + (((2 + x) & (0xf0)) >> 4)), 0x7f };
#endif
  lcd_command(commandSequence, sizeof(commandSequence));
  return true;
}
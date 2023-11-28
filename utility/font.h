/*
 *  font.h
 *  i2c
 *
 *  Created by Michael Köhler on 13.09.18.
 *  Copyright 2018 Skie-Systems. All rights reserved.
 *
 */
#ifndef _font_h_
#define _font_h_
#include <avr/pgmspace.h>

extern const uint8_t ssd1306oled_font[][6] PROGMEM;
extern const uint8_t special_char[][2] PROGMEM;

#endif

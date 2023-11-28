/*********************

Example code for the OLEDPanel Library

This code displays text on the shield, and also reads the buttons on the keypad.

**********************/

#define PCF8574_ADDR 0x21 << 1

// include the library code:
#include <OLEDPanel.h>
OLEDPanel oled = OLEDPanel();

void setup() {
  // Debugging output
  Serial.begin(57600);

  if(oled.detect_i2c(PCF8574_ADDR) != 0)
    Serial.println("OLED-Panel missing...");
    
  // init pcf8574: 
  oled.begin();

  // Print a message to the OLED. We track how long it takes since
  oled.setCursor(0,0);
  int time = millis();
  oled.print("Hello, world!");
  time = millis() - time;
  Serial.print("Took "); Serial.print(time); Serial.println(" ms");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  oled.setCursor(0, 1);
  // print the number of seconds since reset:
  oled.print(millis()/1000);

  uint8_t buttons = oled.readButtons();

  if (buttons) {
    oled.setCursor(0,2);
    if (buttons & BUTTON_UP) {
      oled.print("UP    ");
    }
    if (buttons & BUTTON_DOWN) {
      oled.print("DOWN  ");
    }
    if (buttons & BUTTON_LEFT) {
      oled.print("LEFT  ");
    }
    if (buttons & BUTTON_RIGHT) {
      oled.print("RIGHT ");
    }
    if (buttons & BUTTON_SELECT) {
      oled.print("SELECT");
    }
  }
}

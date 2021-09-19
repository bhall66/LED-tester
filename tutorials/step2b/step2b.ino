 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   19 Sep 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display module
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   LED Tester, step 2b
                Display text on the OLED display
                Demonstrating use of setTextSize, print, etc.

                Make 4 wire connections between the two devices:
                Connect XIAO pin D4 to display SDA
                Connect XIAO pin D5 to display SCL
                Connect XIAO 3v3 to display VCC
                Connect XIAO Gnd to display GND 

                Check the I2C address of your device.
                For mine, the board documentation was wrong!
                SCREEN_ADDRESS 0x3C works for my board
  
 **************************************************************************/


#include <Wire.h>                   // built-in I2C library
#include <Adafruit_GFX.h>           // Adafruit graphics library
#include <Adafruit_SSD1306.h>       // Adafruit OLED library

#define SCREEN_WIDTH 128            // OLED display width, in pixels
#define SCREEN_HEIGHT 64            // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C         // OLED display I2C address, in hex

Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


void drawChars() {                  // draw a screenful of tiny ASCII characters
  led.clearDisplay();               // start with blank screen
  led.setTextSize(1);               // smallest font
  led.setCursor(0,0);               // start at top,left corner
  for (uint8_t i=0; i < 168; i++) { // for each of the 168 characters:
    if (i == '\n') continue;        // skipping the newline character
    led.write(i);                   // add the character to the screen
    if ((i > 0) && (i % 21 == 0))   // start a new line every 21 characters
      led.println();
  }    
  led.display();                    // when done, update the screen
}

void drawText() {
  led.clearDisplay();               // start with a blank screen
  led.setTextSize(1);               // in the smallest font
  led.setCursor(0,0);               // begin in top,left corner
  led.println("Testing LED at 5.2v");
  led.println();    
  
  led.setTextSize(2);               // in medium size font
  led.println("R   2000");          // write a few lines
  led.println("Vf  1.9");           // that look like an LED tester
  led.println("I   7.3");           // but its not, just static text!
  led.display();                    // update the screen
}

void setup()   {
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  led.setTextColor(WHITE,BLACK);
}

void loop() {
  drawChars();                      // fill screen with small ASCII characters
  delay(2000);                      // wait 2 seconds
  drawText();                       // draw static text on display
  delay(2000);                      // wait 2 seconds
}

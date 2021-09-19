 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   14 Sep 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, MCP4261
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   LED Tester, step 7
                Measuring LED voltage and current with series 100-ohm resistor.

                The rotary encoder has 5 pins: 3 on one side and 2 on the other
                For the 3 pin side, connect the center pin to ground.
                and the ouside pins to XIAO D0 and D1.

                For the MCP4261, make the following connections:
                Pin 1  (CS)  to XIAO D7
                Pin 2  (SCK) to XIAO D8 (SCK)
                Pin 3  (SDI) to XIAO D10 (MOSI)
                Pin 4  (Vss) to GND
                Pin 5  (P1A) to Pin 8
                Pin 6  (P1A) to Pin 9
                Pin 8  (P0A) to +3.3V
                Pin 9  (P0W) to 100-OHM resistor
                Pin 14 (Vdd) to +3.3V

                For the LED circuit:
                LED anode to other end of 100-ohm resistor
                LED cathode to GND.
                LED anode to XIAO pin 3 (V_LO)
                MXP4362 pin 9 to XIAO pin 2 (V_HI)
 
 **************************************************************************/
 
#include <SPI.h>                                   // Arduino SPI library
#include <Wire.h>                                  // Arduino I2C library
#include <Adafruit_GFX.h>                          // Adafruit graphics library
#include <Adafruit_SSD1306.h>                      // Adafruit OLED library
#include <RotaryEncoder.h>                         // Matthias Hertel version

#define ENCODER_A            0                     // Connect encoder to MCU pin D0
#define ENCODER_B            1                     // Connect encoder to MCU pin D1
#define VHI_PIN              2                     // voltage input above 100-ohm resistor
#define VLO_PIN              3                     // voltage input below 100-ohm resistor
#define DIGIPOT_CS           7                     // CS pin on MCP4161 digital potentiometer
#define R_WIPER             65                     // resistance of the pot wiper.
#define SCREEN_WIDTH       128                     // OLED display width, in pixels
#define SCREEN_HEIGHT       64                     // OLED display height, in pixels
#define SCREEN_ADDRESS    0x3C                     // OLED display I2C address

Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
RotaryEncoder  encoder(ENCODER_A, ENCODER_B);      // instantiate encoder object

void showValue(int value) {
  led.clearDisplay();                              // erase display contents
  led.setCursor(0,0);                              // pick a spot on the display
  led.print(value);                                // write integer value to display
  led.display();                                   // and show it
}

void writeDP(byte cmd, byte data) {                // write 2 bytes: 1 command byte & 1 data byte
  digitalWrite(DIGIPOT_CS,LOW);                    // enable device write
  SPI.transfer(cmd);                               // send the command byte
  SPI.transfer(data);                              // then send the data byte
  digitalWrite(DIGIPOT_CS,HIGH);                   // disable device write  
}

void setWiper(byte value) {                        // set both digipot wipers to a specified value
  writeDP(0x00,value);                             // set pot#0
  writeDP(0x10,value);                             // set pot#1
}

void setResistance (int r) {                       // set pot to a given resistance
   float pos = (r-R_WIPER)/19.53;                  // calculate wiper position from value
   if (pos<0) pos=0;                               // we can't go below zero ohms
   setWiper(round(pos));                           // set the wiper position
   showValue(r);                                   // show resistance on LED display
}

float getVoltage(int pin) {                        // returns voltage on an input pin, 3.3V max
  int data = analogRead(pin);                      // get analog input, returns range 0..1023
  return (3.3*data)/1023;                          // convert this value to voltage
}

void showResults() {
  float vHi = getVoltage(VHI_PIN);
  float vLo = getVoltage(VLO_PIN);
  float current = (vHi - vLo)*10;                  // calculate current in mA (A * 1000);
  led.setCursor(0,30);  led.print(vHi);            // show V_HI
  led.setCursor(0,50);  led.print(vLo);            // show V_LO
  led.setCursor(64,50); led.print(current);        // show current
  led.display();
}

void setup()   {              
  SPI.begin();                                     // initialize SPI communication
  pinMode(DIGIPOT_CS,OUTPUT);                      // use D7 as DigiPot chip select
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); // initialize OLED display
  led.setTextSize(2);                              // with readable digits  
  led.setTextColor(WHITE,BLACK);                   // monochrome, of course
  encoder.setPosition(20);                         // start at 20*100 = 2K resistance
}

void loop() {                                      
  static int oldPos = 0;                           // previous position
  static int timer = millis();                     // initialize 1 second timer
  encoder.tick();                                  // keep encoder states current
  int pos = encoder.getPosition();                 // get current encoder position
  if (pos != oldPos) {                             // hsa encoder moved?
    setResistance(pos*100);                        // set resistance in 100-ohm steps
    oldPos = pos;                                  // and remember it
  }
  if ((millis()-timer)>1000) {                     // did 1 second elapse?
    showResults();                                 // show V_HI, V_LO, and current
    timer = millis();                              // reset timer
  } 
}

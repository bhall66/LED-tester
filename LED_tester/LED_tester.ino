 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   04 Oct 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, MCP4261
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   LED Tester.  
                See w8bh.net for tutorial, schematic, and builder's guide
 
 **************************************************************************/
 
#include <SPI.h>                                   // Arduino SPI library
#include <Wire.h>                                  // Arduino I2C library
#include <Adafruit_GFX.h>                          // Adafruit graphics library
#include <Adafruit_SSD1306.h>                      // Adafruit OLED library
#include <RotaryEncoder.h>                         // Matthias Hertel version

#define VOLTAGE_DIVIDER   true                     // if true, using voltage/2 on inputs

#define ENCODER_A            0                     // Connect encoder to MCU pin D0
#define ENCODER_B            1                     // Connect encoder to MCU pin D1
#define VHI_PIN              2                     // voltage input above 100-ohm resistor
#define VLO_PIN              3                     // voltage input below 100-ohm resistor
#define VCC_PIN              6                     // voltage supplied to LED circuit
#define DIGIPOT_CS           7                     // CS pin on MCP4161 digital potentiometer

#define R_WIPER             65                     // resistance of the pot wiper.
#define SCREEN_WIDTH       128                     // OLED display width, in pixels
#define SCREEN_HEIGHT       64                     // OLED display height, in pixels
#define SCREEN_ADDRESS    0x3C                     // OLED display I2C address

Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
RotaryEncoder  encoder(ENCODER_A, ENCODER_B);      // instantiate encoder object


//===============================================  ANALOG CODE ============================

// The MCP4261 digital potentiometer contains two 10K pots.  
// In this application they are wired in parallel, so the maxiumum resistance = 5K.
//
// There are 256 wiper positions or steps.
// Therefore, each wiper step = 5000 ohms / 256 steps = 19.53 ohms
//
//          digipot
//   Vcc ---/\/\/\/-----0---> vHi        There are 2 resistors, the digipot and a 100-ohm
//                      |                fixed resistor, in serial with the LED.
//                      |                Therefore, Rtotal = Rpot + 100.
//                  (100 ohm)
//                      |                The voltage drop across the 100 ohm resistor
//                      |                Vr = vHi - vLo.
//                      0---> vLo
//                      |                The current across the 100-ohm resistor is:
//                      |                I = Vr / R = (vHi - vLo)/100
//                    (LED)
//                      |                The current throught the LED is the same as the 
//                      |                current through the resistor.
//                     ///    GND
//
// rVal is an array, containing values for 15 common LED current-limiting resistors

const int rVal[] = {220,270,330,390,470,560,680,820,1000,1200,1500,2200,2700,3300,4700};

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

float getVoltage(int pin) {                        // returns voltage on an input pin, 3.3V max
  int data = analogRead(pin);                      // get analog input, returns range 0..1023
  if (VOLTAGE_DIVIDER) data *= 2;                  // correct for input voltage divider 
  return (3.3*data)/1023;                          // convert to voltage.
}

void setResistance (int r) {                       // set pot to given resistance (TERMINAL A to WIPER)
  float pos = (r-R_WIPER-100)/19.53;               // calculate wiper pos, accounting for 100-ohm R
  setWiper(256-round(pos));                        // set wiper position. For TERMINAL B, omit "256-".
}


//===============================================  DISPLAY CODE ============================
#define ROW0  0                                    // y-coordinate of top row
#define ROW1 14                                    // y-coordinate of first data row
#define ROW2 32                                    // y-coordinate of second data row
#define ROW3 50                                    // y-coordinate of third data row

void drawFloat(float f, int x, int y) {            // draw floating point # at (x,y)
  led.setCursor(x,y); led.print(f); 
}

void drawText(char* txt, int x, int y) {           // draw string at (x,y)
  led.setCursor(x,y); led.print(txt);  
}

void drawInt(int i, int x, int y) {                // draw integer # at (x,y)
  led.setCursor(x,y); led.print(i);
  led.print(" ");
}

void showResistance (int r) {                      // show resistance value on display
   drawInt(r,60,ROW1);                             // put value in row #1
   led.display();                                  // immediately
}

void showResults() {
  float vCC = getVoltage(VCC_PIN);                 // measure the digipot input voltage
  float vHi = getVoltage(VHI_PIN);                 // measure vHi: above 100 ohm resistor
  float vLo = getVoltage(VLO_PIN);                 // measure vLo: below 100 ohm resistor
  float current = abs(vHi-vLo)*10;                 // calculate I=Vr/R = dV/100 = dV*10 (in mA) 
  led.fillRect(60,30,68,34,BLACK);                 // erase previous values
  led.setTextSize(1);
  drawFloat(vCC,90,ROW0); led.write('v');          // display LED testing voltage in small font
  led.setTextSize(2); 
  if (vLo < 0.1) {                                 // detect "short" condition = no Vf
    drawText("----",60,ROW2);
    drawText("Shrt",60,ROW3);
  } else if ((vCC-vLo) < 0.2) {                    // detect "No LED" condition = no current
    drawText("-NO-",60,ROW2);
    drawText("LED!",60,ROW3);
  } else {                                         // display voltage and current:
    drawFloat(vLo,60,ROW2);                        // LED Vf
    drawFloat(current,60,ROW3);                    // LED current
  }
  led.display(); 
}

void initScreen() {
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); // initialize OLED display
  led.clearDisplay();                              // start fresh!
  led.setTextColor(WHITE,BLACK);                   // monochrome display
  led.setTextSize(1);                              // in small font,
  led.setCursor(0,ROW0);   
  led.print("Testing LED at ");                    // text for testing Vcc.
  led.drawLine(0,9,128,9,WHITE);                   // draw a horizontal line.
  led.setTextSize(2);                              // in medium font,
  led.setCursor(0,ROW1);  led.print("R");          // show resistance,
  led.setCursor(0,ROW2);  led.print("Vf");         // then forward voltage,
  led.setCursor(0,ROW3);  led.print("mA");         // then current labels
  led.display();                                   // update display 
}


//===============================================  MAIN PROGRAM ============================

void isr() {                                       // Encoder Interrupt Service Routine:
  encoder.tick();                                  // on interrupt, check encoder state
}

void initEncoder() {                               // enable encoder interrupts
  attachInterrupt(digitalPinToInterrupt            // (More responsive than polling)
    (ENCODER_A), isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt
    (ENCODER_B), isr, CHANGE);  
}

void flashMorseL() {                               // flash LED: dit-dah-dit-dit (Morse "L")
  const int width[] = {1,3,1,1};                   // relative widths: dah = 3 x dit
  const int ditLen = 120;                          // code speed = 1200/ditlen = 10 WPM
  pinMode(LED_BUILTIN,OUTPUT);                     // init LED pin
  for (int i=0; i<4; i++) {                        // for each of the 4 elements:
    digitalWrite(LED_BUILTIN,0);                   // turn on LED
    delay(width[i]*ditLen);                        // keep on for duration of element
    digitalWrite(LED_BUILTIN,1);                   // and turn off LED
    delay(ditLen);                                 // for one dit length  
  }         
}

void setup()   {              
  SPI.begin();                                     // initialize SPI communication
  pinMode(DIGIPOT_CS,OUTPUT);                      // use D7 as DigiPot chip select
  initScreen();                                    // configure screen for showing data
  initEncoder();                                   // enable encoder interrupts
  showResistance(1000);                            // start with resistance of 1K
  setResistance(1000);                             // and set pot to this value
  flashMorseL();                                   // show startup is complete
}

void loop() { 
  static int i = 8;                                // start w/ rVal[8] = 1000 ohms
  static int timer = millis();                     // initialize 1 second timer
  int dir = (int)encoder.getDirection();           // check the encoder for movement
  if (dir!=0) {                                    // did the encoder move?
    i += dir;                                      // step back or forward
    i = constrain(i,0,14);                         // keep resistor index in bounds
    int r = rVal[i];                               // get next resistance value from array
    showResistance(r);                             // display it,
    setResistance(r);                              // and set digipot to this value
  }
  if ((millis()-timer)>1000) {                     // did 1-second timer elapse?
    showResults();                                 // update results once per second
    timer = millis();                              // reset timer
  } 
}

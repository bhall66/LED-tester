 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   14 Sep 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, MCP4261
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   LED Tester, step 4b
                Connect both MCP4261 potentiometers in parallel.
                The output will alternate between 2K and 5K, every 5s. 

                For the MCP4261, make the following connections:
                Pin 1  (CS)  to XIAO D7
                Pin 2  (SCK) to XIAO D8 (SCK)
                Pin 3  (SDI) to XIAO D10 (MOSI)
                Pin 4  (Vss) to GND
                Pin 5  (P1A) to Pin 8
                Pin 6  (P1A) to Pin 9
                Pin 8  (P0A) to Multimeter Black 
                Pin 9  (P0W) to Multimieter Red
                Pin 14 (Vdd) to +5V
 
 **************************************************************************/
 
#include <SPI.h>                                   // use SPI communication with MCP4261
#define DIGIPOT_CS           7                     // CS pin on MCP4261 digital potentiometer
#define R_WIPER             65                     // resistance of the pot wiper.

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
}

void setup()   {              
  SPI.begin();                                     // initialize SPI communication
  pinMode(DIGIPOT_CS,OUTPUT);                      // use D7 as DigiPot chip select
}

void loop() {                                      // set alternating resistances:
  setResistance(2000); delay(5000);                // 2K resistance for 5 seconds, then
  setResistance(5000); delay(5000);                // 5K resistance for 5 seconds
}

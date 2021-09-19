 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   14 Sep 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, MCP4261
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   LED Tester, step 4a
                Using a MCP4261 with a Seeeduino XIAO 

                Make the following connections to the MCP4261:
                Pin 1  (CS)  to XIAO D7
                Pin 2  (SCK) to XIAO D8 (SCK)
                Pin 3  (SDI) to XIAO D10 (MOSI)
                Pin 4  (Vss) to GND
                Pin 8  (P0A) to Multimeter Black 
                Pin 9  (P0W) to Multimeter Red
                Pin 14 (Vdd) to +5V
 
 **************************************************************************/
 
#include <SPI.h>                                   // use SPI communication with MCP4261
#define DIGIPOT_CS           7                     // CS pin on MCP4161 digital potentiometer

void writeDP(byte cmd, byte data) {                // write 2 bytes: 1 command byte & 1 data byte
  digitalWrite(DIGIPOT_CS,LOW);                    // enable device write
  SPI.transfer(cmd);                               // send the command byte
  SPI.transfer(data);                              // then send the data byte
  digitalWrite(DIGIPOT_CS,HIGH);                   // disable device write  
}

void setup()   {              
  SPI.begin();                                     // initialize SPI communication
  pinMode(DIGIPOT_CS,OUTPUT);                      // use D7 as DigiPot chip select
}

void loop() { 
  writeDP(0,0);   delay(5000);                     //  no 39-ohm resistors = 0 ohms
  writeDP(0,128); delay(5000);                     // 128 39-ohm resistors = 5K
  writeDP(0,255); delay(5000);                     // 255 39-ohm resistors = 10K
}

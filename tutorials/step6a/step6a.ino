 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   14 Sep 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, MCP4261
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   LED Tester, step 6a
                Testing a rotary encoder with the XIAO microcontroller.

                Shows encoder position on the Serial monitor. Turn the
                encoder knob, and the position will change accordingly.

                The encoder has 5 pins: 3 on one side and 2 on the other
                For the 3 pin side, connect the center pin to ground.
                and the ouside pins to XIAO D0 and D1.

 **************************************************************************/
 
#include <RotaryEncoder.h>                         // Matthias Hertel version
#define ENCODER_A            0                     // Connect encoder to MCU pin D0
#define ENCODER_B            1                     // Connect encoder to MCU pin D1

RotaryEncoder  encoder(ENCODER_A, ENCODER_B);      // instantiate encoder object

void setup()   {              
  Serial.begin(115200);                            // init serial monitor at 115200 baud
}

void loop() {                                      // set alternating resistances:
  static int oldPos=0;                             // previous position
  encoder.tick();                                  // keep encoder states current
  int pos = encoder.getPosition();                 // get current encoder position
  if (pos!=oldPos) {                               // has encoder moved?
    Serial.println(pos);                           // if so, show new position
    oldPos = pos;                                  // and rememeber it  
  }
}

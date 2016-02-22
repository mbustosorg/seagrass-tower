/*

  ledDriver.h

  Copyright (c) 2016, Mauricio Bustos
  All rights reserved.

  Redistribution and use in source and binary forms, 
  with or without modification, are permitted provided 
  that the following conditions are met:

  - Redistributions must not be for profit.  This software in source or binary form, 
      with or without modification, cannot be included in anyproduct that is then sold.  
      Please contact Mauricio Bustos m@bustos.org if you are interested in a for-profit license.
  - Redistributions of source code or in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ledDriver_h
#define ledDriver_h

#include <stdint.h>

#ifdef NOT_EMBEDDED
// Do Nothing
#elif USE_TCL
#include "TCL.h"
#elif USE_WS2801
// Adafruit RGB strand
#include "Adafruit_WS2801.h"
extern Adafruit_WS2801 strip;
const int lpdDataPin = 2;
const int lpdClockPin = 3;
const int lpdCount = 50;
#else
// Adafruit RGB strip
#include <Arduino.h>
//#include "LPD8806.h"
//extern LPD8806 strip;
const int lpdDataPin = 3;
const int lpdClockPin = 2;
#endif

// LED parameters
#ifdef FS_TOWER_EYE
#define LED_COUNT (100)
#else
#define LED_COUNT (50)
#endif

#define PWM_COUNTER_RESET (2)

class ledDriver {
 public:
  ledDriver();

  uint8_t lowLevelPWMCounter;

  void sendStartFrame();
  void sendEndFrame();
  void sendColor (int pixelIndex, uint8_t red, uint8_t green, uint8_t blue);

  // Definitions for use by the simulator
#ifdef NOT_EMBEDDED
  uint8_t nonEmbedRed[LED_COUNT];
  uint8_t nonEmbedGreen[LED_COUNT];
  uint8_t nonEmbedBlue[LED_COUNT];
#endif

};
#endif

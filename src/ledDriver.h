/*

    Copyright (C) 2016 Mauricio Bustos (m@bustos.org)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

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

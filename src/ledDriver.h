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
#include "opc/opc.h"
#elif USE_TCL
#include "TCL.h"
#elif USE_NEOPIXEL
#include "Adafruit_NeoPixel.h"
#define NEOPIXEL_DATA_PIN (2)
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
#define LED_COUNT (101)
#elif FS_REEDS
#define LED_COUNT (57)
#elif FS_WINDFLOWERS
#define LED_COUNT (250)
#elif FS_DRESS
#define LED_COUNT (46)
#elif FS_ROTOFUEGO
#define LED_COUNT (20)
#elif FS_BUZZ_INN
#define LED_COUNT (50)
#else
#define LED_COUNT (50)
#endif

#define PWM_COUNTER_RESET (2)

class ledDriver {
 public:
  ledDriver();

  uint8_t lowLevelPWMCounter;

  void connect();
  void disconnect();
  void sendStartFrame();
  void sendEndFrame();
  void sendColor (int pixelIndex, uint8_t red, uint8_t green, uint8_t blue);

  // Definitions for use by non-embedded systems
#ifdef NOT_EMBEDDED
  opc_sink opcSink;
  pixel pixels[LED_COUNT];
  
  uint8_t nonEmbedRed[LED_COUNT];
  uint8_t nonEmbedGreen[LED_COUNT];
  uint8_t nonEmbedBlue[LED_COUNT];
#endif

#ifdef USE_NEOPIXEL
  Adafruit_NeoPixel NEOPIXEL = Adafruit_NeoPixel(LED_COUNT, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
#endif

};
#endif

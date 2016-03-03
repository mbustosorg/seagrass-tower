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

#ifndef statusLED_h
#define statusLED_h

#include "Adafruit_NeoPixel.h"

struct led_pulse {
  byte red;
  byte green;
  byte blue; 
  unsigned long duration, start;
  bool fade;
};

#define STATUS_LED_PIN (20)

class statusLED {

 public:
  statusLED();
  void update();
  void pulse(uint8_t red, uint8_t green, uint8_t blue, int duration, bool fade);

 private:
  Adafruit_NeoPixel led = Adafruit_NeoPixel(1, STATUS_LED_PIN, NEO_GRB + NEO_KHZ800);

  led_pulse pulseValue;

};

#endif

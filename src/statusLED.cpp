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

#include "statusLED.h"

//! Create the device object
statusLED::statusLED() {  
  led.begin();
  pulseValue.red = 0;
  pulseValue.green = 0;
  pulseValue.blue = 0;
  pulseValue.duration = 0;
  pulseValue.start = 0;
  pulseValue.fade = false;
}

//! Update the display with current status
void statusLED::update() {
  float proportion = 1.0;
  uint8_t red = 0, green = 0, blue = 0;
  if (millis() - pulseValue.start > pulseValue.duration) {
	proportion = 0.0;
  } else if (pulseValue.fade) {
	proportion = min ((float) (pulseValue.duration - (millis() - pulseValue.start)) / (float) pulseValue.duration, 1.0);
	proportion = max (proportion, 0.0);
  }
  red = pulseValue.red * proportion;
  green = pulseValue.green * proportion;
  blue = pulseValue.blue * proportion;
#ifdef FS_DRESS
  led.setPixelColor(0, red, green, blue);
  led.setPixelColor(1, red, green, blue);
  led.setPixelColor(2, red, green, blue);
  led.setPixelColor(3, red, green, blue);
  led.setPixelColor(4, red, green, blue);
  led.setPixelColor(5, red, green, blue);
  led.setPixelColor(6, red, green, blue);
  led.setPixelColor(7, red, green, blue);
  led.setPixelColor(8, red, green, blue);
  led.setPixelColor(9, red, green, blue);
#else  
  led.setPixelColor(0, red, green, blue);
#endif
  led.show();
}

//! Display a pulse
void statusLED::pulse(uint8_t red, uint8_t green, uint8_t blue, int duration, bool fade) {
  pulseValue.red = red;
  pulseValue.green = green;
  pulseValue.blue = blue;
  pulseValue.duration = duration;
  pulseValue.start = millis();
  pulseValue.fade = fade;  
}

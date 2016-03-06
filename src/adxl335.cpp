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

#include "adxl335.h"
#ifdef NOT_EMBEDDED
#include "embeddedInterface.h"
#include <algorithm>
using namespace std;
#else
#include "Arduino.h"
#endif

//! Create the device object
adxl335::adxl335() {
  range = 3.0; // Default range is +/- 3.0g
}

//! Startup the accelerometer
void adxl335::startup() {
  // Nothing to do
}

//! Normalized read on `pin'
float adxl335::normalizedRead(int pin) {
  float tiltValue;
  tiltValue = analogRead(pin) - 512.0;
  tiltValue = min(tiltValue, TILT_BOUND);
  tiltValue = max (tiltValue, -TILT_BOUND);
  tiltValue = tiltValue * 3300.0 / 1024.0 / 348.0; // 348 mV/g @ 3.3 V 
  return tiltValue;
}

//! X acceleration value
float adxl335::x() {
  return normalizedRead (X_TILT_PIN);
}

//! Y acceleration value
float adxl335::y() {
  return normalizedRead (Y_TILT_PIN);
}

//! Z acceleration value
float adxl335::z() {
  return normalizedRead (Z_TILT_PIN);
}

/*

    Copyright (C) 2019 Mauricio Bustos (m@bustos.org)

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

#include "rhbSignal.h"

float calc_dist(float flat1, float flon1, float flat2, float flon2)
{
  float dist_calc = 0.0;
  float dist_calc2 = 0.0;
  float diflat = 0.0;
  float diflon = 0.0;

  diflat = radians(flat2-flat1);
  flat1 = radians(flat1);
  flat2 = radians(flat2);
  diflon = radians(flon2 - flon1);

  dist_calc = sin(diflat / 2.0) * sin(diflat / 2.0);
  dist_calc2 = cos(flat1);
  dist_calc2 *= cos(flat2);
  dist_calc2 *= sin(diflon/2.0);
  dist_calc2 *= sin(diflon/2.0);
  dist_calc += dist_calc2;

  dist_calc = (2 * atan2(sqrt(dist_calc), sqrt(1.0 - dist_calc)));

  dist_calc *= 6371000.0; //Converting to meters
  //Serial.println(dist_calc);
  return dist_calc;
}

rhbSignal::rhbSignal() {
  latitude = 0;
  longitude = 0;

  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
}

void rhbSignal::initializePattern(uint8_t *data, uint8_t dataLength) {
#ifdef SERIAL_DIAGNOSTICS
  Serial.println("rhbSignal::initializePattern");
#endif
}

void rhbSignal::continuePatternDisplay() {
#ifdef SERIAL_DIAGNOSTICS
  //Serial.println("rhbSignal::continuePatternDisplay");
#endif
}

void rhbSignal::displayData(bool red, bool green, bool blue) {
#ifdef SERIAL_DIAGNOSTICS
  Serial.println("rhbSignal::displayData");
#endif
}

void rhbSignal::setPatternData(uint8_t *data, uint8_t dataLength) {
#ifdef SERIAL_DIAGNOSTICS
  Serial.println("rhbSignal::setPatternData");
#endif
  light = !light;
  digitalWrite(LIGHT_PIN, light);
}

void rhbSignal::setRadioTowerSyncTimestamp(uint32_t timestamp) {
  radioTowerSyncTimestamp = timestamp;
}

void rhbSignal::setFrameNumber(int newFrameNumber) {
  frameNumber = newFrameNumber;
}

void rhbSignal::triggerPatternChange(bool forward) {
  // noop
}

void rhbSignal::advancePatternSpeed(bool continuous, bool up) {
  // noop
}

void rhbSignal::advancePatternIntensity(uint8_t pattern, bool continuous) {
  // noop
}

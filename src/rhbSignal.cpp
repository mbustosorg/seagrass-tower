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

// Max brightness is acheived at around PWM=50

#include "rhbSignal.h"

float calc_dist(float flat1, float flon1, float flat2, float flon2)
// Distance in meters
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

  dist_calc *= 6371000.0; 
  return dist_calc;
}

rhbSignal::rhbSignal() {
  latitude = 0.0;
  longitude = 0.0;

  for (int i = 0; i < DEVICE_COUNT; i++) {
    pinMode(pwmPins[i], OUTPUT);
    analogWrite(pwmPins[i], 0);
  }
}

void rhbSignal::initializePattern(uint8_t *data, uint8_t dataLength) {
#ifdef SERIAL_DIAGNOSTICS
  Serial.println("rhbSignal::initializePattern");
#endif
}

void rhbSignal::continuePatternDisplay() {
  // Update PWM proportion
  for (int i = 0; i < DEVICE_COUNT; i++) {
    float proportion = 1.0;
    if (millis() - pulseStart[i] > pulseDuration[i]) {
      proportion = 0.0;
      if (millis() > pulseStart[i] + 1000 / pulseFrequency[i]) {
	pulseStart[i] = millis();
	proportion = 1.0;
      }
    } else {
      proportion = min ((float) (pulseDuration[i] - (millis() - pulseStart[i])) / (float) pulseDuration[i], 1.0);
    }
    analogWrite(pwmPins[i], int(pwmValues[i] * proportion));
  }
}

void rhbSignal::displayData(bool red, bool green, bool blue) {
#ifdef SERIAL_DIAGNOSTICS
  Serial.println("rhbSignal::displayData");
#endif
}

void rhbSignal::setPatternData(uint8_t *data, uint8_t dataLength) {
  float targetLatitude = 0.0;
  float targetLongitude = 0.0;
  
  memcpy (&targetLatitude, &data[11], sizeof(targetLatitude));
  memcpy (&targetLongitude, &data[15], sizeof(longitude));

  float distance = calc_dist(latitude, longitude, targetLatitude, targetLongitude);

  bool checkTransition = distance < IN_RANGE_DISTANCE;
  
  //if (checkTransition != inRange) {
  //  initialDistanceTimestamp = millis();
  //}

  //if ((millis() - initialDistanceTimestamp) > 30000) {
    if (checkTransition != inRange) {
      inRange = checkTransition;
      if (!inRange) {
	for (int i = 0; i < DEVICE_COUNT; i++) {
	  pulseFrequency[i] = defaultPulseFrequency[i];
	  pulseDuration[i] = defaultPulseDuration[i];
	} 
      } else {
	for (int i = 0; i < DEVICE_COUNT; i++) {
	  pulseStart[i] = millis();
	}
      }
    }
    //}
    if (inRange) {
      // Update frequency as needed when in range
      Serial.print("New Frequency: ");
      float newFrequency = min(2.0, IN_RANGE_DISTANCE / distance * DEFAULT_PULSE_FREQUENCY);
      float newDuration = 1.0 / newFrequency * 500.0;
      Serial.println(newFrequency);
      for (int i = 0; i < DEVICE_COUNT; i++) {
	pulseFrequency[i] = newFrequency;
	pulseDuration[i] = newDuration;
      }
    }
    

#ifdef SERIAL_DIAGNOSTICS
  Serial.println("rhbSignal::setPatternData");
  Serial.print("Self:  Latitude:");
  Serial.print(latitude);
  Serial.print(" Longitude:");
  Serial.println(longitude);
  Serial.print("Targ:  Latitude:");
  Serial.print(targetLatitude);
  Serial.print(" Longitude:");
  Serial.println(targetLongitude);
  Serial.print("Distance: ");
  Serial.println(distance);
  if (inRange) Serial.println("InRange");
  else Serial.println("not InRange");
#endif

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

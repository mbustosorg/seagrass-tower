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

#include "accelerometer.h"
#include <stdlib.h>
#include <math.h>

//! Create the accelerometer object
accelerometer::accelerometer() {
  filterLength = 0;
  shakingMovingAverageLength = 100;
  filteredReadings.x = 0.0;
  filteredReadings.y = 0.0;
  filteredReadings.z = 0.0;
  shakingMovingAverage.x = 0.0;
  shakingMovingAverage.y = 0.0;
  shakingMovingAverage.z = 0.0;
  lastTilt.x = 0.0;
  lastTilt.y = 0.0;
  lastTilt.z = 0.0;
  shakingMovingAverageLength = 30;
}

//! Current moving average tilt vector
TiltVector accelerometer::currentTilt() {
  TiltVector newVector;
  newVector.x = device.x();
  newVector.y = device.y();
  newVector.z = device.z();
  return newVector;
}

//! Set `filterLength' to `newFilterLength'
void accelerometer::setFilterLength(int newFilterLength) {
  if (newFilterLength > 0) {
	filterLength = newFilterLength;
  }
}

//! Moving average of tilt readings
TiltVector accelerometer::filteredTilt() {
  TiltVector newVector = currentTilt();
  filteredReadings.x = filteredReadings.x + (newVector.x - filteredReadings.x) / filterLength;
  filteredReadings.y = filteredReadings.y + (newVector.y - filteredReadings.y) / filterLength;
  filteredReadings.z = filteredReadings.z + (newVector.z - filteredReadings.z) / filterLength;
  return filteredReadings;
}

//! Startup the accelerator
void accelerometer::startup() {
  device.startup();
}

//! Shutdown the acceleromter for powersaving
void accelerometer::shutdown() {
}

//! Have we been recently shaken?
bool accelerometer::isShaking() {
  TiltVector tilt = filteredTilt();
  shakingMovingAverage.x = shakingMovingAverage.x + (fabs(tilt.x - lastTilt.x) - shakingMovingAverage.x) / shakingMovingAverageLength;
  shakingMovingAverage.y = shakingMovingAverage.y + (fabs(tilt.y - lastTilt.y) - shakingMovingAverage.y) / shakingMovingAverageLength;
  shakingMovingAverage.z = shakingMovingAverage.z + (fabs(tilt.z - lastTilt.z) - shakingMovingAverage.z) / shakingMovingAverageLength;
  lastTilt = tilt;
  float totalEnergy = sqrt(shakingMovingAverage.x * shakingMovingAverage.x * 1000000 + shakingMovingAverage.y * shakingMovingAverage.y * 1000000 + shakingMovingAverage.z * shakingMovingAverage.z * 1000000);
  bool shaking = totalEnergy > 30.0;
#ifdef ACCEL_DIAGNOSTICS
  Serial.print ("ACCEL_DATA:");
  Serial.print ("x=");
  Serial.print (shakingMovingAverage.x * 1000);
  Serial.print (",y=");
  Serial.print (shakingMovingAverage.y * 1000);
  Serial.print (",z=");
  Serial.print (shakingMovingAverage.z * 1000);
  Serial.print (",total=");
  Serial.print (totalEnergy);
  if (shaking) Serial.println ("---SHAKING---");
  else Serial.println ("");
#endif
#ifdef NOT_EMBEDDED
  shaking = false;
#endif
  return shaking;
}

//! Calibrate base angle
void accelerometer::calibrate() {
}

//! Force a reset of shaking status
void accelerometer::resetShake() {
  shakeTimeStart = 0;
}

//! Set the shake detection threshold to `newShakeThreshold'
void accelerometer::setShakeThreshold(float newShakeThreshold) {
  shakeThreshold = newShakeThreshold;
}
 
//! Set the amount of time we sit in shake mode
void accelerometer::setResetTime(long newResetTime) {
  shakeResetTime = newResetTime;
}



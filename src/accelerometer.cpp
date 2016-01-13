/*

  accelerometer.cpp

  Copyright (c) 2014, Mauricio Bustos
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

#include "accelerometer.h"
#include <stdlib.h>
#include <math.h>

//! Create the accelerometer object
accelerometer::accelerometer() {
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
  shakingMovingAverage.x = shakingMovingAverage.x + (abs(tilt.x - lastTilt.x) - shakingMovingAverage.x) / shakingMovingAverageLength;
  shakingMovingAverage.y = shakingMovingAverage.y + (abs(tilt.y - lastTilt.y) - shakingMovingAverage.y) / shakingMovingAverageLength;
  shakingMovingAverage.z = shakingMovingAverage.z + (abs(tilt.z - lastTilt.z) - shakingMovingAverage.z) / shakingMovingAverageLength;
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
#ifndef NOT_EMBEDDED
  return shaking;
#else
  return false;
#endif
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



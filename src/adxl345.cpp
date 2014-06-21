/*

  adxl345.cpp

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

#include "adxl345.h"
#include <Wire.h>

//! Create the device object
adxl345::adxl345() {
  range = 2.0; // Default range is +/- 2.0g
}

//! Start the accelerometer
void adxl345::startup() {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.send(ADXL345_POWER_CTL);  
  Wire.send(0x08);  // Measurement mode
  Wire.endTransmission();
}

//! Normalized read for 2 byte set starting at `byteStart'
float adxl345::normalizedRead(int byteStart) {
  int16_t reading;
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.send(byteStart);  
  Wire.endTransmission();
  Wire.requestFrom(ADXL345_ADDR, 2);
  reading = Wire.receive() | Wire.receive() << 8;
  return (float)reading * (range * 2) / pow (2, 10);
}

//! X acceleration value
float adxl345::x() {
  return normalizedRead(ADXL345_DATAX0);
}

//! Y acceleration value
float adxl345::y() {
  return normalizedRead(ADXL345_DATAY0);
}

//! Z acceleration value
float adxl345::z() {
  return normalizedRead(ADXL345_DATAZ0);
}


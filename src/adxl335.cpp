/*

  adxl335.cpp

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

#include "adxl335.h"
#include <Wire.h>

//! Create the device object
adxl335::adxl335() {
  range = 3.0; // Default range is +/- 3.0g
}

//! Normalized read on `pin'
float adxl335::normalizedRead(int pin) {
  float tiltValue;
  tiltValue = analogRead(pin) - 512.0;
  tiltValue = max (min(tiltValue, TILT_BOUND), -TILT_BOUND);
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

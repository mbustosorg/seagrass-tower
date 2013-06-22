//
//  embeddedInterface.cpp
//
//  $Date: 2013-04-11 23:54:33 -0700 (Thu, 11 Apr 2013) $
//  $Rev: 861 $
//  $Author: mauricio $
//
//  Copyright (c) 2012, Mauricio Bustos
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, 
//  with or without modification, are permitted provided 
//  that the following conditions are met:
//
//  - Redistributions must not be for profit.  This software in source or binary form, 
//      with or without modification, cannot be included in anyproduct that is then sold.  
//      Please contact Mauricio Bustos m@bustos.org if you are interested in a for-profit license.
//  - Redistributions of source code or in binary form must reproduce the above copyright notice, 
//      this list of conditions and the following disclaimer in the documentation 
//      and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
//  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "embeddedInterface.h"

#ifdef NOT_EMBEDDED

int xTiltSetting = 512;
int yTiltSetting = 512;
int zTiltSetting = 512;

#include "ofUtils.h"

int analogRead(uint8_t val) {
  if (val == 18) { // xTiltPin
	uint32_t randomValue = random(500, 524);
	return randomValue;
  } else if (val == 17) { // yTiltPin
	uint32_t randomValue = random(500, 524);
	return randomValue;
  } else if (val == 16) { // zTiltPin
	uint32_t randomValue = random(500, 524);
	return randomValue;
  } else {
	return random(1024);
  }
}

uint32_t random(uint32_t val) {
  float result = ofRandom(1.0) * val;
  return (uint32_t) result;
}

uint32_t random(uint32_t val0, uint32_t val1) {
  return (uint32_t) ofRandom((float) val0, (float) val1);
}

void randomSeed(unsigned int val) {
  ofSeedRandom((int) val);
}

uint32_t millis(void) {
  // Add 1 because some patters need a non-zero start time
  return (uint32_t) ofGetElapsedTimeMillis() + 1;
}

void delay(uint32_t val) {
  uint32_t timestamp = millis();
  while (timestamp + val > millis()) {}
}

#endif

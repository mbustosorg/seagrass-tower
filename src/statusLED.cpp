/*

  statusLED.cpp

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
  led.setPixelColor(0, red, green, blue);
  led.show();
}

//! Display a pulse
void statusLED::pulse(byte red, byte green, byte blue, int duration, bool fade) {
  pulseValue.red = red;
  pulseValue.green = green;
  pulseValue.blue = blue;
  pulseValue.duration = duration;
  pulseValue.start = millis();
  pulseValue.fade = fade;  
}

/*
 
 ledDreiver.cpp
 
 Copyright (c) 2016, Mauricio Bustos
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

#include "ledDriver.h"

ledDriver::ledDriver() {
#ifndef NOT_EMBEDDED
#ifdef USE_TCL
    // Do nothing
#elif USE_WS2801
    // Adafruit bulbs
#else
    // Adafruit strip
    pinMode(lpdDataPin, OUTPUT);
    pinMode(lpdClockPin, OUTPUT);
#endif
#endif
    lowLevelPWMCounter = 0;
}

//! Send the start frame to start the display update
void ledDriver::sendStartFrame() {
    lowLevelPWMCounter++;
    if (lowLevelPWMCounter > PWM_COUNTER_RESET) lowLevelPWMCounter = 1;
#ifndef NOT_EMBEDDED
#ifdef USE_TCL
    TCL.sendEmptyFrame();
#elif USE_WS2801
    // Adafruit bulbs
#else
    // Adafruit strip
#endif
#endif
}

//! Send the end frame to complete the display
void ledDriver::sendEndFrame() {
#ifndef NOT_EMBEDDED
#ifdef USE_TCL
    TCL.sendEmptyFrame();
#elif USE_WS2801
    // Adafruit bulbs
    strip.show();
#else
    // Adafruit strip
    digitalWrite(lpdDataPin, LOW);
    int n = ((LED_COUNT + 63) / 64) * 3;
    for(uint16_t i = 8 * n; i > 0; i--) {
        digitalWrite(lpdClockPin, HIGH);
        digitalWrite(lpdClockPin, LOW);
    }
#endif
#endif
}

//! Send a particular color to the whole strand
void ledDriver::sendColor (int pixelIndex, uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t trueRed, trueGreen, trueBlue;
    trueRed = red;
    trueGreen = green;
    trueBlue = blue;
    /*
     if (red < PWM_DIMMER_THRESHOLD && lowLevelPWMCounter > PWM_COUNTER_RESET - PWM_COUNTER_OFFSET) {
     trueRed = 0;
     }
     if (green < PWM_DIMMER_THRESHOLD && lowLevelPWMCounter > PWM_COUNTER_RESET - PWM_COUNTER_OFFSET) {
     trueGreen = 0;
     }
     if (blue < PWM_DIMMER_THRESHOLD && lowLevelPWMCounter > PWM_COUNTER_RESET - PWM_COUNTER_OFFSET) {
     trueBlue = 0;
     }
     */
#ifdef NOT_EMBEDDED
    nonEmbedRed[pixelIndex] = trueRed;
    nonEmbedGreen[pixelIndex] = trueGreen;
    nonEmbedBlue[pixelIndex] = trueBlue;
#else
#ifdef USE_TCL
    TCL.sendColor (trueRed, trueGreen, trueBlue);
#elif USE_WS2801
    // Adafruit bulbs
    strip.setPixelColor (pixelIndex, trueRed, trueGreen, trueBlue);
#else
    // Adafruit strip
    uint8_t greenByte = trueGreen >> 1;
    uint8_t redByte = trueRed >> 1;
    uint8_t blueByte = trueBlue >> 1;
    for (uint8_t bit = 0x80; bit; bit >>= 1) {
        if ((greenByte | 0x80) & bit) {
            digitalWrite(lpdDataPin, HIGH);
        } else {
            digitalWrite(lpdDataPin, LOW);
        }
        digitalWrite(lpdClockPin, HIGH);
        digitalWrite(lpdClockPin, LOW);
    }
    for (uint8_t bit = 0x80; bit; bit >>= 1) {
        if ((redByte | 0x80) & bit) {
            digitalWrite(lpdDataPin, HIGH);
        } else {
            digitalWrite(lpdDataPin, LOW);
        }
        digitalWrite(lpdClockPin, HIGH);
        digitalWrite(lpdClockPin, LOW);
    }
    for (uint8_t bit = 0x80; bit; bit >>= 1) {
        if ((blueByte | 0x80) & bit) {
            digitalWrite(lpdDataPin, HIGH);
        } else {
            digitalWrite(lpdDataPin, LOW);
        }
        digitalWrite(lpdClockPin, HIGH);
        digitalWrite(lpdClockPin, LOW);
    }
#endif
#endif
}



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

#include "ledDriver.h"

#ifdef NOT_EMBEDDED

ledDriver::ledDriver() {
#ifndef NOT_OPC
    opcSink = -1;
#endif
    connect();
    lowLevelPWMCounter = 0;
}

//! Connect to the LED Server
void ledDriver::connect() {
#ifndef NOT_OPC
    if (opcSink < 0 || opcSink > OPC_MAX_SINKS) {
        char opc_target[] = "127.0.0.1:7890";
        opcSink = opc_new_sink(opc_target);
    }
    opc_connect(opcSink, 1000);
#endif
}

//! Disconnect from the LED Server
void ledDriver::disconnect() {
#ifndef NOT_OPC
    opc_close(opcSink);
#endif
}

//! Send the start frame to start the display update
void ledDriver::sendStartFrame() {
    lowLevelPWMCounter++;
    if (lowLevelPWMCounter > PWM_COUNTER_RESET) lowLevelPWMCounter = 1;
}

//! Send the end frame to complete the display
void ledDriver::sendEndFrame() {
  for (int i = 0; i < LED_COUNT; i++) {
#ifdef FS_REEDS
    pixels[i].r = nonEmbedBlue[i];
    pixels[i].g = nonEmbedRed[i];
    pixels[i].b = nonEmbedGreen[i];
#elif FS_TOWER_EYE
    if (i < LED_COUNT - 1) {
      pixels[i + 1].r = nonEmbedRed[i];
      pixels[i + 1].g = nonEmbedGreen[i];
      pixels[i + 1].b = nonEmbedBlue[i];
    }
    pixels[0].r = 255;
    pixels[0].g = 0;
    pixels[0].b = 0;
#else
    pixels[i].r = nonEmbedRed[i];
    pixels[i].g = nonEmbedGreen[i];
    pixels[i].b = nonEmbedBlue[i];
#endif
  }
#ifndef NOT_OPC
  if (opc_connected(opcSink)) opc_put_pixels(opcSink, 0, LED_COUNT, pixels);
#endif
}

//! Send a particular color to the whole strand
void ledDriver::sendColor (int pixelIndex, uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t trueRed, trueGreen, trueBlue;
    trueRed = red;
    trueGreen = green;
    trueBlue = blue;
    nonEmbedRed[pixelIndex] = trueRed;
    nonEmbedGreen[pixelIndex] = trueGreen;
    nonEmbedBlue[pixelIndex] = trueBlue;
}
#else // #ifdef NOT_EMBEDDED
ledDriver::ledDriver() {
#ifdef USE_TCL
    TCL.begin();
#elif USE_NEOPIXEL
    NEOPIXEL.begin();
#else
    // Adafruit strip
    pinMode(lpdDataPin, OUTPUT);
    pinMode(lpdClockPin, OUTPUT);
#endif
    lowLevelPWMCounter = 0;
}

//! Connect to the LED Server
void ledDriver::connect() {
}

//! Disconnect from the LED Server
void ledDriver::disconnect() {
}

//! Send the start frame to start the display update
void ledDriver::sendStartFrame() {
    lowLevelPWMCounter++;
    if (lowLevelPWMCounter > PWM_COUNTER_RESET) lowLevelPWMCounter = 1;
#ifdef USE_TCL
    TCL.sendEmptyFrame();
#else
    // Adafruit strip & Neopixel
#endif
}

//! Send the end frame to complete the display
void ledDriver::sendEndFrame() {
#ifdef USE_TCL
    TCL.sendEmptyFrame();
#elif USE_NEOPIXEL
    NEOPIXEL.show();
#else
    // Adafruit strip
    digitalWrite(lpdDataPin, LOW);
    int n = ((LED_COUNT + 63) / 64) * 3;
    for(uint16_t i = 8 * n; i > 0; i--) {
        digitalWrite(lpdClockPin, HIGH);
        digitalWrite(lpdClockPin, LOW);
    }
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
#ifdef USE_TCL
    TCL.sendColor (trueRed, trueGreen, trueBlue);
#elif USE_NEOPIXEL
    NEOPIXEL.setPixelColor(pixelIndex, trueRed, trueGreen, trueBlue);
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
}
#endif // #ifdef NOT_EMBEDDED

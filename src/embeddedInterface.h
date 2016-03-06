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

#ifndef embeddedInterface_h
#define embeddedInterface_h

#ifdef NOT_EMBEDDED

#include <stdint.h>

// Prototype to divorce us from Arduino.h
#ifdef __cplusplus
extern "C"{
#endif
int analogRead(uint8_t);
#ifdef __cplusplus
} // extern "C"
#endif

// Definitions for use by the simulator

extern int xTiltSetting;
extern int yTiltSetting;
extern int zTiltSetting;

#define LOW (0)
#define HIGH (0)

unsigned long rtc_get();
uint32_t random(uint32_t);
uint32_t random(uint32_t, uint32_t);
void randomSeed(unsigned int);
uint32_t millis(void);
void delay(uint32_t);
uint8_t digitalRead(uint8_t);
void digitalWrite(uint8_t, uint8_t);

float max(float, float);
float min(float, float);

#endif

#endif

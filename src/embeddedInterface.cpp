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

#include "embeddedInterface.h"
#include <cstdlib>
#include <sys/time.h>
#include <ctime>
#include <stdio.h>
#include <algorithm>

int xTiltSetting = 512;
int yTiltSetting = 512;
int zTiltSetting = 512;

static long startTime = 0;

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

unsigned long rtc_get() {
  return millis() / 1000;
}

uint32_t random(uint32_t val) {
  randomSeed(0);
  return rand() % val;
}

uint32_t random(uint32_t val0, uint32_t val1) {
  randomSeed(0);
  return rand() % (val1 - val0) + val0;
}

void randomSeed(unsigned int val) {
  srand (time(NULL));
}

uint32_t millis(void) {
  // Add 1 because some patters need a non-zero start time
  timeval tv;
  gettimeofday(&tv, 0);
  if (startTime == 0) startTime = tv.tv_sec * 1000;
  //printf("%d\n", tv.tv_sec * 1000 + tv.tv_usec / 1000 - startTime);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000 + 1 - startTime;
}

void delay(uint32_t val) {
  uint32_t timestamp = millis();
  while (timestamp + val > millis()) {}
}

float max(float first, float second) {
    return std::max(first, second);
}

float min(float first, float second) {
    return std::min(first, second);
}


/*

    Copyright (C) 2019 Mauricio Bustos (m@bustos.org)

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

#ifndef rhbSignal_h
#define rhbSignal_h

#include "accelerometer.h"
#include "towerAnimations.h"
#include <XBee.h>

#ifdef NOT_EMBEDDED
typedef float float32_t;
#include <math.h>
#include <memory.h>
#else
#define ARM_MATH_CM4
#include "arm_math.h"
#endif

#define LED_COUNT (1)

#define FRAME_RATE (60)
#define FFT_LEN (256)
#define TEST_LENGTH_SAMPLES (2 * FFT_LEN)

#define LIGHT_PIN (31)

typedef struct {
  unsigned long hours;
  unsigned long minutes;
  unsigned long seconds;
} timeStruct;

class rhbSignal {

 public:

  rhbSignal();

  bool light = false;
  
  // Redefinitions
  void initializePattern(uint8_t *data, uint8_t dataLength);
  void continuePatternDisplay();
  void displayData(bool red, bool green, bool blue);
  void setPatternData(uint8_t *data, uint8_t dataLength);
  void setRadioTowerSyncTimestamp(uint32_t timestamp);
  void setFrameNumber(int newFrameNumber);
  void triggerPatternChange(bool forward);
  void advancePatternSpeed(bool continuous, bool up);
  void advancePatternIntensity(uint8_t pattern, bool continuous);

  uint16_t failedMessageCount;
  uint8_t pattern;
  bool isShaking;
  uint8_t patternSpeed;
  timeStruct clock;
  uint32_t radioTowerSyncTimestamp;
  int frameNumber;

  // GPS data
  int32_t latitude;
  int32_t longitude;

  // Accelerometer data
  accelerometer accel;

  // Animation data
  towerAnimations animations;

  // Redefinitions for framework
  uint8_t ledRed[LED_COUNT];
  uint8_t ledGreen[LED_COUNT];
  uint8_t ledBlue[LED_COUNT];
  float FS_BREATHE_MULTIPLIER;
  int randomSeedPin;
  uint8_t audioAnalogPin;
  unsigned long heartbeatPumpShortStart;
  unsigned long heartbeatPumpShortPeriod;
  unsigned long heartbeatPumpLongStart;
  uint8_t heartbeatIntensity;
  uint32_t audioSampleInputIndex;
  float32_t audioSampleInput[2 * 512];
  uint8_t characterIndex;
  uint16_t frameIndex;
  
};

#endif

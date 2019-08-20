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

typedef struct {
  unsigned long hours;
  unsigned long minutes;
  unsigned long seconds;
} timeStruct;

#define DEVICE_COUNT (3)
#define DEFAULT_PULSE_FREQUENCY (0.30)
#define DEFAULT_PULSE_DURATION (1500.0)
#define IN_RANGE_DISTANCE (2000.0)

#define TEST_LENGTH_SAMPLES (1)
#define FRAME_RATE (60)

class rhbSignal {

 public:

  rhbSignal();

  int pwmPins[DEVICE_COUNT] = {20, 21, 22}; //, 23, 25, 32};
  int pwmValues[DEVICE_COUNT] = {50, 50, 50}; //, 0, 0, 0};
  float pulseDuration[DEVICE_COUNT] = {}; //, 1000.0, 1000.0, 5000.0};
  float defaultPulseDuration[DEVICE_COUNT] = {DEFAULT_PULSE_DURATION, DEFAULT_PULSE_DURATION, DEFAULT_PULSE_DURATION};
  float pulseFrequency[DEVICE_COUNT] = {DEFAULT_PULSE_FREQUENCY,
					DEFAULT_PULSE_FREQUENCY + 0.01,
					DEFAULT_PULSE_FREQUENCY + 0.02}; //, 0.25, 0.25, 0.25};
  float defaultPulseFrequency[DEVICE_COUNT] = {DEFAULT_PULSE_FREQUENCY,
					       DEFAULT_PULSE_FREQUENCY + 0.01,
					       DEFAULT_PULSE_FREQUENCY + 0.02}; //, 0.25, 0.25, 0.25};
  long pulseStart[DEVICE_COUNT] = {0, 0, 0}; //, 0, 0, 0};
  
  bool light = false;
  long initialDistanceTimestamp = 0;
  bool inRange = false;
  
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
  float latitude;
  float longitude;

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

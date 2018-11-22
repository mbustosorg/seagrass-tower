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

#ifndef furSwarmPatterns_h
#define furSwarmPatterns_h

#include <stdint.h>
#include "colorUtilities.h"
#include "ledDriver.h"

typedef struct {
  uint8_t red;       
  uint8_t green;      
  uint8_t blue;       
} ledColorSpec;

typedef struct {
  unsigned long hours;
  unsigned long minutes;
  unsigned long seconds;
} timeStruct;

extern const uint8_t memberType;

#define IN_OUT_BREAK_POINT (150)
#define PUMP_UPPER_LEVEL (150)
#define PUMP_LOWER_LEVEL (10)
#define ORGANIC_UPPER_LEVEL (1)
#define STARFIELD_UPPER_LEVEL (200)
#define PWM_COUNTER_OFFSET (1) // 80% Duty cycle: 1 - PWM_COUNTER_OFFSET / PWM_COUNTER_RESET
#define PWM_DIMMER_THRESHOLD (3) // Dim values lower than this based on above duty cycle

#define MAX_DATA_LENGTH (10) // Maximum expected number of bytes in incoming message
#define FS_DELAY_FACTOR (500)
#define PATTERN_START_MOD (5) // Second interval to start patterns for synchronization
#define SMOOTH_FACTOR_FLOAT (8.0)
#define SMOOTH_FACTOR_INT (8)

class furSwarmPatterns {
 public:
  furSwarmPatterns();

  ledDriver leds;
  float FS_BREATHE_MULTIPLIER;
  uint8_t pattern;
  bool transitionRequested;
  uint8_t triggerPattern;
  uint16_t failedMessageCount;
  uint8_t lastData[MAX_DATA_LENGTH];
  uint8_t lastDataLength;
  unsigned long delayStopwatch;
  int secondModStart; 
  uint8_t lastDelayFactor;
  timeStruct clock;

  int randomSeedPin;
  unsigned long secondsIntoMinute;
  void setSecondsIntoMinute(unsigned long newSecondsIntoMinute);
  void iterateForTransition();
  uint8_t timeToDrop;
  uint16_t cycleSpot;
    
  uint8_t blendingLedRed[LED_COUNT];
  uint8_t blendingLedGreen[LED_COUNT];
  uint8_t blendingLedBlue[LED_COUNT];
  long blendingStart;
  
  uint8_t ledRed[LED_COUNT];
  uint8_t ledGreen[LED_COUNT];
  uint8_t ledBlue[LED_COUNT];
  uint8_t ledCycleColor[LED_COUNT];
  uint8_t ledRedDirection[LED_COUNT];
  uint8_t ledGreenDirection[LED_COUNT];
  uint8_t ledBlueDirection[LED_COUNT];
  uint8_t breatheBorderRed[LED_COUNT];
  uint8_t breatheBorderGreen[LED_COUNT];
  uint8_t breatheBorderBlue[LED_COUNT];
  uint32_t flashStartTime; 
  uint8_t ledChangeRate;
  uint8_t flashLedChangeRate;

  uint8_t lastLedRed[LED_COUNT];
  uint8_t lastLedGreen[LED_COUNT];
  uint8_t lastLedBlue[LED_COUNT];
  
  unsigned long heartbeatPumpShortStart;
  unsigned long heartbeatPumpShortPeriod;
  unsigned long heartbeatPumpLongStart;
  uint8_t breatheUpperLevel;
  uint8_t heartbeatIntensity;
  uint8_t patternSpeed;
  uint8_t patternSpeedLevel;
  bool patternForward;
  uint8_t characterIndex; // Index of currently displayed character
  uint8_t characterIndexUpper; // Count of characters available in PROGMEM
  uint16_t frameIndex; // Index for frame based patterns
  uint16_t frameRelease; // Counter to aid with pattern iteration speed
  uint8_t redLevel;
  uint8_t greenLevel;
  uint8_t blueLevel;
  uint8_t intensityLevel;
  float prismHue;
  float prismValue;
 
  // Searching eye pattern variables
  uint16_t maxEye, minEye;

  // Sound Activate pattern variables
  uint8_t audioAnalogPin;
  long saThreshold;
  float saTargetThreshold;
  bool saHighResetThreshold;
  bool saMidResetThreshold;
  bool saLowResetThreshold;
  // Sound Activate - Gain
  float saGain;
  // Sound Activate - Moving Average
  float saMovingAverage;
  // Sound Activate - Running Average
  float saRunningAverage;
  #define SA_RUNNING_AVG_HISTORY 5
  float saRunningAverageHistory[SA_RUNNING_AVG_HISTORY];
  uint8_t saRunningAveragePosition = 0;
 
  int saNumberOfSamples; // Higher to make running average more peak'y
  float saMovingAverageCount;
  uint8_t saAveragedOver; // Lower to make running average more peak'y
  uint8_t previousSoundLevel;
  uint8_t peakSoundLevel;
  uint32_t soundLevelRetractionTime;
  uint32_t peakRetractionTime;
 
  virtual void initializePattern(uint8_t *data, uint8_t dataLength);
  virtual void continuePatternDisplay();
  void setPatternData(uint8_t *data, uint8_t dataLength);
  virtual void checkLatestData();
  void setPatternSpeedWithFactor(int factor);
  void sendStartFrame();
  void sendEndFrame();
  void sendColor (int pixelIndex, uint8_t red, uint8_t green, uint8_t blue);
  // Pattern function prototypes
  void setfullStrand(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue, bool smooth);
  void descendData(int changeRate);
  void cycleData(int changeRate, bool upperAdjust, uint8_t stayBelow, uint8_t stayAbove);
  void cycleDataScaled(int changeRate, bool upperAdjust, uint8_t stayBelow, uint8_t stayAbove);
  void cycleDataColor(uint8_t ledColor[], uint8_t ledDirection[], uint8_t ledBorder[], int ledIndex, uint8_t step, bool upperAdjsut, uint8_t stayBelow, uint8_t stayAbove);
  void setColorScaled(uint8_t ledSourceColor[], uint8_t ledTargetColor[], uint8_t scaledLevel, int index);
  void descendPumpData(int step);
  void descendPumpDataScaled(int step);
  void displayData(bool red, bool green, bool blue);
  void displayDataWithOverlay(bool red, bool green, bool blue,
			      uint8_t redOverlay, uint8_t greenOverlay, uint8_t blueOverlay);
  void displayPumpData(bool red, bool green, bool blue);
  void initializeRandom(uint8_t low, uint8_t high, uint8_t red, uint8_t green, uint8_t blue, bool single);
  void initializeHeartbeatPattern(uint8_t red, uint8_t green, uint8_t blue);
  void initializeCylon(uint8_t red, uint8_t green, uint8_t blue, bool twoSided);
  int cylonIndexMap (int i, int frameIndex);
  void fillMatrix(int ledPosition, int framePosition, float proportion, bool accum);
  void iterateFrameStep(uint8_t upperFrameNumber);
  float incrementalDegree (float input);
  void displayCylon();
  void initializeDrop();
  void displayDrop();
  void initializeCharacter(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue);
  void displayBlank();
  void initializeBitmapPattern(uint8_t red, uint8_t green, uint8_t blue, int index, bool accum, uint8_t patternId);
  void displayBitmapPattern(uint8_t patternID);
  void initializeFlash(uint8_t red, uint8_t green, uint8_t blue);
  virtual float sumOfSquareAudio();
  void iterateSoundActivate();
  void displaySoundActivate();
  void iteratePrismColor(bool force);
  void initializeMatrix(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue, int upperFrameCount);
  void displayMatrix();
  void initializeHSVstrand();
  void iterateStrandByHSV();
  virtual void updateSoundActivateParameters(uint8_t thresholdData, uint8_t sampleData, uint8_t averageData);
  void advancePatternIntensity(uint8_t pattern, bool continuous);
  void triggerPatternChange(bool forward);
  void advancePatternSpeed(bool continuous, bool up);
 private:
  // Internal prototypes
  void setTwoWaySweepSpeed (int factor, uint8_t dataValue);
  bool patternOrColorChanged(uint8_t proposedData[]);
  bool colorNoChange(uint8_t proposedData[]);

  volatile uint8_t *clkport;
  volatile uint8_t *dataport;
};
#endif




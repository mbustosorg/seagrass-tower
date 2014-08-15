/*

  furSwarmMember.h

  $Date: 2012-02-25 09:44:01 -0800 (Sat, 25 Feb 2012) $
  $Revision: 273 $
  $Author: mauricio $
  
  Copyright (c) 2012, Mauricio Bustos
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

#ifndef furSwarmPatterns_h
#define furSwarmPatterns_h

#include <stdint.h>
#include "colorUtilities.h"

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
#ifdef NOT_EMBEDDED
  // Do Nothing
#elif USE_TCL
#include "TCL.h"
#elif USE_WS2801
// Adafruit RGB strand
#include "Adafruit_WS2801.h"
extern Adafruit_WS2801 strip;
const int lpdDataPin = 2;
const int lpdClockPin = 3;
const int lpdCount = 50;
#else
// Adafruit RGB strip
#include <Arduino.h>
//#include "LPD8806.h"
//extern LPD8806 strip;
const int lpdDataPin = 3;
const int lpdClockPin = 2;
#endif

// LED parameters
#define LED_COUNT (50)
#define inOutBreakPoint (150)
#define pumpUpperLevel (150)
#define pumpLowerLevel (10)
#define starfieldUpperLevel (1)
#define PWM_COUNTER_RESET (2)
#define PWM_COUNTER_OFFSET (1) // 80% Duty cycle: 1 - PWM_COUNTER_OFFSET / PWM_COUNTER_RESET
#define PWM_DIMMER_THRESHOLD (3) // Dim values lower than this based on above duty cycle

#define MAX_DATA_LENGTH (10) // Maximum expected number of bytes in incoming message
#define FS_DELAY_FACTOR (500)
#define PATTERN_START_MOD (5) // Second interval to start patterns for synchronization

class furSwarmPatterns {
 public:
  furSwarmPatterns();
  
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
  uint8_t timeToDrop = 0;
  uint8_t cycleSpot = 0;
  
  // Definitions for use by the simulator
#ifdef NOT_EMBEDDED
  uint8_t nonEmbedRed[LED_COUNT];
  uint8_t nonEmbedGreen[LED_COUNT];
  uint8_t nonEmbedBlue[LED_COUNT];
#endif
  uint8_t lowLevelPWMCounter;
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
 
  uint32_t heartbeatPumpShortStart;
  uint32_t heartbeatPumpShortPeriod;
  uint32_t heartbeatPumpLongStart;
  uint8_t breatheUpperLevel;
  uint8_t heartbeatIntensity;
  uint8_t patternSpeed;
  bool patternForward;
  uint8_t characterIndex; // Index of currently displayed character
  uint8_t characterIndexUpper; // Count of characters available in PROGMEM
  uint8_t frameIndex; // Index for frame based patterns
  uint16_t frameRelease; // Counter to aid with pattern iteration speed
  uint8_t redLevel;
  uint8_t greenLevel;
  uint8_t blueLevel;
  uint8_t intensityLevel;
  float prismHue;
  float prismValue;
 
  // Searching eye pattern variables
  uint8_t maxEye, minEye;

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




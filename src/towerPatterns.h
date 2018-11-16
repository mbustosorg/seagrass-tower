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

#ifndef towerPatterns_h
#define towerPatterns_h

#include "furSwarmPatterns.h"
#include "furSwarmPatternConst.h"
#include "towerAnimations.h"
#include "towerBall.h"
#include "accelerometer.h"
#ifdef FS_TOWER_TIKI
#include "pooferTiki.h"
#else
#include "poofer.h"
#endif
#include <XBee.h>

#define SIN_TABLE_COUNT (80)

#ifdef NOT_EMBEDDED
typedef float float32_t;
#include <math.h>
#include <memory.h>
#else
#define ARM_MATH_CM4
#include "arm_math.h"
#endif

#define AUDIO_SAMPLE_RATE (40000)
#define FRAME_RATE (60)
#define FFT_LEN (256)
#define TEST_LENGTH_SAMPLES (2 * FFT_LEN)
//#define BUCKET_FACTOR (1.17) // For 25 buckets
//#define BUCKET_COUNT (LED_COUNT / 2)
#define BUCKET_FACTOR (1.043) // For 50 buckets
#define BUCKET_COUNT (50)
#define FLAME_HEIGHT (5)
#define SPECTRUM_MIN_DB (30.0)          // Audio intensity (in decibels) that maps to low LED brightness.
#define SPECTRUM_MAX_DB (60.0)          // Audio intensity (in decibels) that maps to high LED brightness.
#define BLENDING_FRAME_COUNT (60)

const int MAX_TOWER_COUNT = 35;

class towerPatterns : public furSwarmPatterns {

 public:

  towerPatterns();

  // Redefinitions
  void initializePattern(uint8_t *data, uint8_t dataLength);
  void continuePatternDisplay();
  void displayData(bool red, bool green, bool blue);
  
  void calibrateTilt();
  bool checkShaking();
  void readTilt();
  rgb tiltColor();
  void initializeDancing();
  void dance();
  void initializeTilt();
  void tilt();
  void shakeSparkle();
  void grassWave();
  void sparklerDrip();
  void initializeGrassWave(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue, bool resetPositions);
  void iterateGrassWave();
  float gravity();
  float damping();
  void initializeBouncingBall();
  void iterateBouncingBall();
  float ballLocation(bool persist);
  void setBasicParameters(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue);
  void initializeRadioTower();
  void iterateRadioTower();
  void setRadioTowerSyncTimestamp(uint32_t timestamp);
  float sumOfSquareAudio();
  void updateSoundActivateParameters(uint8_t thresholdData, uint8_t sampleData, uint8_t averageData);
  void iterateSpectrumAnalyzer();
  void updateFrequencyBuckets();
  void updateSpectrumLevels();
  void iterateSearchingEye();
  void iterateBubbleWave();
  void initializeBroken();
  void iterateBroken();
  void iteratePong();
  void initializeFlame();
  void iterateFlame();
  void initializeCandle();
  void iterateCandle();

  // Bounce
  bool reverseBounce;

  // Pong
  towerBall ball;

  // GPS data
  int32_t latitude;
  int32_t longitude;

  // Accelerometer data
  accelerometer accel;
  TiltVector tiltVector;
  rgb currentRGBOut, lastRGBOut;
  TiltVector lastTiltVector;
  TiltVector penultimateTiltVector;
  bool useTiltForBounceColor;

  bool isShaking;
  bool wasShaking;  
  unsigned long shakeStart;

  uint32_t shakingStart;
  uint32_t shakingTimestamp;
  uint8_t releaseCounter;
  uint8_t timeToDrop2;
  uint8_t timeToDropInitial;
  uint8_t timeToDropRed;
  uint8_t sparkleDrops[7];
  uint8_t adjustedRed;
  uint8_t adjustedBlue;
  uint8_t adjustedGreen;
  uint8_t adjustedPatternSpeed;
  uint8_t unadjustedIntensity;
  uint8_t unadjustedRed;
  uint8_t unadjustedBlue;
  uint8_t unadjustedGreen;

  float ballY;
  float ballSpeed;
  uint32_t ballStart;
  uint32_t nextBallStart;

  uint32_t radioTowerStart;
  uint32_t radioTowerPeriod;
  uint32_t radioTowerSyncTimestamp;

  uint8_t bladeHeightLow, bladeHeightHigh;

  // Animation data
  towerAnimations animations;
  int frameNumber;
  void setFrameNumber(int newFrameNumber);

  // Broken pattern
  uint32_t brokenOff;
  uint32_t brokenOn;
  uint8_t brokenBits[7], unBrokenBits[7];

  // Poofer control
#ifdef FS_TOWER_TIKI
  pooferTiki pooferControl;
#else
  poofer pooferControl;
#endif

  // Audio data
  uint8_t spectrumTowerId;
  uint8_t spectrumTowerCount;
  uint32_t audioSampleInputIndex;
  float32_t audioSampleInput[2 * FFT_LEN];
  float32_t audioMagnitudeOutput[FFT_LEN]; 
  float32_t audioMagnitudeBuckets[BUCKET_COUNT];
  uint8_t runningMagnitude[LED_COUNT];

  float32_t signalPower;
  float32_t gain;
  float32_t reference;
  float32_t average;
  
  float sinTable[SIN_TABLE_COUNT];

  float hammingFilter[FFT_LEN / 2];
  
  /* 40 entry sine table
	 float sinTable[SIN_TABLE_COUNT] = {1.2801,1.3955, 1.4832, 1.5559, 1.6186,
	 1.6738, 1.7228, 1.7667, 1.8059, 1.8409,
	 1.8720, 1.8995, 1.9234, 1.9439, 1.9612,
	 1.9752, 1.9861, 1.9938, 1.9985, 2.0000,
	 1.9985, 1.9938, 1.9861, 1.9752, 1.9612,
	 1.9439, 1.9234, 1.8995, 1.8720, 1.8409,
	 1.8059, 1.7667, 1.7228, 1.6738, 1.6186,
	 1.5559, 1.4832, 1.3955, 1.2801, 1.0000,
	 0.7199, 0.6045, 0.5168, 0.4441, 0.3814,
	 0.3262, 0.2772, 0.2333, 0.1941, 0.1591,
	 0.1280, 0.1005, 0.0766, 0.0561, 0.0388,
	 0.0248, 0.0139, 0.0062, 0.0015, 0.0000,
	 0.0015, 0.0062, 0.0139, 0.0248, 0.0388,
	 0.0561, 0.0766, 0.1005, 0.1280, 0.1591,
	 0.1941, 0.2333, 0.2772, 0.3262, 0.3814,
	 0.4441, 0.5168, 0.6045, 0.7199, 1.0000};
  */
  
  /* 20 entry sine table
	 float sinTable[] = {1.1564, 1.3090, 1.4540, 1.5878, 1.7071, 
	 1.8090, 1.8910, 1.9511, 1.9877, 2.0000, 
	 1.9877, 1.9511, 1.8910, 1.8090, 1.7071, 
	 1.5878, 1.4540, 1.3090, 1.1564, 1.0000, 
	 0.8436, 0.6910, 0.5460, 0.4122, 0.2929, 
	 0.1910, 0.1090, 0.0489, 0.0123, 0.0000, 
	 0.0123, 0.0489, 0.1090, 0.1910, 0.2929, 
	 0.4122, 0.5460, 0.6910, 0.8436, 1.0000};
  */
  
  /* Fat 40 entry sine table
	 float sinTable[] = {1.3955, 1.5559, 1.6738, 1.7667, 1.8409, 
	 1.8995, 1.9439, 1.9752, 1.9938, 2.0000, 
	 1.9938, 1.9752, 1.9439, 1.8995, 1.8409, 
	 1.7667, 1.6738, 1.5559, 1.3955, 1.0000, 
	 0.6045, 0.4441, 0.3262, 0.2333, 0.1591, 
	 0.1005, 0.0561, 0.0248, 0.0062, 0.0000, 
	 0.0062, 0.0248, 0.0561, 0.1005, 0.1591, 
	 0.2333, 0.3262, 0.4441, 0.6045, 1.0000};
  */
  
  /* 20 entry sine table
	 float sinTable[] = {1.3090, 1.5878, 1.8090, 1.9511, 2.0000, 
	 1.9511, 1.8090, 1.5878, 1.3090, 1.0000, 
	 0.6910, 0.4122, 0.1910, 0.0489, 0.0000, 
	 0.0489, 0.1910, 0.4122, 0.6910, 1.0000}; 
  */

};

#endif



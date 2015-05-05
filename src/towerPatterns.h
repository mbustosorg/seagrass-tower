/*

  towerPatterns.h

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

#ifndef towerPatterns_h
#define towerPatterns_h

#include "furSwarmPatterns.h"
#include "furSwarmPatternConst.h"
#include "towerAnimations.h"
#include "towerBall.h"
#include "accelerometer.h"
#include <XBee.h>

#define SIN_TABLE_COUNT (80)

#ifdef NOT_EMBEDDED
typedef float float32_t;
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
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
#define BUCKET_COUNT (LED_COUNT)
#define FLAME_HEIGHT (5)
#define SPECTRUM_MIN_DB (30.0)          // Audio intensity (in decibels) that maps to low LED brightness.
#define SPECTRUM_MAX_DB (60.0)          // Audio intensity (in decibels) that maps to high LED brightness.

const int MAX_TOWER_COUNT = 35;

class towerPatterns : public furSwarmPatterns {

 public:

  towerPatterns();

  // Redefinitions
  void initializePattern(uint8_t *data, uint8_t dataLength);
  void continuePatternDisplay();
  
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
  bool reverseBounce = false;

  // Pong
  towerBall ball;

  // GPS data
  int32_t latitude = 0;
  int32_t longitude = 0;

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

  uint32_t shakingStart = 0;
  uint32_t shakingTimestamp = 0;
  uint8_t releaseCounter = 1;
  uint8_t timeToDrop2 = 0;
  uint8_t timeToDropInitial = 0;
  uint8_t timeToDropRed = 0;
  uint8_t sparkleDrops[7] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t adjustedRed = 0;
  uint8_t adjustedBlue = 0;
  uint8_t adjustedGreen = 0;
  uint8_t adjustedPatternSpeed = 0;
  uint8_t unadjustedIntensity = 0;
  uint8_t unadjustedRed = 0;
  uint8_t unadjustedBlue = 0;
  uint8_t unadjustedGreen = 0;

  float ballY;
  float ballSpeed;
  uint32_t ballStart;
  uint32_t nextBallStart;

  uint32_t radioTowerStart;
  uint32_t radioTowerPeriod;
  uint32_t radioTowerSyncTimestamp;

  uint8_t bladeHeightLow, bladeHeightHigh;

  // Animation data
  XBeeAddress64 towerAddresses[MAX_TOWER_COUNT];
  towerAnimations animations;
  int frameNumber = 0;
  void setFrameNumber(int newFrameNumber);

  // Broken pattern
  uint32_t brokenOff;
  uint32_t brokenOn;
  uint8_t brokenBits[7], unBrokenBits[7];

  // Audio data
  uint8_t spectrumTowerId = 0;
  uint8_t spectrumTowerCount = 0;
  uint32_t audioSampleInputIndex = 0;
  float32_t audioSampleInput[2 * FFT_LEN];
  float32_t audioMagnitudeOutput[FFT_LEN]; 
  float32_t audioMagnitudeBuckets[BUCKET_COUNT];
  uint8_t runningMagnitude[LED_COUNT];

  float32_t signalPower = 0.0;
  float32_t gain = -20.0;
  float32_t reference = -10.0;
  float32_t average = 0.0;
  
  float sinTable[SIN_TABLE_COUNT] = {1.0785, 1.1564, 1.2334, 1.3090, 1.3827,
									 1.4540, 1.5225, 1.5878, 1.6494, 1.7071, 
									 1.7604, 1.8090, 1.8526, 1.8910, 1.9239,
									 1.9511, 1.9724, 1.9877, 1.9969, 1.9969,
									 1.9969, 1.9877, 1.9724, 1.9511, 1.9239,
									 1.8910, 1.8526, 1.8090, 1.7604, 1.7071,
									 1.6494, 1.5878, 1.5225, 1.4540, 1.3827,
									 1.3090, 1.2334, 1.1564, 1.0785, 1.0000,
									 0.9215, 0.8436, 0.7666, 0.6910, 0.6173, 
									 0.5460, 0.4775, 0.4122, 0.3506, 0.2929,
									 0.2396, 0.1910, 0.1474, 0.1090, 0.0761,
									 0.0489, 0.0276, 0.0123, 0.0031, 0.0031,
									 0.0031, 0.0123, 0.0276, 0.0489, 0.0761,
									 0.1090, 0.1474, 0.1910, 0.2396, 0.2929,
									 0.3506, 0.4122, 0.4775, 0.5460, 0.6173,
									 0.6910, 0.7666, 0.8436, 0.9215, 1.0000};

  float hammingFilter[FFT_LEN / 2] = {0.00000, 0.08056, 0.08225, 0.08506, 0.08898,
									  0.09400, 0.10012, 0.10731, 0.11556, 0.12485,
									  0.13516, 0.14645, 0.15871, 0.17191, 0.18600,
									  0.20096, 0.21675, 0.23333, 0.25066, 0.26870,
									  0.28740, 0.30672, 0.32661, 0.34703, 0.36792,
									  0.38922, 0.41090, 0.43289, 0.45515, 0.47761,
									  0.50022, 0.52294, 0.54569, 0.56843, 0.59110,
									  0.61365, 0.63601, 0.65814, 0.67998, 0.70148,
									  0.72258, 0.74324, 0.76340, 0.78301, 0.80203,
									  0.82041, 0.83810, 0.85506, 0.87125, 0.88663,
									  0.90116, 0.91480, 0.92753, 0.93932, 0.95012,
									  0.95992, 0.96869, 0.97642, 0.98308, 0.98865,
									  0.99312, 0.99649, 0.99873, 0.99986, 0.99986,
									  0.99873, 0.99649, 0.99312, 0.98865, 0.98308,
									  0.97642, 0.96869, 0.95992, 0.95012, 0.93932,
									  0.92753, 0.91480, 0.90116, 0.88663, 0.87125,
									  0.85506, 0.83810, 0.82041, 0.80203, 0.78301,
									  0.76340, 0.74324, 0.72258, 0.70148, 0.67998,
									  0.65814, 0.63601, 0.61365, 0.59110, 0.56843,
									  0.54569, 0.52294, 0.50022, 0.47761, 0.45515,
									  0.43289, 0.41090, 0.38922, 0.36792, 0.34703,
									  0.32661, 0.30672, 0.28740, 0.26870, 0.25066,
									  0.23333, 0.21675, 0.20096, 0.18600, 0.17191,
									  0.15871, 0.14645, 0.13516, 0.12485, 0.11556,
									  0.10731, 0.10012, 0.09400, 0.08898, 0.08506,
									  0.08225, 0.08056, 0.00000};
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



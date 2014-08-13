/*

  towerPatterns.cpp

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

#include "embeddedInterface.h"

#include "towerPatterns.h"
#include "colorUtilities.h"
#include "animation/flameFrames.h"

#ifdef NOT_EMBEDDED
uint32_t PIT_TCTRL3 = 0;
#endif

towerPatterns::towerPatterns() : furSwarmPatterns(){
  isShaking = false;
  wasShaking = false;
  shakingStart = 0;
  lastRGBOut.r = 0.0;
  lastRGBOut.g = 0.0;
  lastRGBOut.b = 0.0;
  currentRGBOut.r = 0.0;
  currentRGBOut.g = 0.0;
  currentRGBOut.b = 0.0;

  uint32_t ifftFlag = 0; 
  uint32_t doBitReverse = 1; 

  for (int i = 0; i < MAX_TOWER_COUNT; i++) {
	towerAddresses[i] = XBeeAddress64(0x00000000, 0x00000000);
  }
  
#ifndef NOT_EMBEDDED
  arm_cfft_radix4_instance_f32 fft_inst;  /* CFFT Structure instance */
  arm_cfft_radix4_init_f32(&fft_inst, FFT_LEN, ifftFlag, doBitReverse);
#endif
}

//! Set the current frame
void towerPatterns::setFrameNumber(int newFrameNumber) {
  frameNumber = newFrameNumber;
}

//! Initialize the selected pattern
void towerPatterns::initializePattern(uint8_t *data, uint8_t dataLength) {
  int messageType;
  // data [0] - messageType
  // data [1] - speed
  // data [2] - red
  // data [3] - green
  // data [4] - blue
  // data [5] - aux (usually intensity)
  // data [6] - delay in ms
  // Disable Timer 3 used for audio sampling, let the request turn it back on
  PIT_TCTRL3 &= ~(1 << 0);
  messageType = (int) (0x7F & data[0]);
  //messageType = (int) data[0];
  transitionRequested = 0x80 & data[0];
  patternSpeed = (int) data[1];
  switch (messageType) {
  case FS_ID_TILT:
	patternSpeed = 255 - (int) data [1];
	setPatternSpeedWithFactor(10);
	if (pattern != messageType) {
	  initializeTilt();
	  pattern = messageType;
	}
	break;
  case FS_ID_SHAKE_SPARKLE:
  case FS_ID_SPARKLER:
	//setPatternSpeedWithFactor(1);
	setPatternSpeedWithFactor(20);
	setBasicParameters (data[5], data[2], data[3], data[4]);
	pattern = messageType;
	break;
  case FS_ID_GRASS_WAVE:
	patternSpeed = 255 - (int) data [1];
	setPatternSpeedWithFactor(15);
	initializeGrassWave(data[5], data[2], data[3], data[4], pattern != messageType);
	pattern = messageType;
	break;
  case FS_ID_RADIO_TOWER:
	if (pattern != messageType) {
	  initializeRadioTower();
	}
	setBasicParameters(data[5], data[2], data[3], data[4]);
	pattern = messageType;
	break;
  case FS_ID_BOUNCING_BALL:
	if (pattern != messageType) {
	  initializeBouncingBall();
	}
	if (data[2] == 255 && data[3] == 255 && data[4] == 255) {
	  useTiltForBounceColor = true;
	} else {
	  useTiltForBounceColor = false;
	  setBasicParameters(data[5], data[2], data[3], data[4]);
	}
 	pattern = messageType;
	break;
  case FS_ID_SPECTRUM_ANALYZER:
 	pattern = messageType;
	updateSoundActivateParameters(data[1], data[5], data[5]);
	PIT_TCTRL3 |= 0x1;
	break;
  case FS_ID_FOREST_RUN:
	patternSpeed = (int) data [1];
	setPatternSpeedWithFactor(15);
	setBasicParameters(data[5], data[2], data[3], data[4]);
	for (int i = 0; i < LED_COUNT; i++) {
	  ledRed[i] = unadjustedRed;
	  ledGreen[i] = unadjustedGreen;
	  ledBlue[i] = unadjustedBlue;
	}
	//if (pattern != messageType) {
	  timeToDropInitial = (uint8_t) random (1, patternSpeed);
	  timeToDrop = timeToDropInitial;
	  //}
 	pattern = messageType;
	break;
  case FS_ID_ANIMATE_1:
	animations.startAnimation(millis());
	initializePattern(animations.currentPattern(), 6);
	break;
  case FS_ID_SEARCHING_EYE:
 	pattern = messageType;
	setPatternSpeedWithFactor(10);
	patternSpeed = random (patternSpeed, patternSpeed + 2);
	setBasicParameters(data[5], data[2], data[3], data[4]);
	break;
  case FS_ID_BUBBLE_WAVE:
 	pattern = messageType;
	setPatternSpeedWithFactor(10);
	patternSpeed = random (patternSpeed, patternSpeed + 2);
	setBasicParameters(data[5], data[2], data[3], data[4]);
	break;
  case FS_ID_BROKEN:
 	pattern = messageType;
	initializeBroken();
	for (int i = 0; i < 7; i++) {
	  brokenBits[i] = random (0, 255);
	  unBrokenBits[i] = random (0, 255);
	}
	break;
  case FS_ID_PONG:
	if (pattern != messageType) {
	  ball.initialize(data[2], data[3]);
	}
	setBasicParameters(data[5], data[2], data[3], data[4]);
	if (ball.xSpeed != 0.0 && ball.ySpeed != 0.0) {
	  ball.setSpeed (((float) data [1]) / 20.0 * (ball.xSpeed / abs (ball.xSpeed)), ((float) data [1]) / 30.0 * (ball.ySpeed / abs (ball.ySpeed)));
	} else {
	  ball.setSpeed (((float) data [1]) / 20.0, ((float) data [1]) / 30.0);
	}
	pattern = messageType;
	break;
  case FS_ID_GIANT_SPECTRUM:
 	pattern = messageType;
	spectrumTowerId = data[2];
	spectrumTowerCount = data[3];
	updateSoundActivateParameters(data[1], data[5], data[5]);
	PIT_TCTRL3 |= 0x1;
	break;  
  case FS_ID_FLAME:
	setBasicParameters(data[5], data[2], data[3], data[4]);
	setPatternSpeedWithFactor(5);
	if (pattern != messageType) {
	  initializeFlame();
	}
	pattern = messageType;
	break;
  case FS_ID_CANDLE:
	setBasicParameters(data[5], data[2], data[3], data[4]);
	setPatternSpeedWithFactor(5);
	initializeCandle();
	pattern = messageType;
	break;
  default:
	furSwarmPatterns::initializePattern(data, dataLength);
	break;
  }
}

//! Continue pattern display
void towerPatterns::continuePatternDisplay() {
  checkLatestData();
#ifdef FS_TOWER
  if (checkShaking()) {
	isShaking = true;
	shakeStart = millis();
  }
  if (isShaking) {
	setfullStrand(0, 0, 0, 0, false);
	if (millis() - shakeStart > 10000) isShaking = false;
	return;
  }
#endif
  // Continue with pattern display
  switch (pattern) {
  case FS_ID_TILT:
	tilt();
	displayData(true, true, true);
	break;
  case FS_ID_SHAKE_SPARKLE:
	shakeSparkle();
	displayData(true, true, true);		
    break;
  case FS_ID_SPARKLER:
	sparklerDrip();
	displayData(true, true, true);		
	break;
  case FS_ID_GRASS_WAVE:
	iterateGrassWave();
	displayData(true, true, true);		
	break;
  case FS_ID_RADIO_TOWER:
	iterateRadioTower();
	displayData(true, true, true);
	break;
  case FS_ID_BOUNCING_BALL:
	iterateBouncingBall();
	displayData(true, true, true);		
	break;
  case FS_ID_SPECTRUM_ANALYZER:
	iterateSoundActivate();
	iterateSpectrumAnalyzer();
	displayData(true, true, true);		
	break;
  case FS_ID_FOREST_RUN:
	if (timeToDrop == 0) {
	  timeToDrop = timeToDropInitial;
	} else {
	  float iterationProportion = (float) 1.0 / (float) timeToDropInitial;
	  for (int i = 0; i < LED_COUNT; i++) {
		ledRed[i] = ledRed[i] - (uint8_t) max (1, timeToDropInitial * (1.0 - iterationProportion));
		ledGreen[i] = ledGreen[i] - (uint8_t) max (1, timeToDropInitial * (1.0 - iterationProportion));
		ledBlue[i] = ledBlue[i] - (uint8_t) max (1, timeToDropInitial * (1.0 - iterationProportion));
	  }
	  timeToDrop--;
	}
	displayData(unadjustedRed != 0, unadjustedGreen != 0, unadjustedBlue != 0);
	break;
  case FS_ID_SEARCHING_EYE:
	iterateSearchingEye();
	displayData(true, true, true);		
	break;
  case FS_ID_BUBBLE_WAVE:
	iterateBubbleWave();
	displayData(true, true, true);		
	break;
  case FS_ID_BROKEN:
	iterateBroken();
	displayData(true, true, true);		
	break;
  case FS_ID_PONG:
	iteratePong();
	displayData(true, true, true);
	break;
  case FS_ID_GIANT_SPECTRUM:
	iterateSoundActivate();
	iterateSpectrumAnalyzer();
	displaySoundActivate();
	break;
  case FS_ID_FLAME:
	iterateFlame();
	displayData(true, true, true);
	break;
  case FS_ID_CANDLE:
	iterateCandle();
	displayData(true, true, true);
	break;
  default:
	furSwarmPatterns::continuePatternDisplay();
	break;
  }
  // Check if we're in animation mode and what to do next
  if (animations.isAnimating) {
	if (animations.nextPattern(millis())) {
	  uint8_t animationData[MAX_DATA_LENGTH];
	  memcpy (animationData, animations.currentPattern(), ANIMATION_COMMAND_LENGTH);
	  if (lastDelayFactor > 0) {
		animationData[ANIMATION_COMMAND_LENGTH - 1] = lastDelayFactor;
	  }
	  setPatternData(animationData, ANIMATION_COMMAND_LENGTH);
	}
  }
}

//! Read the current tilt
void towerPatterns::readTilt() {
  tiltVector = accel.filteredTilt();
  calibrateTilt();
}

//! This is used for establishing a calibration opportunity
void towerPatterns::calibrateTilt() {
}

//! Does the accelerometer detect shaking?
bool towerPatterns::checkShaking() {
  return accel.isShaking();
}

//! Initialize the tilt pattern
void towerPatterns::initializeTilt() {
  adjustedPatternSpeed = patternSpeed;
  timeToDrop = adjustedPatternSpeed;
  accel.setFilterLength(10);
  cycleSpot = 0;
}

//! color struct for the current tilt
rgb towerPatterns::tiltColor() {
  readTilt();
  hsv in;
  if (tiltVector.z == 0 && tiltVector.x == 0) {
	in.h = 0.0;
  } else {
	in.h = atan2(tiltVector.z, tiltVector.x) * 360.0 / (2.0 * 3.14159);
  }
  in.s = 1.0;
  in.v = tiltVector.x * tiltVector.x + tiltVector.z * tiltVector.z;
  if (in.v < 0.1) {
	in.v = 0.1;
  }
  return hsv2rgb(in);
}

// Iterate the transition point for patterns that request it
void towerPatterns::iterateForTransition() {
  if (timeToDrop == 0) {
	if (cycleSpot < LED_COUNT + 1) {
	  timeToDrop = patternSpeed / 3;
	  cycleSpot++;
	  if (patternSpeed < 100) {
		cycleSpot++;
	  }
	} else {
	  timeToDrop = patternSpeed;
	}
  } else {
	timeToDrop--;
  }
}

//! determine tilt and set colors
void towerPatterns::tilt() {
  iterateForTransition();
  if (timeToDrop == 1) {
	lastRGBOut = currentRGBOut;
  }
  currentRGBOut = tiltColor();
  float brightnessFactor = 7.0;
  float iterationProportion = (float) timeToDrop / (float) patternSpeed;
  rgb currentByte, lastByte;
  currentByte.r = min(currentRGBOut.r * 255.0 * brightnessFactor, 255);
  currentByte.g = min(currentRGBOut.g * 255.0 * brightnessFactor, 255);
  currentByte.b = min(currentRGBOut.b * 255.0 * brightnessFactor, 255);
  for (int i = 0; i < LED_COUNT; i++) {
	if (LED_COUNT - i - 1 < cycleSpot) {
	  ledRed[i] = (uint8_t) currentByte.r;
	  ledGreen[i] = (uint8_t) currentByte.g;
	  ledBlue[i] = (uint8_t) currentByte.b;
	}
	if (LED_COUNT - i == cycleSpot) {
	  ledRed[i] = (uint8_t) (ledRed[i] * (1.0 - iterationProportion));
	  ledGreen[i] = (uint8_t) (ledGreen[i] * (1.0 - iterationProportion));
	  ledBlue[i] = (uint8_t) (ledBlue[i] * (1.0 - iterationProportion));
	}
  }
}

//! Iterate shake sparkle pattern
void towerPatterns::shakeSparkle() {
  sparklerDrip();
}

//! Perform the grass wave cycle
void towerPatterns::initializeGrassWave(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue, bool resetPositions) {
  if (resetPositions) {
	timeToDrop = patternSpeed;
	cycleSpot = (uint8_t) random (0, SIN_TABLE_COUNT - 1);
	bladeHeightLow = (uint8_t) random (5, 15);
	bladeHeightHigh = (uint8_t) random (35, 49);
  }
  adjustedPatternSpeed = patternSpeed;
  setBasicParameters(intensity, red, green, blue);
}

//! Set the basics.  Can be used to update parameters during the execution of a pattern.
void towerPatterns::setBasicParameters(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue) {
  unadjustedIntensity = intensity;
  adjustedRed = red * intensity / 255;
  adjustedGreen = green * intensity / 255;
  adjustedBlue = blue * intensity / 255;
  unadjustedRed = red;
  unadjustedGreen = green;
  unadjustedBlue = blue;
}

//! Increment the grass wave pattern
void towerPatterns::iterateGrassWave() {
  uint8_t nextCycleSpot = 0;
  if (timeToDrop == 0) {
	timeToDrop = patternSpeed;
	cycleSpot++;
	if (cycleSpot >= SIN_TABLE_COUNT) {
	  cycleSpot = 0;
	}
  } else {
	timeToDrop--;
  }
  nextCycleSpot = cycleSpot + 1;
  if (nextCycleSpot >= SIN_TABLE_COUNT) {
	nextCycleSpot = 0;
  }
  int displayLevel = bladeHeightLow + (int) ((bladeHeightHigh - bladeHeightLow) * sinTable [cycleSpot] / 2.0);
  int nextDisplayLevel = bladeHeightLow + (int) ((bladeHeightHigh - bladeHeightLow) * sinTable [nextCycleSpot] / 2.0);
  float iterationProportion = (float) timeToDrop / (float) patternSpeed;
  nextDisplayLevel = displayLevel + floor((float)(nextDisplayLevel - displayLevel) * (1.0 - iterationProportion) + 0.5);
  for (int i = 0; i < LED_COUNT; i++) {
	ledRed[i] = 0;
	ledGreen[i] = 0;
	ledBlue[i] = 0;
	if (displayLevel <= nextDisplayLevel) {
	  if (i < displayLevel) {
		ledRed[i] = adjustedRed;
		ledGreen[i] = adjustedGreen;
		ledBlue[i] = adjustedBlue;
	  } else if (i < nextDisplayLevel) {
		ledRed[i] = ledRed[i] + adjustedRed * (1.0 - iterationProportion);
		ledGreen[i] = ledGreen[i] + adjustedGreen * (1.0 - iterationProportion);
		ledBlue[i] = ledBlue[i] + adjustedBlue * (1.0 - iterationProportion);
	  }
	} else {
	  if (i < nextDisplayLevel) {
		ledRed[i] = adjustedRed;
		ledGreen[i] = adjustedGreen;
		ledBlue[i] = adjustedBlue;
	  } else if (i < displayLevel) {
		ledRed[i] = ledRed[i] + adjustedRed * iterationProportion;
		ledGreen[i] = ledGreen[i] + adjustedGreen * iterationProportion;
		ledBlue[i] = ledBlue[i] + adjustedBlue * iterationProportion;
	  }
	}
  }
}

//! Perform the sparkler drip pattern
void towerPatterns::sparklerDrip() {
  if (timeToDrop == 0) {
	sparkleDrops[6] = sparkleDrops[6] | 0x01;
	timeToDrop = (uint8_t) random (5, 60);
  } else {
	timeToDrop--;
  }
  if (timeToDropRed == 0) {
	//adjustedRed = (uint8_t) random (0, 255);
	//adjustedGreen = adjustedRed;
	//adjustedBlue = adjustedRed;
	timeToDropRed = (uint8_t) random (0, 30);
  } else {
	timeToDropRed--;
  }
  releaseCounter--;
  if (releaseCounter == 0) {
	releaseCounter = patternSpeed;
	for (int i = 0; i < 7; i++) {
	  sparkleDrops[i] = sparkleDrops[i] << 1;
	  if (i < 6 && sparkleDrops[i + 1] & 0x80) {
		sparkleDrops[i] = sparkleDrops[i] | 0x01;
	  }
	}
  }
  float iterationProportion = (float) releaseCounter / (float) patternSpeed;
  for (int i = 0; i < LED_COUNT; i++) {
 	ledRed[i] = 0;
	ledGreen[i] = 0;
	ledBlue[i] = 0;
  }
  // LED 50 is at the top of the tower
  int j;
  for (int i = LED_COUNT - 1; i >= 0; i--) {
	j = i + 1;
	if ((1 << (7 - i % 8)) & sparkleDrops[i / 8]) {
	  ledRed[i] = adjustedRed * (1.0 - iterationProportion);
	  ledGreen[i] = adjustedGreen * (1.0 - iterationProportion);
	  ledBlue[i] = adjustedBlue * (1.0 - iterationProportion);
	  if (j < LED_COUNT - 1) {
		ledRed[j] = ledRed[j] + adjustedRed * iterationProportion;
		ledGreen[j] = ledGreen[j] + adjustedGreen * iterationProportion;
		ledBlue[j] = ledBlue[j] + adjustedBlue * iterationProportion;
	  }
	} else {
	  if (i > 48  && (1 << (7 - i % 8))) {
		ledRed[i] = 255;
		ledGreen[i] = 255;
		ledBlue[i] = 255;
	  }
	}
  }
}

//! Initialize the bouncing ball pattern
void towerPatterns::initializeBouncingBall() {
  ballY = 0.0;
  ballSpeed = gravity();
  ballStart = millis();
  // Next start time is random between:
  //   0.5 - 3 seconds at the fastest
  //   7.5 - 15 seconds at the slowest
  nextBallStart = random (500 + 7000 * (1.0 - (float)patternSpeed / 255.0), 3000 + 12000 * (1.0 - (float)patternSpeed / 255.0));
}

//! Iterate the bouncing ball pattern
void towerPatterns::iterateBouncingBall() {
  if (millis() > ballStart + nextBallStart) {
	initializeBouncingBall();
  }
  float thisBallLocation = ballLocation(true);
  float nextBallLocation = ballLocation(false);
  if (abs (nextBallLocation - thisBallLocation) > 1) {
	if (thisBallLocation > nextBallLocation) {
	  nextBallLocation = thisBallLocation - 1;
	} else {
	  nextBallLocation = thisBallLocation + 1;
	}
  }
  if (useTiltForBounceColor) {
	currentRGBOut = tiltColor();
  }
  int index;
  for (int i = 0; i < LED_COUNT; i++) {
	if (reverseBounce) {
	  index = LED_COUNT - i - 1;
	} else {
	  index = i;
	}
	ledRed[index] = 0;
	ledGreen[index] = 0;
	ledBlue[index] = 0;
    if (i == (uint8_t)thisBallLocation || i == (uint8_t)nextBallLocation) {
	  if (useTiltForBounceColor) {
		ledRed[index] = (uint8_t) (currentRGBOut.r * 255.0);
		ledGreen[index] = (uint8_t) (currentRGBOut.g * 255.0);
		ledBlue[index] = (uint8_t) (currentRGBOut.b * 255.0);
	  } else {
		ledRed[index] = unadjustedRed;
		ledGreen[index] = unadjustedGreen;
		ledBlue[index] = unadjustedBlue;
	  }
	}
  }
}

//! Current gravity value
float towerPatterns::gravity() {
  return ((float) patternSpeed / 255.0) * 0.6 + 0.003; // Range is 0.003 - 0.603
}

//! Current damping value
float towerPatterns::damping() {
  return -1.5 * ((255 - unadjustedIntensity) / 255.0);
}

//! The next location for the ball based on previous location and gravity
float towerPatterns::ballLocation(bool persist) {
  float currentDamping = damping();
  float newBallSpeed = ballSpeed + gravity();
  float newBallY = ballY + newBallSpeed;
  if (newBallY + newBallSpeed >= LED_COUNT) {
    newBallSpeed = newBallSpeed * currentDamping;
  }
  if (newBallY > LED_COUNT) newBallY = LED_COUNT;
  float ballLocation = (float)LED_COUNT - (uint8_t) newBallY - 1.0;
  if (ballLocation < 0) ballLocation = 0;
  if (persist) {
	ballSpeed = newBallSpeed;
	ballY = newBallY;
  }
  return ballLocation;
}

//! Initialize the radio tower pattern
void towerPatterns::initializeRadioTower() {
  radioTowerStart = millis();
  radioTowerPeriod = random (3490, 3500);
}

//! Iterate the radio tower pattern
void towerPatterns::iterateRadioTower() {
  uint32_t currentTimestamp = millis();
  if (currentTimestamp > radioTowerStart + radioTowerPeriod) {
	radioTowerStart = radioTowerStart + radioTowerPeriod;
  }
  for (int i = 0; i < LED_COUNT; i++) {
	ledRed[i] = 0;
	ledGreen[i] = 0;
	ledBlue[i] = 0;
	// Blink to 3 LEDs for a second at beginning of period
	if (clock.seconds % 3 == 0 && i > LED_COUNT - 3) {
	  ledRed[i] = adjustedRed;
	  ledGreen[i] = adjustedGreen;
	  ledBlue[i] = adjustedBlue;
	}
	// Blink bottom 3 LEDs at 1Hz rate using `radioTowerSyncTimestamp' as the boundary
	if (frameNumber < 8 && i < 3) {
	  ledRed[i] = adjustedRed;
	  ledGreen[i] = adjustedGreen;
	  ledBlue[i] = adjustedBlue;	  
	}
  }
}

//! Set the timestamp for the radio tower blink
void towerPatterns::setRadioTowerSyncTimestamp (uint32_t timestamp) {
  radioTowerSyncTimestamp = timestamp;
}

//! Update sound parameters
// thresholdData is controlled the pattern speed slider
// sampleData and averageData is controlled by intensity slider
void towerPatterns::updateSoundActivateParameters(uint8_t thresholdData, uint8_t sampleData, uint8_t averageData) {
  if (pattern == FS_ID_SOUND_ACTIVATE) {
	return furSwarmPatterns::updateSoundActivateParameters(thresholdData, sampleData, averageData);
  } else {
	float newThreshold;
	float newSampleNumber;
	float newAveragedOver;
	newThreshold = (float) thresholdData;
	newThreshold = 100 + newThreshold * 900.0 / 255.0; // Threshold can range from 100 to 1000
	newSampleNumber = (float) sampleData;
	newSampleNumber = 100 + newSampleNumber * 250.0 / 255.0; // Number of samples can range from 1 to 350
	newAveragedOver = (float) averageData;
	newAveragedOver = 12 - (float) newAveragedOver * 11.0 / 255.0; // Averaged over count can range from 12 to 1
	saThreshold = (long) newThreshold;
	saTargetThreshold = saThreshold * 0.90;
	saNumberOfSamples = (int) newSampleNumber;
	saAveragedOver = (int) newAveragedOver;
  }
}

//! Sum of squared audio signal
float towerPatterns::sumOfSquareAudio() {
  if (pattern == FS_ID_SOUND_ACTIVATE) {
	saNumberOfSamples = 350;
	return furSwarmPatterns::sumOfSquareAudio();
  } else { 
	saNumberOfSamples = TEST_LENGTH_SAMPLES / 2;
	saThreshold = 300.0;
	saTargetThreshold = saThreshold * 0.90;
	saMovingAverageCount = 25;
	saAveragedOver = 2;
	float saSumOfSquare = 0;
 	for (int i = 0; i < TEST_LENGTH_SAMPLES / 2; i++) {
	  saSumOfSquare += audioSampleInput[i * 2] * audioSampleInput[i * 2] * saGain;
	}
	return saSumOfSquare;
  }
}

//! Iterate the spectrum analyzer pattern
void towerPatterns::iterateSpectrumAnalyzer() {
  iterateSoundActivate();
  if (audioSampleInputIndex == 0) {
#ifndef NOT_EMBEDDED
#ifdef FFT_DIAGNOSTICS
	uint32_t fftStart, fftMag;
	Serial.print ("RUN:");
	Serial.print ((float32_t)saTargetThreshold);
	Serial.print (",");
	Serial.print ((float32_t)saMovingAverage);
	Serial.print (",");
	Serial.print ((float32_t)saGain);
	Serial.print (",");
	Serial.print ((float32_t)saRunningAverage);
	Serial.print (",");
	Serial.print ((float32_t)signalPower);
	Serial.println ("|");
	Serial.print ("INP:");
  	for (int i = 0; i < TEST_LENGTH_SAMPLES / 2; i++) {
	  Serial.print(audioSampleInput[i * 2]);
	  if (i < TEST_LENGTH_SAMPLES / 2 - 1) {
		Serial.print(",");
	  }
	}
	Serial.println ("|");
	fftStart = millis();
#endif
	uint32_t ifftFlag = 0; 
	uint32_t doBitReverse = 1; 
	arm_cfft_radix4_instance_f32 fft_inst;  /* CFFT Structure instance */
	arm_cfft_radix4_init_f32(&fft_inst, FFT_LEN, ifftFlag, doBitReverse);
	
	// Apply Hamming Filter and compute power
	float32_t newSignalPower = 0.0;
	for (int i = 0; i < FFT_LEN; i++) {
	  newSignalPower += audioSampleInput [i * 2] * audioSampleInput [i * 2];
	  if (i == 0 || i == FFT_LEN - 1) {
		audioSampleInput [i * 2] = audioSampleInput [i * 2] * hammingFilter [0];
	  } else {
		audioSampleInput [i * 2] = audioSampleInput [i * 2] * (hammingFilter [i / 2] + hammingFilter [i / 2 + 1]) / 2.0;
	  }
	}
	newSignalPower /= FFT_LEN / 2.0;
    signalPower = signalPower + (newSignalPower - signalPower) / 60.0;
	
	/* Process the data through the CFFT/CIFFT module */ 
	arm_cfft_radix4_f32(&fft_inst, audioSampleInput);
	/* Process the data through the Complex Magnitude Module for  
	   calculating the magnitude at each bin */ 
	arm_cmplx_mag_f32(audioSampleInput, audioMagnitudeOutput, FFT_LEN);  
#ifdef FFT_DIAGNOSTICS
	fftMag = millis();
	Serial.print ("MAG:");
	for (int i = 0; i < FFT_LEN / 2; i++) {
		Serial.print (audioMagnitudeOutput [i]);
		if (i < FFT_LEN / 2 - 1) {
		  Serial.print (",");
		}
	}
	Serial.println ("|");
	Serial.print ("TIM:");
	Serial.print ((float32_t) (fftMag - fftStart));
	Serial.println ("|");
#endif
	updateFrequencyBuckets();
	updateSpectrumLevels();
  	for (int i = 0; i < TEST_LENGTH_SAMPLES; i++) {
	  audioSampleInput[i] = 0.0;
	}
	for (int i = 0; i < TEST_LENGTH_SAMPLES/2; i++) {
	  audioMagnitudeOutput[i] = 0.0;
	}
	// Turn the interrupt timer back on
	PIT_TCTRL3 |= 0x1;
#endif
  }  
}

//! Aggregate frequencies into their proper buckets
void towerPatterns::updateFrequencyBuckets() {
  float32_t binWidth = AUDIO_SAMPLE_RATE / FFT_LEN;
  int movingWindow = 1;
  float32_t buckets[BUCKET_COUNT];

  // 256 real observations going into 50 buckets
  buckets[0] = (int)(BUCKET_FACTOR);
  for (int i = 1; i < BUCKET_COUNT; i++) {
	buckets[i] = buckets[i - 1] * BUCKET_FACTOR;
  }
  for (int i = 1; i < BUCKET_COUNT; i++) {
	buckets[i] = min(FFT_LEN, buckets[i - 1] + round (buckets[i]));
  }
  for (int i = 0; i < BUCKET_COUNT; i++) {
	float32_t accum = 0.0;
	float32_t total = 0;
	uint16_t upper, lower;
	// Second half of result is imaginary space, that's why we used FFT_LEN / 2
	upper = min((uint16_t) buckets [i + 1], FFT_LEN);
	if (i >= movingWindow) {
	  lower = max(buckets[i - movingWindow], 2);
	} else {
	  lower = max(buckets[i], 2);
	}
	for (int j = lower; j < upper; j++) {
	  accum += abs(audioMagnitudeOutput[j]);
	  total++;
	}
    if (total > 0) {
        audioMagnitudeBuckets[i] = accum;
    } else {
        audioMagnitudeBuckets[i] = 0.0;
    }
  }
}

//! Update the LED levels based on the latest magnitudes
void towerPatterns::updateSpectrumLevels() {
  float32_t magnitude;
  float32_t maximum = 0.0;
  float32_t minimum = 1000.0;
  float32_t maximumLevel = log10 (11.0);
  float32_t maxLevelFactor = 0.85;
  float32_t runningValue = 0.0;
  float32_t runningCount = 0.0;

  // Shut down the 60Hz bucket
  //audioMagnitudeBuckets[2] = 0;
  for (int i = 1; i < BUCKET_COUNT; i++) {
	maximum = max(maximum, audioMagnitudeBuckets[i]);
	minimum = min(minimum, audioMagnitudeBuckets[i]);
  } 
  if (pattern == FS_ID_GIANT_SPECTRUM) {
	for (int i = 0; i < BUCKET_COUNT; i++) {
	  magnitude = min(1.0, log10 (10.0 * audioMagnitudeBuckets[i] / (maximum * maxLevelFactor) + 1.0) / maximumLevel);
	  if (i / (BUCKET_COUNT / spectrumTowerCount) + 1 == spectrumTowerId) {
		runningValue += magnitude;
		runningCount++;
	  }
	}
	if (runningCount > 0 && maximum > 0.0) {
	  saRunningAverage = ((runningValue / runningCount) / maximum) * (float) saThreshold;
	} else {
	  saRunningAverage = 0.0;
	}
  } else {
	//ledRed[0] = 0;
	//ledGreen[0] = 0;
	//ledBlue[0] = 200;
	float ascentRate = 0.9;
	float decentRate = 0.9;
	for (int i = 1; i < LED_COUNT; i++) {
	  //magnitude = min(1.0, log10 (10.0 * audioMagnitudeBuckets[i] / (maximum * maxLevelFactor) + 1.0) / maximumLevel);
	  magnitude = 20.0 * log10 (audioMagnitudeBuckets[i]);
	  magnitude -= SPECTRUM_MIN_DB;
	  magnitude = max(magnitude, 0.0);
	  magnitude /= (20 * log10 (maximum) - SPECTRUM_MIN_DB);
	  magnitude = min(1.0, magnitude);
	  if (magnitude * 255.0 > runningMagnitude[i]) {
		runningMagnitude[i] = runningMagnitude[i] + (magnitude * 255.0 - runningMagnitude[i]) * ascentRate;
	  } else {
		// Descend more slowly than ascending
		runningMagnitude[i] = runningMagnitude[i] + (magnitude * 255.0 - runningMagnitude[i]) * decentRate;
	  }
	  ledGreen[i] = (uint8_t) (runningMagnitude[i]);
	  ledBlue[i] = (uint8_t) (255.0 - runningMagnitude[i]);
	  if (runningMagnitude [i] == 255.0) {
		ledRed[i] = 0xFF;
		ledGreen[i] = 0x00;
	  } else if (runningMagnitude [i] > 0.95 * 255.0) {
		ledRed[i] = 0xFF;
		ledGreen[i] = 0x20;
	  } else if (runningMagnitude [i] > 0.85 * 255.0) {
		ledRed[i] = 0xFF;
		ledGreen[i] = 0x80;
	  } else if (runningMagnitude [i] > 0.70 * 255.0) {
		ledRed[i] = 0x7F;
	  } else {
		ledRed[i] = 0x00;
	  }
	}
  }
}

//! Update the searching eye pattern
void towerPatterns::iterateSearchingEye() {
  if (timeToDrop == 0 && timeToDrop2 == 0) {
	timeToDrop = patternSpeed;
	if (patternForward) {
	  cycleSpot++;
	  cycleSpot++;
	  cycleSpot++;
	  if (cycleSpot >= maxEye) {
		patternForward = false;
		cycleSpot = maxEye - 1;
		timeToDrop2 = unadjustedIntensity;
		maxEye = random (minEye, LED_COUNT);
	  }
	} else {
	  if (cycleSpot == minEye) {
		patternForward = true;
		timeToDrop2 = unadjustedIntensity;
		minEye = random (0, maxEye);
	  } else {
		cycleSpot--;
		if (cycleSpot != minEye) {
		  cycleSpot--;
		  if (cycleSpot != minEye) {
			cycleSpot--;
		  }
		}
	  }
	}
  } else {
	if (timeToDrop2 == 0) {
	  timeToDrop--;
	} else {
	  timeToDrop2--;
	}
  }
  for (int i = 0; i < LED_COUNT; i++) {
	ledRed[i] = 0;
	ledGreen[i] = 0;
	ledBlue[i] = 0;
	if (i == cycleSpot || i == cycleSpot - 1) {
	  ledRed[i] = adjustedRed;
	  ledGreen[i] = adjustedGreen;
	  ledBlue[i] = adjustedBlue;
	}
  }
}

//! Update the bubble wave pattern
void towerPatterns::iterateBubbleWave() {
}

//! Initialize the broken display pattern
void towerPatterns::initializeBroken() {
  brokenOff = millis() + random (200, 4000);
  brokenOn = brokenOff + random (200, 250);
}

//! Update the broken display pattern
void towerPatterns::iterateBroken() {
  unsigned long timestamp = millis();
  if (timestamp > brokenOn) {
	initializeBroken();
  } else if (timestamp > brokenOff) {
	for (int i = 0; i < LED_COUNT; i++) {
	  if (((1 << (7 - i % 8)) & unBrokenBits[i / 8]) || ((1 << (7 - i % 8)) & brokenBits[i / 8])) {
		ledRed[i] = 100; ledGreen[i] = 100; ledBlue[i] = 100;
	  } else {
		ledRed[i] = 0; ledGreen[i] = 0; ledBlue[i] = 0;
	  }
	}
  } else {
	for (int i = 0; i < LED_COUNT; i++) {
	  if ((1 << (7 - i % 8)) & brokenBits[i / 8]) {
		ledRed[i] = 0; ledGreen[i] = 0; ledBlue[i] = 0;
	  }
	}
  }
}

//! Iterate the pong pattern
void towerPatterns::iteratePong() {
  ball.step();
  for (int i = 0; i < LED_COUNT; i++) {
	if (ball.yourTurn(i)) {
	  ledRed[i] = adjustedRed; ledGreen[i] = adjustedGreen; ledBlue[i] = adjustedBlue;
	} else {
	  ledRed[i] = 0; ledGreen[i] = 0; ledBlue[i] = 0;
	}
  }
}

//! Initialize the flame animation pattern
void towerPatterns::initializeFlame() {
  for (int i = 0; i < LED_COUNT; i++) {
	ledRed[i] = 0;
	ledGreen[i] = 0;
	ledBlue[i] = 0;
  }
  frameRelease = patternSpeed;
  frameIndex = random (0, FLAME_FRAME_COUNT - 1);
}

//! Iterate the flame animation
void towerPatterns::iterateFlame() {
  iterateFrameStep(FLAME_FRAME_COUNT - 1);
  float iterationProportion = (float) frameRelease / (float) patternSpeed;
  int previousFrameIndex = frameIndex - 1;
  if (previousFrameIndex < 0) previousFrameIndex = FLAME_FRAME_COUNT - 1;
  float intensity = (float) unadjustedIntensity / 255.0;
  for (int i = 0; i < LED_COUNT; i++) {
	ledRed[LED_COUNT - 1 - i] = (uint8_t) (intensity * (float) unadjustedRed / 255.0 * 
										   ((float) flameFrames [frameIndex][i * 3] * (1.0 - iterationProportion) + 
											(float) flameFrames [previousFrameIndex][i * 3] * iterationProportion));
	ledGreen[LED_COUNT - 1 - i] = (uint8_t) (intensity * (float) unadjustedGreen / 255.0 * 
											 ((float) flameFrames [frameIndex][i * 3 + 1] * (1.0 - iterationProportion) + 
											  (float) flameFrames [previousFrameIndex][i * 3 + 1] * iterationProportion));
	ledBlue[LED_COUNT - 1 - i] = (uint8_t) (intensity * (float) unadjustedBlue / 255.0 * 
											((float) flameFrames [frameIndex][i * 3 + 2] * (1.0 - iterationProportion) + 
											 (float) flameFrames [previousFrameIndex][i * 3 + 2] * iterationProportion));
	if (ledRed[LED_COUNT - 1 - i] < 75) ledRed[LED_COUNT - 1 - i] = 0;
	if (ledGreen[LED_COUNT - 1 - i] < 10) ledGreen[LED_COUNT - 1 - i] = 0;
	if (ledBlue[LED_COUNT - 1 - i] < 75) ledBlue[LED_COUNT - 1 - i] = 0;
  }
}

//! Initialize the candle animation pattern
void towerPatterns::initializeCandle() {
  for (int i = 0; i < LED_COUNT - FLAME_HEIGHT; i++) {
	ledRed[i] = adjustedRed;
	ledGreen[i] = adjustedGreen;
	ledBlue[i] = adjustedBlue;
  }
}

//! Iterate the candle animation
void towerPatterns::iterateCandle() {
  frameRelease--;
  if (frameRelease <= 0 || frameRelease > patternSpeed) {
	frameRelease = random (patternSpeed / 2, patternSpeed);
	float flameLevel = (float) random (1, 255) / 255.0;
	for (int i = 0; i <= FLAME_HEIGHT; i++) {
	  audioMagnitudeBuckets[i] = audioMagnitudeBuckets[i + 1];
	}
	audioMagnitudeBuckets[FLAME_HEIGHT + 1] = 255.0 * flameLevel;
  }
  float iterationProportion = (float) frameRelease / (float) patternSpeed;
  for (int i = 0; i < FLAME_HEIGHT; i++) {
	ledRed[LED_COUNT - i - 1] = (uint8_t) ((float) audioMagnitudeBuckets[i + 1] * (1.0 - iterationProportion) + 
										   (float) audioMagnitudeBuckets[i] * iterationProportion);
	ledGreen[LED_COUNT - i - 1] = (uint8_t) ((float) audioMagnitudeBuckets[i + 1] * (1.0 - iterationProportion) + 
											 (float) audioMagnitudeBuckets[i] * iterationProportion);
	ledBlue[LED_COUNT - i - 1] = 0;
  }
}

/*
//! Iterate the candle animation
void towerPatterns::iterateCandle() {
  uint8_t flameHeight = 5;
  frameRelease--;
  if (frameRelease <= 0) {
	frameRelease = random (1, patternSpeed);
	float flameLevel = (float) random (100, 200) / 255.0;
	ledRed[LED_COUNT - flameHeight - 1] = ledRed[LED_COUNT - 1];
	ledGreen[LED_COUNT - flameHeight - 1] = ledGreen[LED_COUNT - 1];
	ledBlue[LED_COUNT - flameHeight - 1] = ledBlue[LED_COUNT - 1];
	ledRed[LED_COUNT - flameHeight - 2] = adjustedGreen * flameLevel;
	ledGreen[LED_COUNT - flameHeight - 2] = adjustedRed * flameLevel;
	ledBlue[LED_COUNT - flameHeight - 2] = adjustedBlue * flameLevel;
  }
  float iterationProportion = (float) frameRelease / (float) patternSpeed;
  for (int i = LED_COUNT - flameHeight; i < LED_COUNT; i++) {
	ledRed[i] = (uint8_t) ((float) ledRed[LED_COUNT - flameHeight - 2] * (1.0 - iterationProportion) + 
						   (float) ledRed[LED_COUNT - flameHeight - 1] * iterationProportion);
	ledGreen[i] = (uint8_t) ((float) ledGreen[LED_COUNT - flameHeight - 2] * (1.0 - iterationProportion) + 
							 (float) ledGreen[LED_COUNT - flameHeight - 1] * iterationProportion);
	//ledBlue[i] = (uint8_t)((float) ledBlue[LED_COUNT - flameHeight - 2] * (1.0 - iterationProportion) + 
	//					   (float) ledBlue[LED_COUNT - flameHeight - 1] * iterationProportion);
	ledBlue[i] = 0;
  }
}
*/

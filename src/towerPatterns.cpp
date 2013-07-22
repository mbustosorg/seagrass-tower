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
  //  if (dataLength > 6) {
  //for (unsigned long stopWatch = millis(); (millis() - stopWatch) < ((unsigned long) data [6] * 10); ) {}
  //}
  // Disable Timer 3 used for audio sampling, let the request turn it back on
  PIT_TCTRL3 &= ~(1 << 0);
  animations.isAnimating = false;
  messageType = (int) data[0];
  patternSpeed = (int) data[1];
  switch (messageType) {
  case FS_ID_TILT:
	patternSpeed = 255 - (int) data [1];
	setPatternSpeedWithFactor(10);
	if (pattern != data [0]) {
	  initializeTilt();
	  pattern = data [0];
	}
	break;
  case FS_ID_SHAKE_SPARKLE:
  case FS_ID_SPARKLER:
	//setPatternSpeedWithFactor(1);
	setPatternSpeedWithFactor(20);
	setBasicParameters (data[5], data[2], data[3], data[4]);
	pattern = data [0];
	break;
  case FS_ID_GRASS_WAVE:
	patternSpeed = 255 - (int) data [1];
	setPatternSpeedWithFactor(15);
	initializeGrassWave(data[5], data[2], data[3], data[4], pattern != data [0]);
	pattern = data [0];
	break;
  case FS_ID_RADIO_TOWER:
	if (pattern != data[0]) {
	  initializeRadioTower();
	}
	setBasicParameters(data[5], data[2], data[3], data[4]);
	pattern = data [0];
	break;
  case FS_ID_BOUNCING_BALL:
	if (pattern != data[0]) {
	  initializeBouncingBall();
	}
	if (data[2] == 255 && data[3] == 255 && data[4] == 255) {
	  useTiltForBounceColor = true;
	} else {
	  useTiltForBounceColor = false;
	  setBasicParameters(data[5], data[2], data[3], data[4]);
	}
 	pattern = data [0];
	break;
  case FS_ID_SPECTRUM_ANALYZER:
 	pattern = data [0];
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
	if (pattern != data[0]) {
	  timeToDropInitial = (uint8_t) random (1, patternSpeed);
	  timeToDrop = timeToDropInitial;
	}
 	pattern = data [0];
	break;
  case FS_ID_ANIMATE_1:
	animations.startAnimation(millis());
	initializePattern(animations.currentPattern(), 6);
	animations.isAnimating = true;
	break;
  case FS_ID_SEARCHING_EYE:
 	pattern = data [0];
	setPatternSpeedWithFactor(10);
	patternSpeed = random (patternSpeed, patternSpeed + 2);
	setBasicParameters(data[5], data[2], data[3], data[4]);
	break;
  case FS_ID_BUBBLE_WAVE:
	break;
  case FS_ID_BROKEN:
	break;
  default:
	furSwarmPatterns::initializePattern(data, dataLength);
	break;
  }
}

//! Continue pattern display
void towerPatterns::continuePatternDisplay() {
  // Continue with pattern display
  switch (pattern) {
  case FS_ID_TILT:
	//if (isShaking) {
	//  shakeSparkle();
	//} else if (wasShaking) {
	//  initializeTilt();
	//  wasShaking = false;
	//} else {
	  tilt();
	  //}
	displayData(true, true, true);
	//checkShaking();
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
  default:
	furSwarmPatterns::continuePatternDisplay();
	break;
  }
  // Check if we're in animation mode and what to do next
  if (animations.isAnimating) {
	if (animations.nextPattern(millis())) {
	  initializePattern(animations.currentPattern(), 6);
	}
	animations.isAnimating = true;
  }
}

//! Read the current tilt
void towerPatterns::readTilt() {
  xTilt = analogRead(xTiltPin) - 512.0 - xTiltCal;
  if (xTilt > TILT_BOUND) {
	xTilt = TILT_BOUND;
  } else if (xTilt < -TILT_BOUND) {
	xTilt = -TILT_BOUND;
  }
  yTilt = analogRead(yTiltPin) - 512.0 - yTiltCal;
  if (yTilt > TILT_BOUND) {
	yTilt = TILT_BOUND;
  } else if (yTilt < -TILT_BOUND) {
	yTilt = -TILT_BOUND;
  }
  zTilt = analogRead(zTiltPin) - 512.0 - zTiltCal;
  if (zTilt > TILT_BOUND) {
	zTilt = TILT_BOUND;
  } else if (zTilt < -TILT_BOUND) {
	zTilt = -TILT_BOUND;
  }
  calibrateTilt();
}

//! This is used for establishing a calibration opportunity
void towerPatterns::calibrateTilt() {
  uint32_t now = millis();
  bool tilting = abs(yTilt - yTiltCal) > tiltCalThreshold || abs(zTilt - zTiltCal) > tiltCalThreshold;
  if (tiltCalStart == 0 || tilting) {
	tiltCalStart = now;
	yLastTiltCal = yTilt;
	zLastTiltCal = zTilt;
  } else if (now - tiltCalStart > 5000) {
	xTiltCal = xTilt;
	yTiltCal = yTilt;
	zTiltCal = zTilt;
  }
}

//! Does the accelerometer detect shaking?
bool towerPatterns::checkShaking() {
  unsigned long timeStamp = millis();
  if (shakingTimestamp == 0) {
	shakingTimestamp = timeStamp;
  }
  if (isShaking && timeStamp - shakingStart > SHAKE_LENGTH) {
	isShaking = false;
	readTilt();
	xShakeTilt = xTilt;
	yShakeTilt = yTilt;
	zShakeTilt = zTilt;
  } else if (!isShaking && timeStamp - shakingTimestamp > SHAKE_DETECTION_TIME) {
	readTilt();
	shakingTimestamp = timeStamp;
	if (abs (xTilt - xShakeTilt) > SHAKE_THRESHOLD || 
		abs (yTilt - yShakeTilt) > SHAKE_THRESHOLD || 
		abs (zTilt - zShakeTilt) > SHAKE_THRESHOLD) {
	  shakingStart = timeStamp;
	  isShaking = true;
	  wasShaking = true;
	}
	xShakeTilt = xTilt;
	yShakeTilt = yTilt;
	zShakeTilt = zTilt;
  }
  return isShaking;
}

//! Initialize the tilt pattern
void towerPatterns::initializeTilt() {
  adjustedPatternSpeed = patternSpeed;
  timeToDrop = adjustedPatternSpeed;
  cycleSpot = 0;
}

//! Color struct for the current tilt
rgb towerPatterns::tiltColor() {
  readTilt();
  hsv in;
  if (zTilt == 0 && yTilt == 0) {
	in.h = 0.0;
  } else {
	in.h = atan2(zTilt, yTilt) * 360.0 / (2.0 * 3.14159);
  }
  in.s = 1.0;
  in.v = (yTilt / TILT_BOUND) * (yTilt / TILT_BOUND) + (zTilt / TILT_BOUND) * (zTilt / TILT_BOUND);
  if (in.v < 0.1) {
	in.v = 0.1;
  }
  return hsv2rgb(in);
}

//! Determine tilt and set colors
void towerPatterns::tilt() {
  if (timeToDrop == 0) {
	if (cycleSpot < LED_COUNT + 1) {
	  timeToDrop = patternSpeed / 3;
	  cycleSpot++;
	  if (patternSpeed < 100) {
		cycleSpot++;
	  }
	} else {
	  timeToDrop = patternSpeed;
	  lastRGBOut = currentRGBOut;
	  currentRGBOut = tiltColor();
	}
  } else {
	timeToDrop--;
  }
  float brightnessFactor = 7.0;
  float iterationProportion = (float) timeToDrop / (float) patternSpeed;
  rgb currentByte, lastByte;
  currentByte.r = min(currentRGBOut.r * 255.0 * brightnessFactor, 255); // Now in byte
  currentByte.g = min(currentRGBOut.g * 255.0 * brightnessFactor, 255); // Now in byte
  currentByte.b = min(currentRGBOut.b * 255.0 * brightnessFactor, 255); // Now in byte
  lastByte.r = min(lastRGBOut.r * 255.0 * brightnessFactor, 255); // Now in byte
  lastByte.g = min(lastRGBOut.g * 255.0 * brightnessFactor, 255); // Now in byte
  lastByte.b = min(lastRGBOut.b * 255.0 * brightnessFactor, 255); // Now in byte
  for (int i = 0; i < LED_COUNT; i++) {
	if (LED_COUNT - i - 1 < cycleSpot) {
	  ledRed[i] = (uint8_t) (lastByte.r * iterationProportion) + (uint8_t) (currentByte.r * (1.0 - iterationProportion));
	  ledGreen[i] = (uint8_t) (lastByte.g * iterationProportion) + (uint8_t) (currentByte.g * (1.0 - iterationProportion));
	  ledBlue[i] = (uint8_t) (lastByte.b * iterationProportion) + (uint8_t) (currentByte.b * (1.0 - iterationProportion));
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
  for (int i = 0; i < LED_COUNT; i++) {
	ledRed[i] = 0;
	ledGreen[i] = 0;
	ledBlue[i] = 0;
    if (i == (uint8_t)thisBallLocation || i == (uint8_t)nextBallLocation) {
	  if (useTiltForBounceColor) {
		ledRed[i] = (uint8_t) (currentRGBOut.r * 255.0);
		ledGreen[i] = (uint8_t) (currentRGBOut.g * 255.0);
		ledBlue[i] = (uint8_t) (currentRGBOut.b * 255.0);
	  } else {
		ledRed[i] = unadjustedRed;
		ledGreen[i] = unadjustedGreen;
		ledBlue[i] = unadjustedBlue;
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
  radioPeriod = random (3490, 3500);
}

//! Iterate the radio tower pattern
void towerPatterns::iterateRadioTower() {
  uint32_t currentTimestamp = millis();
  for (int i = 0; i < LED_COUNT; i++) {
	ledRed[i] = 0;
	ledGreen[i] = 0;
	ledBlue[i] = 0;
	if (currentTimestamp > radioTowerStart + radioPeriod) {
	  radioTowerStart = radioTowerStart + radioPeriod;
	} else if (currentTimestamp < radioTowerStart + 1000 && i > LED_COUNT - 3) {
	  ledRed[i] = adjustedRed;
	  ledGreen[i] = adjustedGreen;
	  ledBlue[i] = adjustedBlue;
	}
  }
}

//! Iterate the spectrum analyzer pattern
void towerPatterns::iterateSpectrumAnalyzer() {
  if (audioSampleInputIndex == 0) {
#ifndef NOT_EMBEDDED
	uint32_t ifftFlag = 0; 
	uint32_t doBitReverse = 1; 
	arm_cfft_radix4_instance_f32 fft_inst;  /* CFFT Structure instance */
	arm_cfft_radix4_init_f32(&fft_inst, FFT_LEN, ifftFlag, doBitReverse);
	/* Process the data through the CFFT/CIFFT module */ 
	arm_cfft_radix4_f32(&fft_inst, audioSampleInput);
	/* Process the data through the Complex Magnitude Module for  
	   calculating the magnitude at each bin */ 
	arm_cmplx_mag_f32(audioSampleInput, audioMagnitudeOutput, FFT_LEN);  
	updateFrequencyBuckets();
	updateSpectrumLevels();
  	for (int i = 0; i < TEST_LENGTH_SAMPLES; i++) {
	  audioSampleInput[i] = 0.0;
	}
	for (int i = 0; i < TEST_LENGTH_SAMPLES/2; i++) {
	  audioMagnitudeOutput[i] = 0.0;
	}
#endif
	// Turn the interrupt timer back on
	PIT_TCTRL3 |= 0x1;
  }  
}

//! Update the LED levels based on the latest magnitudes
void towerPatterns::updateSpectrumLevels() {
  float32_t magnitude;
  for (int i = 0; i < LED_COUNT; i++) {
	magnitude = min(1.0, audioMagnitudeBuckets[i] / 100.0);
	ledGreen[i] = (uint8_t) (magnitude * 255.0);
	ledBlue[i] = (uint8_t) ((1.0 - magnitude) * 255.0);
	if (magnitude == 1.0) {
	  ledRed[i] = 0xFF;
	  ledGreen[i] = 0x80;
	} else if (magnitude > 0.95) {
	  ledRed[i] = 0x7F;
	} else {
	  ledRed[i] = 0x00;
	}
  }
}

//! Aggregate frequencies into their proper buckets
void towerPatterns::updateFrequencyBuckets() {
  // Compute frequency buckets
  audioMagnitudeBuckets[0] = (int)(BUCKET_FACTOR);
  for (int i = 1; i < BUCKET_COUNT; i++) {
	audioMagnitudeBuckets[i] = audioMagnitudeBuckets[i - 1] * BUCKET_FACTOR;
  }
  for (int i = 1; i < BUCKET_COUNT; i++) {
	audioMagnitudeBuckets[i] = audioMagnitudeBuckets[i - 1] + (int)(audioMagnitudeBuckets[i]);
  }
  for (int i = 0; i < BUCKET_COUNT; i++) {
	float32_t accum = 0.0;
	uint16_t total = 0;
	uint16_t upper;
	if (i == BUCKET_COUNT - 1) {
	  upper = FFT_LEN / 2;
	} else {
	  upper = (uint16_t) audioMagnitudeBuckets [i + 1];
	}
	for (int j = audioMagnitudeBuckets[i]; j < upper; j++) {
	  accum += audioMagnitudeOutput[j];
	  total++;
	}
	audioMagnitudeBuckets[i] = accum / total;
  }
}

//! Update the searching eye pattern
void towerPatterns::iterateSearchingEye() {
  uint8_t nextCycleSpot = 0;
  if (timeToDrop == 0 && timeToDrop2 == 0) {
	timeToDrop = patternSpeed;
	if (patternForward) {
	  cycleSpot++;
	  cycleSpot++;
	  cycleSpot++;
	  if (cycleSpot >= LED_COUNT) {
		patternForward = false;
		cycleSpot = LED_COUNT - 1;
		timeToDrop2 = unadjustedIntensity;
	  }
	} else {
	  if (cycleSpot == 0) {
		patternForward = true;
		timeToDrop2 = unadjustedIntensity;
	  } else {
		cycleSpot--;
		if (cycleSpot != 0) {
		  cycleSpot--;
		  if (cycleSpot != 0) {
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
	if (i == cycleSpot) {
	  ledRed[i] = adjustedRed;
	  ledGreen[i] = adjustedGreen;
	  ledBlue[i] = adjustedBlue;
	}
  }
}

//! Update the bubble wave pattern
void towerPatterns::iterateBubbleWave() {
}

//! Update the broken display pattern
void towerPatterns::iterateBroken() {
}

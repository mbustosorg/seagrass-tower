/*

  towerAnimations.cpp

  $Date: 2013-06-21 21:38:54 -0700 (Fri, 21 Jun 2013) $
  $Revision: 983 $
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

#include "towerAnimations.h"

const int animationCount = 13;

const uint8_t animationPatterns[animationCount][ANIMATION_COMMAND_LENGTH] =
  {
	// ID, Speed, Red, Green, Blue, Intensity, Delay
	{FS_ID_BOUNCING_BALL, 253, 255, 10, 10, 120, 0},
	{FS_ID_BOUNCING_BALL, 253, 255, 10, 10, 90, 0},
	{FS_ID_BOUNCING_BALL, 9, 255, 10, 10, 120, 0},
	{FS_ID_STARFIELD, 10, 0, 0, 0, 0, 0},
	{FS_ID_RAINBOW_CHASE, 100, 0, 0, 150, 200, 0},
	{FS_ID_SPIRAL, 100, 200, 0, 40, 120, 0},
	{FS_ID_RADIO_TOWER, 100, 255, 0, 0, 200, 0},
	//{FS_ID_SPECTRUM_ANALYZER, 100, 200, 50, 0, 250, 0},
	{FS_ID_CYLON, 100, 200, 50, 0, 120, 0},
	{FS_ID_SEARCHING_EYE, 5, 0, 200, 0, 250, 0},
	{FS_ID_RANDOM_FLASH, 250, 200, 10, 130, 200, 0},
	{FS_ID_SHAKE_SPARKLE, 100, 0, 0, 220, 220, 0},
	{FS_ID_TILT, 120, 255, 100, 130, 255, 0},
	{FS_ID_BREATHE, 100, 20, 10, 200, 200, 0}
  };

//! Milliseconds for indexed pattern
const uint32_t animationTimes[] = 
  {
	120000,
	120000,
	120000,
	300000,
	300000,
	300000,
	200000,
	//300000,
	300000,
	300000,
	300000,
	300000,
	180000,
	300000,
  };

//! Start the animation at specific time
void towerAnimations::startAnimation(unsigned long timestamp) {
  animationStart = timestamp;
  animationPosition = 0;
  isAnimating = true;
}

//! Currently requested pattern
uint8_t* towerAnimations::currentPattern() {
  return (uint8_t *)animationPatterns[animationPosition];
}

//! The next pattern of the cycle if we're far enough along
uint8_t* towerAnimations::nextPattern(unsigned long timestamp) {
  if (timestamp - animationStart > animationTimes[animationPosition]) {
	animationPosition++;
	animationStart = timestamp;
	if (animationPosition >= animationCount) animationPosition = 0;
	return currentPattern();
  } else {
	return 0;
  }
}


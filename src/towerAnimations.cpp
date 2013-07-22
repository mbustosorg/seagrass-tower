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

const int animationCount = 3;

const uint8_t animationPatterns[animationCount][6] =
  {
	{FS_ID_BOUNCING_BALL, 250, 200, 10, 130, 200},
	{FS_ID_STARFIELD, 10, 0, 0, 0, 0},
	{FS_ID_SPIRAL, 100, 200, 0, 40, 0}
  };

const uint32_t animationTimes[] = 
  {
	10000,
	10000,
	10000
  };

//! Start the animation at specific time
void towerAnimations::startAnimation(unsigned long timestamp) {
  animationStart = timestamp;
  animationPosition = 0;
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


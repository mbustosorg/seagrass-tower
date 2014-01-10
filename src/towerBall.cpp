/*

  towerBall.cpp

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

#include "towerBall.h"

towerBall::towerBall() {
  initialize(0, 0);
}

void towerBall::initialize(uint8_t number, uint8_t count) {
  participantCount = count;
  participantNumber = number;
  xPosition = 0.0;
  yPosition = 0.0;
  paddlePosition1 = 0.0;
  paddlePosition2 = 0.0;
  xSpeed = 0.0;
  ySpeed = 0.0;
}

bool towerBall::yourTurn(int pixel) {
  int ballPosition = (int) (yPosition / 255.0 * 50);
  int paddlePosition1int = (int) (paddlePosition1 / 255.0 * 50);
  int paddlePosition2int = (int) (paddlePosition2 / 255.0 * 50);
  if (participantNumber == 1) {
	return pixel < paddlePosition1int + 3 && pixel > paddlePosition1int - 3;
  } else if (participantNumber == participantCount) {
	return pixel < paddlePosition2int + 3 && pixel > paddlePosition2int - 3;
  } else if ((int) (xPosition / 255.0 * participantCount) == participantNumber - 1) {
	return pixel == ballPosition;
  } else {
	return false;
  }
}

void towerBall::step() {
  xPosition = xPosition + xSpeed;
  yPosition = yPosition + ySpeed;

  paddlePosition1 = paddlePosition1 + paddleSpeed1;
  paddlePosition2 = paddlePosition2 + paddleSpeed2;

  if (paddlePosition1 > 255) {
	paddlePosition1 = 255;
	paddleSpeed1 = -paddleSpeed1;
  } else if (paddlePosition1 < 0) {
	paddlePosition1 = 0;
	paddleSpeed1 = -paddleSpeed1;
  }
  if (paddlePosition2 > 255) {
	paddlePosition2 = 255;
	paddleSpeed2 = -paddleSpeed2;
  } else if (paddlePosition2 < 0) {
	paddlePosition2 = 0;
	paddleSpeed2 = -paddleSpeed2;
  }

  if (xPosition > 255) {
	xPosition = 255;
	xSpeed = -xSpeed;
	paddleSpeed1 = ySpeed;
	paddleSpeed2 = -ySpeed;
  } else if (xPosition < 0) {
	xPosition = 0;
	xSpeed = -xSpeed;
	paddleSpeed1 = -ySpeed;
	paddleSpeed2 = ySpeed;
  }
  if (yPosition > 255) {
	yPosition = 255;
	ySpeed = -ySpeed;
  } else if (yPosition < 0) {
	yPosition = 0;
	ySpeed = -ySpeed;
  }
}

void towerBall::setSpeed (float newXspeed, float newYspeed) {
  xSpeed = newXspeed;
  ySpeed = newYspeed;
  paddleSpeed1 = newYspeed;
  paddleSpeed2 = newYspeed;
}


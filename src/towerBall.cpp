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


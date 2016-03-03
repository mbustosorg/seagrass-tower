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

#ifndef towerBall_h
#define towerBall_h

#include <stdint.h>

class towerBall {

 public:

  towerBall();
  void initialize(uint8_t number, uint8_t count);

  void step();
  bool yourTurn(int pixel);
  void setSpeed (float newXspeed, float newYspeed);

  float paddlePosition1, paddlePosition2;
  float xPosition, yPosition;

  float xSpeed, ySpeed;
  float paddleSpeed1, paddleSpeed2;
  uint8_t participantCount = 0;
  uint8_t participantNumber = 0;

};

#endif

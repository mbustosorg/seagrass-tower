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

#ifndef towerAnimations_h
#define towerAnimations_h

#include <stdint.h>
#include "furSwarmPatternConst.h"
#include "embeddedInterface.h"

#define ANIMATION_COMMAND_LENGTH (7)

class towerAnimations {

 public:

  void startAnimation(unsigned long timestamp);
  uint8_t* currentPattern();
  uint8_t* nextPattern(unsigned long timestamp);

  bool isAnimating = false;
  uint32_t animationStart = 0;
  uint8_t animationPosition = 0;
};

#endif

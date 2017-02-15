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

#include "towerAnimations.h"

#if defined FS_TOWER && !defined FS_TOWER_EYE
const int animationCount = 18;
#elif defined FS_TOWN_CENTER
const int animationCount = 15;
#elif defined FS_TOWER_EYE
const int animationCount = 15;
#elif defined FS_ROTOFUEGO
const int animationCount = 10;
#else
const int animationCount = 1;
#endif

const uint8_t animationPatterns[animationCount][ANIMATION_COMMAND_LENGTH] = {
#if defined FS_TOWER && !defined FS_TOWER_EYE
  // ID, Speed, Red, Green, Blue, Intensity, Delay
  {FS_ID_BOUNCING_BALL, 253, 255, 10, 10, 120, 0},
  {FS_ID_BOUNCING_BALL, 253, 0, 255, 0, 120, 0},
  {FS_ID_BOUNCING_BALL, 9, 255, 10, 10, 120, 0},
  {FS_ID_STARFIELD, 10, 0, 0, 0, 0, 0},
  {FS_ID_RAINBOW_CHASE, 100, 0, 0, 150, 200, 0},
  {FS_ID_SPIRAL, 100, 200, 0, 40, 120, 0},
  {FS_ID_RADIO_TOWER, 100, 255, 0, 0, 200, 0},
  {FS_ID_SPECTRUM_ANALYZER, 100, 200, 50, 0, 250, 0},
  {FS_ID_CYLON, 100, 200, 50, 0, 120, 0},
  {FS_ID_CYLON, 150, 50, 220, 75, 120, 0},
  {FS_ID_SEARCHING_EYE, 5, 0, 200, 0, 250, 0},
  {FS_ID_GRASS_WAVE, 100, 0, 200, 0, 250, 0},
  {FS_ID_GRASS_WAVE, 100, 150, 0, 200, 250, 0},
  {FS_ID_RANDOM_FLASH, 250, 200, 10, 130, 200, 0},
  {FS_ID_SHAKE_SPARKLE, 100, 0, 0, 220, 220, 0},
  {FS_ID_TILT, 120, 255, 100, 130, 255, 0},
  {FS_ID_FLAME, 20, 250, 100, 10, 255, 0},
  {FS_ID_BREATHE, 100, 20, 10, 200, 200, 0}
#elif defined FS_TOWN_CENTER
  // ID, Speed, Red, Green, Blue, Intensity, Delay
  {FS_ID_FULL_COLOR, 255, 0, 0, 255, 255, 0},
  {FS_ID_FULL_COLOR, 255, 0, 255, 255, 150, 0},
  {FS_ID_FULL_COLOR, 255, 0, 255, 0, 255, 0},
  {FS_ID_FULL_COLOR, 255, 255, 0, 0, 255, 0},
  {FS_ID_CYLON_PONG, 81, 255, 0, 0, 255, 0},
  {FS_ID_CYLON_PONG, 81, 0, 0, 255, 255, 0},
  {FS_ID_CYLON_PONG, 81, 255, 255, 255, 255, 0},
  {FS_ID_CYLON, 51, 255, 0, 153, 255, 0},
  {FS_ID_CYLON, 214, 255, 119, 0, 255, 0},
  {FS_ID_CYLON, 147, 104, 45, 255, 255, 0},
  {FS_ID_RAINBOW_CHASE, 147, 255, 255, 255, 255, 0},
  {FS_ID_BOUNCING_BALL, 126, 255, 0, 0, 123, 0},
  {FS_ID_SPARKLER, 126, 134, 0, 255, 123, 0},
  {FS_ID_GRASS_WAVE, 218, 0, 255, 0, 255, 0},
  {FS_ID_SPARKLE, 163, 0, 255, 0, 255, 0}
#elif defined FS_TOWER_EYE
    // ID, Speed, Red, Green, Blue, Intensity, Delay
  {FS_ID_FULL_COLOR, 255, 0, 0, 255, 255, 0},
  {FS_ID_FULL_COLOR, 255, 0, 255, 255, 255, 0},
  {FS_ID_FULL_COLOR, 255, 0, 255, 0, 255, 0},
  {FS_ID_FULL_COLOR, 255, 255, 0, 0, 255, 0},
  {FS_ID_CYLON_PONG, 81, 255, 0, 0, 255, 0},
  {FS_ID_CYLON_PONG, 81, 0, 0, 255, 255, 0},
  {FS_ID_CYLON_PONG, 81, 255, 255, 255, 255, 0},
  {FS_ID_CYLON, 51, 255, 0, 153, 255, 0},
  {FS_ID_CYLON, 214, 255, 119, 0, 255, 0},
  {FS_ID_CYLON, 147, 104, 45, 255, 255, 0},
  {FS_ID_RAINBOW_CHASE, 147, 255, 255, 255, 255, 0},
  {FS_ID_STARFIELD, 74, 255, 255, 255, 123, 0},
  {FS_ID_SPARKLER, 126, 134, 0, 255, 123, 0},
  {FS_ID_MATRIX, 128, 255, 255, 255, 255, 0},
  {FS_ID_SPARKLE, 163, 0, 255, 0, 255, 0} 
#elif defined FS_ROTOFUEGO
    // ID, Speed, Red, Green, Blue, Intensity, Delay
  {FS_ID_FULL_COLOR, 255, 0, 0, 255, 255, 0},
  {FS_ID_FULL_COLOR, 255, 0, 255, 255, 255, 0},
  {FS_ID_FULL_COLOR, 255, 0, 255, 0, 255, 0},
  {FS_ID_FULL_COLOR, 255, 255, 0, 0, 255, 0},
  {FS_ID_CYLON, 51, 255, 0, 153, 255, 0},
  {FS_ID_CYLON, 214, 255, 119, 0, 255, 0},
  {FS_ID_CYLON, 147, 104, 45, 255, 255, 0},
  {FS_ID_RAINBOW_CHASE, 147, 255, 255, 255, 255, 0},
  {FS_ID_STARFIELD, 74, 255, 255, 255, 123, 0},
  {FS_ID_SPARKLER, 126, 134, 0, 255, 123, 0},
#else
  {FS_ID_CYLON, 150, 50, 220, 75, 120, 0},
#endif
};

//! Milliseconds for indexed pattern
const uint32_t animationTimes[] = {
#ifdef FS_TOWER
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  300000,
  300000,
  300000,
  200000,
  120000,
  300000,
  300000,
  300000,
  300000,
  300000,
  300000,
  300000,
  180000,
  180000,
  300000
#elif defined FS_TOWN_CENTER
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000
#elif defined FS_TOWER_EYE
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000
#elif defined FS_ROTOFUEGO
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000,
  120000
#else
  300000
#endif
};

//! Constructor
towerAnimations::towerAnimations() {
  isAnimating = false;
  animationStart = 0;
  animationPosition = 0;
}

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


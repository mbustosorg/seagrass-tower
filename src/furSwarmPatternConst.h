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

#ifndef furSwarm_furSwarmPatternConst_h
#define furSwarm_furSwarmPatternConst_h

#define FS_ID_NULL (0x00)
#define FS_ID_MESSAGE_TYPE_FULL (0x01)
#define FS_ID_MESSAGE_TYPE_SHORT_VERSION (0x02)
#define FS_ID_HEARTBEAT_REQUEST (0x02)
#define FS_ID_FULL_COLOR (0x03)
#define FS_ID_SPARKLE (0x04)
#define FS_ID_DESCEND (0x05)
#define FS_ID_OFF (0x06)
#define FS_ID_FLASH (0x07)
#define FS_ID_FIRE (0x08)
#define FS_ID_HEART (0x09)
#define FS_ID_BREATHE (0x0a)
#define FS_ID_ORGANIC (0x0b)
#define FS_ID_CYLON (0x0c)
#define FS_ID_DROP (0x0d)
#define FS_ID_CHARACTER (0x0e)
#define FS_ID_CYLON_VERTICAL (0x0f)
#define FS_ID_CYLON_PONG (0x10)
#define FS_ID_BREATHE_EVOLVE (0x11)
#define FS_ID_WORD (0x12)
#define FS_ID_SOUND_ACTIVATE (0x13)
#define FS_ID_ANIMATE_1 (0x14)
#define FS_ID_TEST_PATTERN (0x15)
#define FS_ID_PRISM (0x16)
#define FS_ID_PRISM_DISTANCE (0x17)
#define FS_ID_MATRIX (0x18)
#define FS_ID_RAINBOW_CHASE (0x19)
#define FS_ID_RANDOM_FLASH (0x1a)
#define FS_ID_IMAGE_SCROLL (0x1b)
#define FS_ID_STARFIELD (0x1c)
#define FS_ID_SPIRAL (0x1d)
#define FS_ID_TILT (0x1e)
#define FS_ID_SHAKE_SPARKLE (0x1f)
#define FS_ID_SPARKLER (0x20)
#define FS_ID_GRASS_WAVE (0x21)
#define FS_ID_RADIO_TOWER (0x22)
#define FS_ID_BOUNCING_BALL (0x23)
#define FS_ID_SPECTRUM_ANALYZER (0x24)
#define FS_ID_FOREST_RUN (0x25)
#define FS_ID_SEARCHING_EYE (0x26)
#define FS_ID_BUBBLE_WAVE (0x27)
#define FS_ID_BROKEN (0x28)
#define FS_ID_PONG (0x29)
#define FS_ID_GIANT_SPECTRUM (0x2a)
#define FS_ID_FLAME (0x2b)
#define FS_ID_CANDLE (0x2c)
#define FS_ID_SHAKING (0x2d)
#define FS_ID_DANCING (0x2e)
#define FS_ID_POOF_1 (0x2f)
#define FS_ID_POOF_2 (0x30)
#define FS_ID_MAX_ID (0x30)
#define FS_ID_MAX_ID_COUNT (0x31)

#define FS_NAME_NULL ""
#define FS_NAME_MESSAGE_TYPE_FULL "Full HB"
#define FS_NAME_MESSAGE_TYPE_SHORT_VERSION "Short HB"
#define FS_NAME_FULL_COLOR "Full Color"
#define FS_NAME_SPARKLE "Sparkle"
#define FS_NAME_DESCEND "Descend"
#define FS_NAME_OFF "Off"
#define FS_NAME_FLASH "Flash"
#define FS_NAME_FIRE "Fire"
#define FS_NAME_HEART "Heart"
#define FS_NAME_BREATHE "Breathe"
#define FS_NAME_ORGANIC "Organic"
#define FS_NAME_CYLON "Cylon"
#define FS_NAME_DROP "Drop"
#define FS_NAME_CHARACTER "Character"
#define FS_NAME_CYLON_VERTICAL "CylonV"
#define FS_NAME_CYLON_PONG "CylonP"
#define FS_NAME_BREATHE_EVOLVE "Evolve"
#define FS_NAME_WORD "Word"
#define FS_NAME_SOUND_ACTIVATE "Sound"
#define FS_NAME_ANIMATE_1 "Animate 1"
#define FS_NAME_TEST_PATTERN "Test"
#define FS_NAME_PRISM "Prism"
#define FS_NAME_PRISM_DISTANCE "Dist"
#define FS_NAME_MATRIX "Matrix"
#define FS_NAME_RAINBOW_CHASE "RChase"
#define FS_NAME_RANDOM_FLASH "RFlash"
#define FS_NAME_IMAGE_SCROLL "iScroll"
#define FS_NAME_STARFIELD "Starfield"
#define FS_NAME_SPIRAL "Spiral"
#define FS_NAME_TILT "Tilt"
#define FS_NAME_SHAKE_SPARKLE "ShSparkle"
#define FS_NAME_SPARKLER "Sparkler"
#define FS_NAME_GRASS_WAVE "Grass Wave"
#define FS_NAME_RADIO_TOWER "RadioTwr"
#define FS_NAME_BOUNCING_BALL "Bouncing"
#define FS_NAME_SPECTRUM_ANALYZER "Spectrum"
#define FS_NAME_FOREST_RUN "Forest"
#define FS_NAME_SEARCHING_EYE "Search"
#define FS_NAME_BUBBLE_WAVE "Bubble"
#define FS_NAME_BROKEN "Broken"
#define FS_NAME_PONG "Pong"
#define FS_NAME_GIANT_SPECTRUM "GSpectrum"
#define FS_NAME_FLAME "Flame"
#define FS_NAME_CANDLE "Candle"
#define FS_NAME_SHAKING "Shaking"
#define FS_NAME_DANCING "Dancing"
#define FS_NAME_POOF_1 "Poof 1"
#define FS_NAME_POOF_2 "Poof 2"

#define MAX_PATTERN_NAME_LENGTH (11)
static const char patternNames[FS_ID_MAX_ID + 1][MAX_PATTERN_NAME_LENGTH] = {
  FS_NAME_NULL,
  FS_NAME_MESSAGE_TYPE_FULL,
  FS_NAME_MESSAGE_TYPE_SHORT_VERSION,
  FS_NAME_FULL_COLOR,
  FS_NAME_SPARKLE,
  FS_NAME_DESCEND,
  FS_NAME_OFF,
  FS_NAME_FLASH,
  FS_NAME_FIRE,
  FS_NAME_HEART,
  FS_NAME_BREATHE,
  FS_NAME_ORGANIC,
  FS_NAME_CYLON,
  FS_NAME_DROP,
  FS_NAME_CHARACTER,
  FS_NAME_CYLON_VERTICAL,
  FS_NAME_CYLON_PONG,
  FS_NAME_BREATHE_EVOLVE,
  FS_NAME_WORD,
  FS_NAME_SOUND_ACTIVATE,
  FS_NAME_ANIMATE_1,
  FS_NAME_TEST_PATTERN,
  FS_NAME_PRISM,
  FS_NAME_PRISM_DISTANCE,
  FS_NAME_MATRIX,
  FS_NAME_RAINBOW_CHASE,
  FS_NAME_RANDOM_FLASH,
  FS_NAME_IMAGE_SCROLL,
  FS_NAME_STARFIELD,
  FS_NAME_SPIRAL,
  FS_NAME_TILT,
  FS_NAME_SHAKE_SPARKLE,
  FS_NAME_SPARKLER,
  FS_NAME_GRASS_WAVE,
  FS_NAME_RADIO_TOWER,
  FS_NAME_BOUNCING_BALL,
  FS_NAME_SPECTRUM_ANALYZER,
  FS_NAME_FOREST_RUN,
  FS_NAME_SEARCHING_EYE,
  FS_NAME_BUBBLE_WAVE,
  FS_NAME_BROKEN,
  FS_NAME_PONG,
  FS_NAME_GIANT_SPECTRUM,
  FS_NAME_FLAME,
  FS_NAME_CANDLE,
  FS_NAME_SHAKING,
  FS_NAME_DANCING,
  FS_NAME_POOF_1,
  FS_NAME_POOF_2
};

#define FS_TYPE_VEST (0x01)
#define FS_TYPE_HAT (0x02)
#define FS_TYPE_TOWER (0x03)
#define FS_TYPE_TOWN_CENTER (0x04)
#define FS_TYPE_TOWER_EYE (0x05)
#define FS_TYPE_DRESS (0x06)
#define FS_TYPE_WINDFLOWERS (0x07)
#define FS_TYPE_REEDS (0x08)

#endif

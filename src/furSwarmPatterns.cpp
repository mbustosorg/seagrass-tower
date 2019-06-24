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

#include "embeddedInterface.h"
#include "furSwarmPatterns.h"
#include "furSwarmMemberConst.h"
#include "furSwarmPatternConst.h"
#include <string.h>
#include <stdlib.h>

furSwarmPatterns::furSwarmPatterns() {
    
    pattern = 1;
    triggerPattern = 0;
    failedMessageCount = 0;
    delayStopwatch = 0;
    lastDataLength = 0;
    frameIndex = 0;
    prismHue = 0.0;
    characterIndexUpper = 26;
    maxEye = LED_COUNT;
    minEye = 0;
    timeToDrop = 0;
    cycleSpot = 0;
    blendingStart = 0;
    
    ledChangeRate = 3;
    flashLedChangeRate = 3;
    saThreshold = 15000;
    saTargetThreshold = saThreshold * 0.90;
    saHighResetThreshold = false;
    saMidResetThreshold = false;
    saLowResetThreshold = false;
    // Sound Activate - Gain
    saGain = 1.0;
    
#define SA_GAIN_UPPER 20.0
#define SA_GAIN_LOWER 0.2
#define SA_PRESSURE_MAX 12.0
    // Sound Activate - Sampling
#define SA_MIDDLE_VALUE 512 // 0.5 * 1024
    saMovingAverage = 15000;
    // Sound Activate - Running Average
    saRunningAverage = 15000;
    previousSoundLevel = 0;
    peakSoundLevel = 0;
    soundLevelRetractionTime = 0;
    peakRetractionTime = 0;
    saNumberOfSamples = 350;
    saMovingAverageCount = 10;
    saAveragedOver = 3;

    for (int i = 0; i < LED_COUNT; i++) {
      blendingLedRed[i] = 0;
      blendingLedGreen[i] = 0;
      blendingLedBlue[i] = 0;
      ledRed[i] = 0;
      ledGreen[i] = 0;
      ledBlue[i] = 0;
      ledCycleColor[i] = 0;
      ledRedDirection[i] = 0;
      ledGreenDirection[i] = 0;
      ledBlueDirection[i] = 0;
      breatheBorderRed[i] = 0;
      breatheBorderGreen[i] = 0;
      breatheBorderBlue[i] = 0;
      lastLedRed[i] = 0;
      lastLedGreen[i] = 0;
      lastLedBlue[i] = 0;
    }

}

//! Set `secondsIntoMinute'
void furSwarmPatterns::setSecondsIntoMinute(unsigned long newSecondsIntoMinute) {
    secondsIntoMinute = newSecondsIntoMinute;
}

// Iterate the transition point for patterns that request it
void furSwarmPatterns::iterateForTransition() {
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

//! Set the LED color strand to a fixed full color set
void furSwarmPatterns::setfullStrand(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue, bool smooth) {
    if (transitionRequested) iterateForTransition();
    leds.sendStartFrame();
    for (int i = 0; i < LED_COUNT; i++) {
        if (!transitionRequested || LED_COUNT - i < cycleSpot) {
            if (smooth && frameRelease > 0) {
                frameRelease--;
                float proportion = (float) frameRelease / ((float) patternSpeed * SMOOTH_FACTOR_FLOAT);
                leds.sendColor(i,
                          ledRed[i] + ((float) red * intensity / 255 - (float) ledRed[i]) * (1.0 - proportion),
                          ledGreen[i] + ((float) green * intensity / 255 - (float) ledGreen[i]) * (1.0 - proportion),
                          ledBlue[i] + ((float) blue * intensity / 255 - (float) ledBlue[i]) * (1.0 - proportion));
            } else {
                ledRed[i] = red * intensity / 255;
                ledGreen[i] = green * intensity / 255;
                ledBlue[i] = blue * intensity / 255;
                leds.sendColor(i, ledRed[i], ledGreen[i], ledBlue[i]);
            }
        }
    }
    leds.sendEndFrame();
}

//! Cycle through in a descending pattern on the LED data
void furSwarmPatterns::descendData(int changeRate) {
    for (int i = 0; i < LED_COUNT; i++) {
        ledRed[i] = ledRed[i] - changeRate;
        ledGreen[i] = ledGreen[i] - changeRate;
        ledBlue[i] = ledBlue[i] - changeRate;
    }
}

//! Cycle through in an in/out pattern on the LED data
void furSwarmPatterns::cycleData(int changeRate, bool upperAdjust, uint8_t stayBelow, uint8_t stayAbove) {
    for (int i = 0; i < LED_COUNT; i++) {
        cycleDataColor(ledRed, ledRedDirection, breatheBorderRed, i, changeRate, upperAdjust, stayBelow, stayAbove);
        cycleDataColor(ledGreen, ledGreenDirection, breatheBorderGreen, i, changeRate, upperAdjust, stayBelow, stayAbove);
        cycleDataColor(ledBlue, ledBlueDirection, breatheBorderBlue, i, changeRate, upperAdjust, stayBelow, stayAbove);
    }
}

//! Cycle through in an in/out pattern on the LED data scaling the colors separately
void furSwarmPatterns::cycleDataScaled(int changeRate, bool upperAdjust, uint8_t stayBelow, uint8_t stayAbove) {
    for (int i = 0; i < LED_COUNT; i++) {
        cycleDataColor(ledCycleColor, ledRedDirection, breatheBorderRed, i, changeRate, upperAdjust, stayBelow, stayAbove);
        setColorScaled(ledCycleColor, ledRed, redLevel, i);
        setColorScaled(ledCycleColor, ledGreen, greenLevel, i);
        setColorScaled(ledCycleColor, ledBlue, blueLevel, i);
    }
}

//! Set the scaled color
void furSwarmPatterns::setColorScaled(uint8_t ledSourceColor[], uint8_t ledTargetColor[], uint8_t scaledLevel, int index) {
    float colorLevel = (float) ledSourceColor[index];
    colorLevel = colorLevel * (float) scaledLevel / 255.0;
    ledTargetColor[index] = (uint8_t)colorLevel;
}

//! Perform cycle action on `ledSourceColor'
void furSwarmPatterns::cycleDataColor(uint8_t ledSourceColor[], uint8_t ledDirection[], uint8_t ledBorder[],
                                      int ledIndex, uint8_t step, bool upperAdjust, uint8_t stayBelow, uint8_t stayAbove) {
    uint8_t stepForPeriod;
    if (ledSourceColor[ledIndex] > IN_OUT_BREAK_POINT && upperAdjust) {
        stepForPeriod = step * 4;
    } else {
        stepForPeriod = step;
    }
    if (ledDirection[ledIndex] == 1) {
        if (ledSourceColor[ledIndex] + stepForPeriod >= 255) {
            ledSourceColor[ledIndex] = 255;
            if (ledBorder[ledIndex] < stayAbove) {
                ledBorder[ledIndex] = ledBorder[ledIndex] + 1;
            } else {
                ledDirection[ledIndex] = 0;
            }
        } else {
            if (ledBorder[ledIndex] > 0) {
                ledBorder[ledIndex] = ledBorder[ledIndex] - 1;
            } else {
                ledBorder[ledIndex] = 0;
                ledSourceColor[ledIndex] = ledSourceColor[ledIndex] + stepForPeriod;
            }
        }
    } else if (ledDirection[ledIndex] == 0) {
        if (ledSourceColor[ledIndex] - stepForPeriod < 0) {
            ledSourceColor[ledIndex] = 0;
            if (ledBorder[ledIndex] < stayBelow) {
                ledBorder[ledIndex] = ledBorder[ledIndex] + 1;
            } else {
                ledDirection[ledIndex] = 1;
            }
        } else {
            if (ledBorder[ledIndex] > 0) {
                ledBorder[ledIndex] = ledBorder[ledIndex] - 1;
            } else {
                ledBorder[ledIndex] = 0;
                ledSourceColor[ledIndex] = ledSourceColor[ledIndex] - stepForPeriod;
            }
        }
    } else {
        ledSourceColor[ledIndex] = 0;
    }
}

//! Descend LED pump data
void furSwarmPatterns::descendPumpData(int step) {
    for (int i = 0; i < LED_COUNT; i++) {
        if ((ledRed[i] - step) > 0) {
            ledRed[i] -= step;
        } else {
            ledRed[i] = 0;
        }
        if ((ledGreen[i] - step) > 0) {
            ledGreen[i] -= step;
        } else {
            ledGreen[i] = 0;
        }
        if ((ledBlue[i] - step) > 0) {
            ledBlue[i] -= step;
        } else {
            ledBlue[i] = 0;
        }
    }
}

//! Descend LED pump data
void furSwarmPatterns::descendPumpDataScaled(int step) {
    float colorLevel, scaledColorLevel;
    for (int i = 0; i < LED_COUNT; i++) {
        if ((ledCycleColor[i] - step) > 0) {
            ledCycleColor[i] = ledCycleColor[i] - step;
        } else {
            ledCycleColor[i] = 0;
        }
        colorLevel = (float) ledCycleColor[i];
        scaledColorLevel = colorLevel * (float) redLevel / 255.0;
        ledRed[i] = (uint8_t)scaledColorLevel;
        scaledColorLevel = colorLevel * (float) greenLevel / 255.0;
        ledGreen[i] = (uint8_t)scaledColorLevel;
        scaledColorLevel = colorLevel * (float) blueLevel / 255.0;
        ledBlue[i] = (uint8_t)scaledColorLevel;
    }
}

//! Display the data currently stored in `led*'
void furSwarmPatterns::displayData(bool red, bool green, bool blue) {
  leds.sendStartFrame();
  for (int i = 0; i < LED_COUNT; i++) {
    if (red && green && blue) {
      leds.sendColor(i, ledRed[i], ledGreen[i], ledBlue[i]);
    } else {
      leds.sendColor(i, red?ledRed[i]:0, green?ledGreen[i]:0, blue?ledBlue[i]:0);
    }
  }
  leds.sendEndFrame();
}

//! Display the data currently stored in `led*'
void furSwarmPatterns::displayDataWithOverlay(bool red, bool green, bool blue,
                                              uint8_t redOverlay, uint8_t greenOverlay, uint8_t blueOverlay) {
    leds.sendStartFrame();
    for (int i = 0; i < LED_COUNT; i++) {
        if (red && green && blue) {
            leds.sendColor(i, ledRed[i], ledGreen[i], ledBlue[i]);
        } else {
            leds.sendColor(i, red?ledRed[i]:redOverlay, green?ledGreen[i]:greenOverlay, blue?ledBlue[i]:blueOverlay);
        }
    }
    leds.sendEndFrame();
}

//! Display the data currently stored in `led*' for a pump display
void furSwarmPatterns::displayPumpData(bool red, bool green, bool blue) {
    leds.sendStartFrame();
    for (int i = 0; i < LED_COUNT; i++) {
        leds.sendColor(i,
                  red?(ledRed[i] > redLevel)?redLevel:ledRed[i]:0,
                  green?(ledGreen[i] > greenLevel)?greenLevel:ledGreen[i]:0,
                  blue?(ledBlue[i] > blueLevel)?blueLevel:ledBlue[i]:0);
    }
    leds.sendEndFrame();
}

//! Initialize the LED strand with random data
void furSwarmPatterns::initializeRandom(uint8_t low, uint8_t high, uint8_t red, uint8_t green, uint8_t blue, bool single) {
    
    redLevel = red;
    greenLevel = green;
    blueLevel = blue;
    
    uint32_t randomValue;
    uint8_t randomByte;
    
    randomSeed(analogRead(randomSeedPin));
    for (int i = 0; i < LED_COUNT; i++) {
        randomValue = random(low, high);
        randomByte = (uint8_t) randomValue;
        ledRed[i] = randomByte;
        ledCycleColor[i] = randomByte;
        
        if (single) {
            ledGreen[i] = randomByte;
            ledBlue[i] = randomByte;
        } else {
            randomValue = random(low, high);
            randomByte = (uint8_t) randomValue;
            ledGreen[i] = randomByte;
            
            randomValue = random(low, high);
            randomByte = (uint8_t) randomValue;
            ledBlue[i] = randomByte;
        }
    }
}

//! Initialize the heartbeat pattern
void furSwarmPatterns::initializeHeartbeatPattern(uint8_t red, uint8_t green, uint8_t blue) {
    float intensityRatio = (float) heartbeatIntensity / 255.0;
    uint8_t lowerLevel = (intensityRatio * PUMP_UPPER_LEVEL);
    uint8_t upperLevel = (intensityRatio * 255);
    initializeRandom(lowerLevel, upperLevel, red, green, blue, false);
}

//! Initialize the Cylon pattern
void furSwarmPatterns::initializeCylon(uint8_t red, uint8_t green, uint8_t blue, bool twoSided) {
    int redHolder = red;
    int greenHolder = green;
    int blueHolder = blue;
#ifdef FS_TOWER_EYE
    int colorDescent = 2;
#else
    int colorDescent = 8;
#endif
    redLevel = red;
    greenLevel = green;
    blueLevel = blue;
    for (int i = 0; i < LED_COUNT; i++) {
        if (redHolder - colorDescent > 0) {
            redHolder = redHolder - colorDescent;
        } else {
            redHolder = 0;
        }
        if (greenHolder - colorDescent > 0) {
            greenHolder = greenHolder - colorDescent;
        } else {
            greenHolder = 0;
        }
        if (blueHolder - colorDescent > 0) {
            blueHolder = blueHolder - colorDescent;
        } else {
            blueHolder = 0;
        }
        ledRed[i] = redHolder;
        ledGreen[i] = greenHolder;
        ledBlue[i] = blueHolder;
    }
}

//! LED position index for cylon index `i' with frame `frameIndex'
int furSwarmPatterns::cylonIndexMap (int i, int frameIndex) {
    int indexMap;
#if defined (FS_VEST) || defined (FS_TOWER_VEST)
    indexMap = pgm_read_byte(&cyLEDMap[i]);
    if (!patternForward) {
        indexMap = indexMap + frameIndex;
        if (indexMap >= LED_COUNT) {
            indexMap = indexMap - LED_COUNT;
        }
    } else {
        indexMap = indexMap - frameIndex;
        if (indexMap < 0) {
            indexMap = indexMap + LED_COUNT;
        }
        indexMap = LED_COUNT - indexMap - 1;
    }
#else
    if (!patternForward) {
        indexMap = LED_CYMAP - i;
    } else {
        indexMap = i;
    }
    indexMap = indexMap + frameIndex;
    if (indexMap >= LED_COUNT) {
        indexMap = indexMap - LED_COUNT;
    }
#endif
    //printf ("i: %i, frameIndex: %i, indexMap: %i\n", i, frameIndex, indexMap);
    return indexMap;
}

//! Iterate and display the Cylon pattern
void furSwarmPatterns::displayCylon() {
    frameRelease--;
    if (frameRelease == 0) {
        frameRelease = patternSpeed;
        frameIndex++;
        if (frameIndex >= LED_COUNT) {
            frameIndex = 0;
        }
        if (patternSpeed < 9) {
            frameIndex++;
            if (frameIndex >= LED_COUNT) {
                frameIndex = 0;
            }
        }
        if (patternSpeed < 6) {
            frameIndex++;
            if (frameIndex >= LED_COUNT) {
                frameIndex = 0;
            }
        }
        if (patternSpeed < 3) {
            frameIndex++;
            if (frameIndex >= LED_COUNT) {
                frameIndex = 0;
            }
        }
    }
    float iterationProportion = (float) frameRelease / (float) patternSpeed;
    leds.sendStartFrame();
    int indexMap;
    uint8_t redLevel, greenLevel, blueLevel;
    for (int i = 0; i < LED_CYMAP; i++) {
        indexMap = cylonIndexMap (i, frameIndex);
        redLevel = (uint8_t) ((float)ledRed[indexMap] * iterationProportion);
        greenLevel = (uint8_t) ((float)ledGreen[indexMap] * iterationProportion);
        blueLevel = (uint8_t) ((float)ledBlue[indexMap] * iterationProportion);
        if (frameIndex + 1 >= LED_COUNT) {
            indexMap = cylonIndexMap (i, 0);
        } else {
            indexMap = cylonIndexMap (i, frameIndex + 1);
        }
        redLevel = redLevel + (uint8_t) ((float)ledRed[indexMap] * (1.0 - iterationProportion));
        greenLevel = greenLevel + (uint8_t) ((float)ledGreen[indexMap] * (1.0 - iterationProportion));
        blueLevel = blueLevel + (uint8_t) ((float)ledBlue[indexMap] * (1.0 - iterationProportion));
        leds.sendColor(i, redLevel, greenLevel, blueLevel);
    }
    leds.sendEndFrame();
}

//! Fill matrix data for `frameIndex'
void furSwarmPatterns::fillMatrix(int ledPosition, int framePosition, float proportion, bool accum) {
    int indexMap = 0;
    int iterationI = 0;
    
    iterationI = ledPosition + 9 * framePosition;
    if (iterationI > LED_BYTEMAP) {
        iterationI -= LED_BYTEMAP;
    }
    indexMap = pgm_read_byte(&rcLEDMap[iterationI]);
    if (indexMap != 0xFF) {
        uint8_t baseLevel;
        baseLevel = pgm_read_byte(&matrixBitmap[ledPosition]);
        float scaledColorLevel = proportion * (float) intensityLevel * (float) baseLevel / 255.0 / 255.0;
        float scaledRedLevel, scaledGreenLevel, scaledBlueLevel;
        
        scaledRedLevel = (float) redLevel * scaledColorLevel;
        if (accum) {
            if (scaledRedLevel + ledRed[indexMap] > 255.0) {
                ledRed[indexMap] = 255;
            } else {
                ledRed[indexMap] = scaledRedLevel + ledRed[indexMap];
            }
        } else {
            ledRed[indexMap] = scaledRedLevel;
        }
        scaledGreenLevel = (float) greenLevel * scaledColorLevel;
        if (accum) {
            if (scaledGreenLevel + ledGreen[indexMap] > 255) {
                ledGreen[indexMap] = 255;
            } else {
                ledGreen[indexMap] = scaledGreenLevel + ledGreen[indexMap];
            }
        } else {
            ledGreen[indexMap] = scaledGreenLevel;
        }
        scaledBlueLevel = (float) blueLevel * scaledColorLevel;
        if (accum) {
            if (scaledBlueLevel + ledBlue[indexMap] > 255) {
                ledBlue[indexMap] = 255;
            } else {
                ledBlue[indexMap] = scaledBlueLevel + ledBlue[indexMap];
            }
        } else {
            ledBlue[indexMap] = scaledBlueLevel;
        }
    }
}

//! Initialize The Matrix pattern from PROGMEM
void furSwarmPatterns::initializeMatrix(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue, int upperFrameCount) {
    int nextIterationPosition = 0;
    float iterationProportion = (float) frameRelease / (float) patternSpeed;
    
    intensityLevel = intensity;
    redLevel = red;
    greenLevel = green;
    blueLevel = blue;
    
    if (frameIndex > upperFrameCount || frameIndex < 0) {
        frameIndex = 0;
    }
    nextIterationPosition = frameIndex + 1;
    if (nextIterationPosition > upperFrameCount || nextIterationPosition < 0) {
        nextIterationPosition = 0;
    }
    for (int i = 0; i < LED_BYTEMAP; i++) {
        fillMatrix(i, frameIndex, iterationProportion, false);
    }
    for (int i = 0; i < LED_BYTEMAP; i++) {
        fillMatrix(i, nextIterationPosition, 1.0 - iterationProportion, true);
    }
}

//! Iterate the frame number
void furSwarmPatterns::iterateFrameStep(uint8_t upperFrameNumber) {
    frameRelease--;
    if (frameRelease <= 0) {
        frameRelease = patternSpeed;
        frameIndex++;
        if (frameIndex > upperFrameNumber - 1) {
            frameIndex = 0;
        }
    }
}

//! Iterate and display the Matrix pattern
void furSwarmPatterns::displayMatrix() {
    iterateFrameStep(matrixFrameCount);
    initializeMatrix(intensityLevel, redLevel, greenLevel, blueLevel, matrixFrameCount);
    displayData(true, true, true);
}

//! Which column does `ledIndex' correspond to?
uint8_t imageColumn(int ledIndex) {
    int column = ledIndex % (LED_PANEL_COLUMNS * 2 + LED_BACK_COLUMNS);
    if (column >= LED_PANEL_COLUMNS && column < (LED_PANEL_COLUMNS + LED_BACK_COLUMNS)) {
        return column - LED_PANEL_COLUMNS;
    } else {
        return 255;
    }
}

//! Initialize the Drop pattern from PROGMEM
void furSwarmPatterns::initializeDrop() {
    int indexMap = 0;
    for (int i = 0; i < LED_BYTEMAP; i++) {
        indexMap = pgm_read_byte(&rcLEDMap[i]);
        if (indexMap != 0xFF) {
            ledRed[indexMap] = pgm_read_byte(&dropRed[i]);
            ledGreen[indexMap] = pgm_read_byte(&dropGreen[i]);
            ledBlue[indexMap] = pgm_read_byte(&dropBlue[i]);
        }
    }
}

//! Iterate and display the Drop pattern
void furSwarmPatterns::displayDrop() {
    leds.sendStartFrame();
    for (int i = 0; i < LED_COUNT; i++) {
        cycleDataColor(ledRed, ledRedDirection, breatheBorderRed, i, patternSpeed, false, 0, 0);
        leds.sendColor(i, ledRed[i], ledGreen[i], ledBlue[i]);
    }
    leds.sendEndFrame();
}

//! Initialize the character pattern from PROGMEM
void furSwarmPatterns::initializeCharacter(uint8_t intensity, uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t indexMap = 0;
    int byteNumber = 0;
    int bitNumber = 0;
    uint8_t patternByte = 0;
    intensityLevel = intensity;
    redLevel = red;
    greenLevel = green;
    blueLevel = blue;
    for (int i = 0; i < LED_BMMAP; i++) {
        indexMap = bmLEDMap[i];
        if (indexMap != 0xFF) {
            byteNumber = i / 8;
            bitNumber = i % 8;
            patternByte = characterTable[characterIndex - 1][byteNumber];
            if (patternByte & (1 << bitNumber)) {
                ledRed[indexMap] = (uint8_t) ((float) intensity * (float) red / 255.0);
                ledGreen[indexMap] = (uint8_t) ((float) intensity * (float) green / 255.0);
                ledBlue[indexMap] = (uint8_t) ((float) intensity * (float) blue / 255.0);
            } else {
                ledRed[indexMap] = 0x00;
                ledGreen[indexMap] = 0x00;
                ledBlue[indexMap] = 0x00;
            }
        }
    }
}

//! Send a blank screen
void furSwarmPatterns::displayBlank() {
    displayData(false,false,false);
}

//! Initialize bitmap based patterns
void furSwarmPatterns::initializeBitmapPattern(uint8_t red, uint8_t green, uint8_t blue, int index, bool accum, uint8_t patternId) {
    uint8_t indexMap = 0;
    int byteNumber = 0;
    int bitNumber = 0;
    uint8_t patternByte = 0;
    float iterationProportion = (float) frameRelease / (float) patternSpeed;
    
    redLevel = red;
    greenLevel = green;
    blueLevel = blue;
    for (int i = 0; i < LED_BMMAP; i++) {
        indexMap = pgm_read_byte(&bmLEDMap[i]);
        if (indexMap != 0xFF) {
            byteNumber = i / 8;
            bitNumber = i % 8;
            patternByte = 0;
            if (patternId == FS_ID_CYLON_PONG) {
                patternByte = cylonPongTable[index][byteNumber];
            } else if (patternId == FS_ID_SPIRAL) {
                patternByte = spiralTable[index][byteNumber];
            } else if (patternId == FS_ID_CYLON_VERTICAL) {
                patternByte = cylonVerticalTable[index][byteNumber];
            }
            if (patternByte & (1 << bitNumber)) {
                if (accum) {
                    ledRed[indexMap] = (float) redLevel * (1.0 - iterationProportion) + ledRed[indexMap];
                    ledGreen[indexMap] = (float) greenLevel * (1.0 - iterationProportion) + ledGreen[indexMap];
                    ledBlue[indexMap] = (float) blueLevel * (1.0 - iterationProportion) + ledBlue[indexMap];
                } else {
                    ledRed[indexMap] = (float) redLevel * iterationProportion;
                    ledGreen[indexMap] = (float) greenLevel * iterationProportion;
                    ledBlue[indexMap] = (float) blueLevel * iterationProportion;
                }
            } else if (!accum) {
                ledRed[indexMap] = 0x00;
                ledGreen[indexMap] = 0x00;
                ledBlue[indexMap] = 0x00;
            }
        }
    }
}

//! Display the current bitmap pattern
void furSwarmPatterns::displayBitmapPattern(uint8_t patternID) {
    int frameCount = 8;
    switch (patternID) {
        case FS_ID_CYLON_PONG:
            frameCount = cylonPongFrameCount;
            break;
        case FS_ID_SPIRAL:
            frameCount = sprialFrameCount;
            break;
        case FS_ID_CYLON_VERTICAL:
            frameCount = cylonVerticalFrameCount;
            break;
        default:
            break;
    }
    iterateFrameStep(frameCount);
    initializeBitmapPattern(redLevel, greenLevel, blueLevel, frameIndex, false, patternID);
    int nextIndex = frameIndex + 1;
    if (nextIndex > frameCount - 1) {
        nextIndex = 0;
    }
    initializeBitmapPattern(redLevel, greenLevel, blueLevel, nextIndex, true, patternID);
    displayData(true, true, true);
}

//! Initialize the flash pattern
void furSwarmPatterns::initializeFlash(uint8_t red, uint8_t green, uint8_t blue) {
    if (millis() - flashStartTime < 500) {
        flashLedChangeRate = ledChangeRate * 2;
    } else {
        flashLedChangeRate = ledChangeRate;
    }
    initializeRandom(PUMP_UPPER_LEVEL, 255, red, green, blue, false);
    flashStartTime = millis();
}

//! Set up HSV based strand
void furSwarmPatterns::initializeHSVstrand() {
    hsv toConvert;
    rgb converted;
    toConvert.h = 0.0;
    toConvert.s = 1.0;
    toConvert.v = prismValue;
    for (int i = 0; i < LED_COUNT; i++) {
        toConvert.h += 360.0 / (float) (LED_COUNT - 1);
        converted = hsv2rgb(toConvert);
    
        ledRed[i] = (uint8_t) (converted.r * 255.0);
        ledGreen[i] = (uint8_t) (converted.g * 255.0);
        ledBlue[i] = (uint8_t) (converted.b * 255.0);
    }
}

//! Incremental degree location on the color wheel based on `input' and `patternSpeed'
float furSwarmPatterns::incrementalDegree (float input) {
    float output = input;
    int directionFactor;
    if (patternForward) {
        directionFactor = 1.0;
    } else {
        directionFactor = -1.0;
    }
    output += 2.0 * directionFactor;
    if (patternSpeed < 17) {
        output += 2.0 * directionFactor;
    }
    if (patternSpeed < 10) {
        output += 2.0 * directionFactor;
    }
    if (patternSpeed < 5) {
        output += directionFactor;
    } else if (output > 360.0) {
        output -= 360.0;
    } else if (output < 0.0) {
        output += 360.0;
    }
    return output;
}

//! Advance the strand by HSV
void furSwarmPatterns::iterateStrandByHSV() {
    rgb toConvert;
    hsv converted;
    frameRelease--;
    if (frameRelease == 0) {
        frameRelease = patternSpeed;
        for (int i = 0; i < LED_COUNT; i++) {
            toConvert.r = (float) ledRed[i] / 255;
            toConvert.g = (float) ledGreen[i] / 255;
            toConvert.b = (float) ledBlue[i] / 255;
            
            converted = rgb2hsv(toConvert);
            converted.h = incrementalDegree(converted.h);
            converted.s = 1.0;
            converted.v = prismValue;
            toConvert = hsv2rgb(converted);
            
            ledRed[i] = toConvert.r * 255;
            ledGreen[i] = toConvert.g * 255;
            ledBlue[i] = toConvert.b * 255;
        }
    }
    
    leds.sendStartFrame();
    int indexMap = 0;
    float iterationProportion = (float) frameRelease / (float) patternSpeed;
    uint8_t redLevel, greenLevel, blueLevel;
    for (int i = 0; i < LED_COUNT; i++) {
#if defined (FS_VEST) || defined (FS_TOWER_VEST) 
        indexMap = pgm_read_byte(&cyLEDMap[i]);
#else
        indexMap = LED_COUNT - i;
#endif
        indexMap = indexMap + frameIndex;
        if (indexMap >= LED_COUNT) {
            indexMap = indexMap - LED_COUNT;
        }
        toConvert.r = (float) ledRed[indexMap] / 255;
        toConvert.g = (float) ledGreen[indexMap] / 255;
        toConvert.b = (float) ledBlue[indexMap] / 255;
        
        converted = rgb2hsv(toConvert);
        converted.h = incrementalDegree(converted.h);
        converted.s = 1.0;
        converted.v = prismValue;
        toConvert = hsv2rgb(converted);
        
        redLevel = (uint8_t) ((float)ledRed[indexMap] * iterationProportion) +
        (uint8_t) (toConvert.r * 255.0 * (1.0 - iterationProportion));
        greenLevel = (uint8_t) ((float)ledGreen[indexMap] * iterationProportion) +
        (uint8_t) (toConvert.g * 255.0 * (1.0 - iterationProportion));
        blueLevel = (uint8_t) ((float)ledBlue[indexMap] * iterationProportion) +
        (uint8_t) (toConvert.b * 255.0 * (1.0 - iterationProportion));
        leds.sendColor(i, redLevel, greenLevel, blueLevel);
    }
    leds.sendEndFrame();
}

//! Select the prism color accordingly
void furSwarmPatterns::iteratePrismColor(bool force) {
    frameRelease--;
    if (frameRelease == 0 || force) {
        frameRelease = patternSpeed;
        prismHue = incrementalDegree(prismHue);
    }
    hsv toConvert;
    rgb converted;
    float iterationProportion = (float) frameRelease / (float) patternSpeed;
    
    toConvert.h = prismHue;
    toConvert.s = 1.0;
    toConvert.v = prismValue;
    converted = hsv2rgb(toConvert);
    
    redLevel = (uint8_t) (converted.r * 255.0 * iterationProportion);
    greenLevel = (uint8_t) (converted.g * 255.0 * iterationProportion);
    blueLevel = (uint8_t) (converted.b * 255.0 * iterationProportion);
    
    toConvert.h = incrementalDegree(prismHue);
    toConvert.s = 1.0;
    toConvert.v = prismValue;
    converted = hsv2rgb(toConvert);
    
    redLevel = redLevel + (uint8_t) (converted.r * 255.0 * (1.0 - iterationProportion));
    greenLevel = greenLevel + (uint8_t) (converted.g * 255.0 * (1.0 - iterationProportion));
    blueLevel = blueLevel + (uint8_t) (converted.b * 255.0 * (1.0 - iterationProportion));
    
    setfullStrand (255, redLevel, greenLevel, blueLevel, false);
}

//! Display the sound display data
void furSwarmPatterns::displaySoundActivate() {
    
    float intensityLevel = 0.0;
    intensityLevel = ((float) saRunningAverage / (float) saThreshold) * 255.0;
    if (intensityLevel > 255.0) intensityLevel = 255.0;
    else if (intensityLevel < 0.0) intensityLevel = 0.0;
    
#if (defined (FS_TOWER) || defined (FS_TOWER_HAT) || defined (FS_TOWN_CENTER) || defined(FS_DRESS)) && !defined(FS_TOWER_EYE)
    // Slow down the intensity level retraction and display a peak for a short period
    uint32_t timeStamp = millis();
    if (intensityLevel > peakSoundLevel) {
        peakRetractionTime = timeStamp;
        peakSoundLevel = intensityLevel;
    } else if (timeStamp - peakRetractionTime > 2000) {
        peakRetractionTime = timeStamp;
        peakSoundLevel = intensityLevel;
    } else if (timeStamp - peakRetractionTime > 500) {
        if (peakSoundLevel > 10) {
            peakSoundLevel -= 10;
        } else {
            peakSoundLevel = 0;
        }
    }
    leds.sendStartFrame();
    /* Use the entire strand as a vue meter */
    uint8_t peakLed = (uint8_t)(peakSoundLevel / 255.0 * LED_COUNT);
    for (int i = 0; i < LED_COUNT; i++) {
        float ledPosition = (float) i / (float) LED_COUNT * 255.0;
        if (i == peakLed - 1 || ledPosition < intensityLevel) {
            float ledFactor;
            if (i == peakLed - 2) {
                ledFactor = .5;
            } else {
                ledFactor = 1.0;
            }
            if (ledPosition > 200.0) {
                leds.sendColor(i, (int) (200 * ledFactor), 0, 0);
            }
            else if (ledPosition > 150.0) {
                leds.sendColor(i, (int) (200 * ledFactor), (int) (200 * ledFactor), 0);
            }
            else {
                leds.sendColor(i, 0, (int) (200 * ledFactor), 0);
            }
        }
        else {
            leds.sendColor(i, 0, 0, 0);
        }
    }
    leds.sendEndFrame();
#else
    int indexMap = 0;
    uint8_t row, column;
    bool displayIfNecessary;
    int levelRow;
    uint32_t randomValue;
    int randomByteHigh, randomByteLow;
#ifdef FS_TOWER_EYE
    if (intensityLevel > 245.0) {
        levelRow = 0;
    } else if (intensityLevel > 235.0) {
        levelRow = 1;
    } else if (intensityLevel > 225.0) {
        levelRow = 2;
    } else if (intensityLevel > 215.0) {
        levelRow = 3;
    } else if (intensityLevel > 205.0) {
        levelRow = 4;
    } else if (intensityLevel > 195.0) {
        levelRow = 5;
    } else if (intensityLevel > 185.0) {
        levelRow = 6;
    } else if (intensityLevel > 175.0) {
        levelRow = 7;
    } else if (intensityLevel > 165.0) {
        levelRow = 8;
    } else if (intensityLevel > 155.0) {
        levelRow = 9;
    } else if (intensityLevel > 145.0) {
        levelRow = 10;
    } else if (intensityLevel > 135.0) {
        levelRow = 11;
    } else if (intensityLevel > 120.0) {
        levelRow = 12;
    } else {
        levelRow = 255;
    }
#else
    if (intensityLevel > 245.0) {
        levelRow = 0;
    } else if (intensityLevel > 220.0) {
        levelRow = 1;
    } else if (intensityLevel > 195.0) {
        levelRow = 2;
    } else if (intensityLevel > 170.0) {
        levelRow = 3;
    } else if (intensityLevel > 145.0) {
        levelRow = 4;
    } else if (intensityLevel > 120.0) {
        levelRow = 5;
    } else {
        levelRow = 255;
    }
#endif
    
    if (intensityLevel > 250.0) {
        // Railing up
        randomByteHigh = 255;
        randomByteLow = 255;
    } else if (intensityLevel < 185.0 || intensityLevel > 245.0) {
        // Reset frequency trick
        randomValue = random(0, 255);
        redLevel = (uint8_t) randomValue;
        randomValue = random(0, 255);
        greenLevel = (uint8_t) randomValue;
        randomByteHigh = redLevel;
        randomByteLow = greenLevel;
    } else {
        // Reuse previously set frequency trick
        randomByteHigh = redLevel;
        randomByteLow = greenLevel;
    }
    
    for (int i = 0; i < LED_BYTEMAP; i++) {
        indexMap = pgm_read_byte(&rcLEDMap[i]);
        row = i / (LED_PANEL_COLUMNS * 2 + LED_BACK_COLUMNS);
        column = imageColumn(i);
        displayIfNecessary = row == levelRow && (randomByteHigh & (1 << column)); // Upper row
        displayIfNecessary = displayIfNecessary || ((row == levelRow + 1) && ((randomByteLow & (1 << column)) || (randomByteHigh & (1 << column)))); // Second rows
        displayIfNecessary = displayIfNecessary || row > levelRow + 1; // Lower rows
        
        if (indexMap != 0xff && displayIfNecessary) {
            switch (row) {
#ifdef FS_TOWER_EYE
                case 0:
                case 1:
#else
                case 0:
#endif
                    ledRed[indexMap] = 200;
                    ledGreen[indexMap] = 0;
                    ledBlue[indexMap] = 0;
                    break;
#ifdef FS_TOWER_EYE
                case 2:
                case 3:
                case 4:
                case 5:
#else
                case 1:
                case 2:
#endif
                    ledRed[indexMap] = 200;
                    ledGreen[indexMap] = 200;
                    ledBlue[indexMap] = 0;
                    break;
#ifdef FS_TOWER_EYE
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
#else
                case 3:
                case 4:
                case 5:
#endif
                    ledRed[indexMap] = 0;
                    ledGreen[indexMap] = 200;
                    ledBlue[indexMap] = 0;
                    break;
                default:
                    ledRed[indexMap] = 0;
                    ledGreen[indexMap] = 0;
                    ledBlue[indexMap] = 0;
                    break;
            }
        } else {
            ledRed[indexMap] = 0;
            ledGreen[indexMap] = 0;
            ledBlue[indexMap] = 0;
        }
    }
    displayData(true,true,true);
#endif
}

//! Update sound parameters
// thresholdData is controlled the pattern speed slider
// sampleData and averageData is controlled by intensity slider
void furSwarmPatterns::updateSoundActivateParameters(uint8_t thresholdData, uint8_t sampleData, uint8_t averageData) {
    float newThreshold;
    float newSampleNumber;
    float newAveragedOver;
    newThreshold = (float) thresholdData;
    newThreshold = 1000.0 + newThreshold * 5000.0 / 255.0; // Threshold can range from 10k to 20k
    newSampleNumber = (float) sampleData;
    newSampleNumber = 40;// + newSampleNumber * 32.0 / 255.0; // Number of samples can range from 78 to 110
    newAveragedOver = (float) averageData;
    newAveragedOver = 1;//12 - (float) newAveragedOver * 11.0 / 255.0; // Averaged over count can range from 12 to 1
    saThreshold = (long) newThreshold;
    saTargetThreshold = saThreshold * 0.90;
    saNumberOfSamples = (int) newSampleNumber;
    saAveragedOver = (int) newAveragedOver;
    saGain = 1.0 + (float) averageData * 20.0 / 255.0;
}

//! Sum of squared audio signal
float furSwarmPatterns::sumOfSquareAudio() {
    float saSumOfSquare = 0;
    int saSample;
    float saSignal;
    for (int i = 0; i < saNumberOfSamples; i++) {
        saSample = analogRead(audioAnalogPin);
        saSignal = abs(saSample - SA_MIDDLE_VALUE);
        //if (saSignal < 0) {
        //    saSignal = 0;
        //}
        saSignal *= saSignal;
        saSumOfSquare += saSignal * saGain;
    }
    return saSumOfSquare;
}

//! Display the sound activate pattern
void furSwarmPatterns::iterateSoundActivate() {
    float saSumOfSquare = sumOfSquareAudio();
    float saAverageReading;
    
    saAverageReading = saSumOfSquare / saNumberOfSamples;
    saRunningAverageHistory[saRunningAveragePosition] = saAverageReading;
    saRunningAveragePosition++;
    if (saRunningAveragePosition >= SA_RUNNING_AVG_HISTORY) saRunningAveragePosition = 0;
    saRunningAverage = 0.0;
    for (int i = 0; i < SA_RUNNING_AVG_HISTORY; i++) {
      saRunningAverage += saRunningAverageHistory[i];
    }
    saRunningAverage /= (float) SA_RUNNING_AVG_HISTORY;
    //float saPrevRunningAverage = saRunningAverage;
    //saRunningAverage = (((saAveragedOver - 1) * saRunningAverage) + saAverageReading) / saAveragedOver;
    saMovingAverage = 0.0; //saMovingAverage + saRunningAverage / saMovingAverageCount - saMovingAverage / saMovingAverageCount;
    
    // PID Controller
    float saProportional = (saMovingAverage - saTargetThreshold) / 1000.0;
    //float saDamper = (saRunningAverage - saPrevRunningAverage) / 500.0;
    //float saPressure = 0;
    //saPressure = saProportional + saDamper;
    /*
     if (saProportional > 0.0 && saDamper > 0.0) {
     saPressure = saProportional + saDamper;
     } else if (saProportional > 0.0 && saDamper < 0.0) {
     saPressure = saProportional - saDamper;
     } else if (saProportional < 0.0 && saDamper > 0.0) {
     saPressure = saProportional + saDamper;
     } else if (saProportional < 0.0 && saDamper < 0.0) {
     saPressure = saProportional - saDamper;
     }
    if (saPressure < 0.0) {
        saPressure *= -1.0;
    }
    if (saPressure > SA_PRESSURE_MAX) {
        saPressure = SA_PRESSURE_MAX;
    }
    if (saMovingAverage > saTargetThreshold) {
        saGain -= 0.1 * saPressure;
    } else if (saMovingAverage < saTargetThreshold) {
        saGain += 0.1 * saPressure;
    }
    if (saGain > SA_GAIN_UPPER) {
        saGain = SA_GAIN_UPPER;
    } else if (saGain < SA_GAIN_LOWER) {
        saGain = SA_GAIN_LOWER;
    }
    saGain = SA_GAIN_UPPER;
    */

#ifdef FFT_DIAGNOSTICS
    Serial.print ("RUN:");
    Serial.print (saTargetThreshold);
    Serial.print (",");
    Serial.print (saMovingAverage);
    Serial.print (",");
    Serial.print (saGain);
    Serial.print (",");
    Serial.print (saRunningAverage);
    Serial.print (",");
    Serial.print ("0");
    Serial.println ("|");
    Serial.print ("INP:");
    for (int i = 0; i < 256 / 2; i++) {
        Serial.print ("0.0");
        if (i < 256 / 2 - 1) {
            Serial.print (",");
        }
    }
    Serial.println ("|");
    Serial.print ("MAG:");
    for (int i = 0; i < 256 / 2; i++) {
        Serial.print ("0.0");
        if (i < 256 / 2 - 1) {
            Serial.print (",");
        }
    }
    Serial.println ("|");
    Serial.print ("TIM:");
    Serial.print ("0.0");
    Serial.println ("|");
#endif
}

//! Initialize the selected pattern
void furSwarmPatterns::initializePattern(uint8_t *data, uint8_t dataLength) {
    int messageType;
    int lastPatternSpeed = patternSpeed;
    // data [0] - pattern
    // data [1] - speed
    // data [2] - red
    // data [3] - green
    // data [4] - blue
    // data [5] - aux (usually intensity)
    // data [6] - delay in ms / FS_DELAY_FACTOR;
    messageType = (int) (0x7F & data[0]);
    //messageType = (int) data[0];
    transitionRequested = (0x80 & data[0]) > 0;
    if (transitionRequested) cycleSpot = 0;
    patternSpeed = (int) data[1];
    patternSpeedLevel = patternSpeed;
    
    switch (messageType) {
        case FS_ID_HEARTBEAT_REQUEST:
            break;
        case FS_ID_FULL_COLOR:
            if (pattern == messageType) transitionRequested = false;
            pattern = messageType;
            intensityLevel = data[5];
            redLevel = data[2];
            greenLevel = data[3];
            blueLevel = data[4];
            frameRelease = patternSpeed * SMOOTH_FACTOR_INT;
            setfullStrand(intensityLevel, redLevel, greenLevel, blueLevel, true);
            break;
        case FS_ID_SPARKLE:
            setPatternSpeedWithFactor(20);
            if (pattern != messageType) {
                initializeRandom(0, 255, 0xff, 0xff, 0xff, false);
                for (int i = 0; i < LED_COUNT; i++) {
                    if (ledRedDirection[i] > 1) {
                        ledRedDirection[i] = 1;
                    }
                    if (ledGreenDirection[i] > 1) {
                        ledGreenDirection[i] = 1;
                    }
                    if (ledBlueDirection[i] > 1) {
                        ledBlueDirection[i] = 1;
                    }
                }
            }
            pattern = messageType;
            break;
        case FS_ID_DESCEND:
            setPatternSpeedWithFactor(20);
            if (pattern != messageType) {
                initializeRandom(0, 255, 0xff, 0xff, 0xff, false);
            }
            pattern = messageType;
            break;
        case FS_ID_OFF:
            pattern = messageType;
            setfullStrand(0, 0, 0, 0, false);
            break;
        case FS_ID_FLASH:
            initializeFlash(data [2], data [3], data [4]);
            displayPumpData(true, true, true);
            pattern = messageType;
            break;
        case FS_ID_RANDOM_FLASH:
            initializeFlash(data [2], data [3], data [4]);
            heartbeatPumpShortStart = random (patternSpeed * 5);
            displayPumpData(true, true, true);
            pattern = messageType;
            break;
        case FS_ID_HEART:
            heartbeatPumpShortPeriod = (255 - data [1]) * 4;
            heartbeatIntensity = data [5];
            if (patternOrColorChanged(data)) {
                redLevel = data [2];
                greenLevel = data [3];
                blueLevel = data [4];
            }
            if (pattern != messageType) {
                initializeHeartbeatPattern(data [2], data [3], data [4]);
                heartbeatPumpShortStart = millis();
                heartbeatPumpLongStart = 0;
            }
            pattern = messageType;
            break;
        case FS_ID_BREATHE:
            setPatternSpeedWithFactor(20);
            redLevel = data [2];
            greenLevel = data [3];
            blueLevel = data [4];
            if (breatheUpperLevel != data[5]) {
                breatheUpperLevel = data[5];
                initializeRandom(breatheUpperLevel, 255, data [2], data [3], data [4], false);
                for (int i = 0; i < LED_COUNT; i++) {
                    breatheBorderRed[i] = 0;
                    breatheBorderGreen[i] = 0;
                    breatheBorderBlue[i] = 0;
                    ledRedDirection[i] = 1;
                    ledGreenDirection[i] = 1;
                    ledBlueDirection[i] = 1;
                }
            }
            pattern = messageType;
            break;
        case FS_ID_ORGANIC:
            setPatternSpeedWithFactor(10);
            if (pattern != messageType) {
                initializeRandom(ORGANIC_UPPER_LEVEL, 255, 0xff, 0xff, 0xff, true);
                for (int i = 0; i < LED_COUNT; i++) {
                    if (i % 2 == 0) {
                        ledRedDirection[i] = 1;
                        ledGreenDirection[i] = 1;
                        ledBlueDirection[i] = 1;
                    } else {
                        ledRedDirection[i] = 0;
                        ledGreenDirection[i] = 0;
                        ledBlueDirection[i] = 0;
                    }
                }
            }
            pattern = messageType;
            break;
        case FS_ID_CYLON:
            setTwoWaySweepSpeed (20, data[1]);
            if (patternOrColorChanged(data)) {
                if (pattern != messageType) {
                    frameIndex = 0;
                    frameRelease = patternSpeed;
                }
                initializeCylon(data [2], data [3], data [4], false);
            }
            pattern = messageType;
            break;
        case FS_ID_DROP:
            setPatternSpeedWithFactor(10);
            if (pattern != messageType) {
                initializeDrop();
                for (int i = 0; i < LED_COUNT; i++) {
                    ledRedDirection[i] = 1;
                    ledGreenDirection[i] = 1;
                    ledBlueDirection[i] = 1;
                }
            }
            pattern = messageType;
            break;
        case FS_ID_CHARACTER:
            if (data[5] == 255) {
                characterIndex = 0;
            } else if (data[5] != 0) {
                characterIndex = data[5];
            } else if (pattern == messageType && colorNoChange(data)) {
                characterIndex++;
            } else {
                characterIndex = 1;
            }
            if (characterIndex >= characterIndexUpper) {
                characterIndex = 1;
            }
            pattern = messageType;
            initializeCharacter(data [1], data [2], data [3], data [4]);
            break;
        case FS_ID_CYLON_VERTICAL:
            patternSpeed = min(255, 256 - (int) data [1]);
            setPatternSpeedWithFactor(15);
            if (frameIndex > cylonVerticalFrameCount || pattern != messageType) {
                frameIndex = 1;
                frameRelease = patternSpeed;
            }
            initializeBitmapPattern(data [2], data [3], data [4], frameIndex, false, FS_ID_CYLON_VERTICAL);
            pattern = messageType;
            break;
        case FS_ID_CYLON_PONG:
            patternSpeed = min(255, 256 - (int) data [1]);
            setPatternSpeedWithFactor(30);
            if (frameIndex > cylonPongFrameCount || pattern != messageType) {
                frameIndex = 1;
                frameRelease = patternSpeed;
            }
            initializeBitmapPattern(data [2], data [3], data [4], frameIndex, false, FS_ID_CYLON_PONG);
            pattern = messageType;
            break;
        case FS_ID_SOUND_ACTIVATE:
	    //initializeFlash(data [2], data [3], data [4]);
            updateSoundActivateParameters(data[1], data[5], data[5]);
            displayPumpData(true, true, true);
            pattern = messageType;
            break;
        case FS_ID_PRISM:
            setTwoWaySweepSpeed (2, data[1]);
            prismValue = (float) data [5] / 255.0;
            if (data [2] != 0 && patternSpeed == 0) {
                // Distance color command
                prismHue = (((float) data [2]) / 255.0) * 360.0;
                iteratePrismColor(true);
            }
            frameRelease = patternSpeed;
            pattern = messageType;
            break;
        case FS_ID_MATRIX:
            patternSpeed = min(255, 256 - (int) data [1]);
            setPatternSpeedWithFactor(10);
            if (patternOrColorChanged(data) || intensityLevel != data[5]) {
                if (pattern != messageType) {
                    frameIndex = 0;
                    frameRelease = patternSpeed;
                }
                initializeMatrix(data[5], data[2], data[3], data[4], 5);  // 5 is the upperFrameCount
            }
            pattern = messageType;
            break;
        case FS_ID_RAINBOW_CHASE:
            setTwoWaySweepSpeed (20, data[1]);
            prismValue = (float) data [5] / 255.0;
            if (prismValue < 0.0) {
                prismValue = 0.0;
            } else if (prismValue > 1.0) {
                prismValue = 1.0;
            }
            frameRelease = patternSpeed;
            if (pattern != messageType) {
                initializeHSVstrand();
            }
            pattern = messageType;
            break;
        case FS_ID_STARFIELD:
            setPatternSpeedWithFactor(10);
            if (pattern != messageType || (colorNoChange(data) && lastPatternSpeed == patternSpeed)) {
                if (data[2] == data[3] && data[3] == data[4]) {
                    initializeRandom(STARFIELD_UPPER_LEVEL, 255, 0xff, 0xff, 0xff, true);
                } else {
                    initializeRandom(STARFIELD_UPPER_LEVEL, 255, data[2], data[3], data[4], false);
                }
                long randomValue;
                randomSeed(analogRead(randomSeedPin));
                for (int i = 0; i < LED_COUNT; i++) {
                    breatheBorderRed[i] = 0;
                    breatheBorderGreen[i] = 0;
                    breatheBorderBlue[i] = 0;
                    randomValue = random(0, 255);
                    if (randomValue & 0b00000011) {
                        ledRedDirection[i] = 2;
                        ledGreenDirection[i] = 2;
                        ledBlueDirection[i] = 2;
                    } else {
                        ledRedDirection[i] = 0;
                        ledGreenDirection[i] = 0;
                        ledBlueDirection[i] = 0;
                    }
                }
            }
            pattern = messageType;
            break;
        case FS_ID_SPIRAL:
            patternSpeed = 255 - (int) data [1];
            setPatternSpeedWithFactor(20);
            if (frameIndex > 8 || pattern != messageType) {
                frameIndex = 1;
                frameRelease = patternSpeed;
            }
            initializeBitmapPattern(data [2], data [3], data [4], frameIndex, false, FS_ID_SPIRAL);
            pattern = messageType;
            break;
        default:
            break;
    }
}

//! Set pattern speed based on a positive / negative sweep
void furSwarmPatterns::setTwoWaySweepSpeed (int factor, uint8_t dataValue) {
    // Set the factor to be larger for a smaller sweep scale
    // Adjust steps in the increment accordingly
    if (dataValue < 128) {
        patternForward = true;
    } else {
        patternForward = false;
    }
    int scaledPatternSpeed = abs(((int) dataValue - 128) * 2);
    if (scaledPatternSpeed > 255) {
        patternSpeed = 0;
    } else if (scaledPatternSpeed < 0) {
        patternSpeed = 255;
    } else {
        patternSpeed = 255 - scaledPatternSpeed;
    }
    patternSpeed = min(max(1, patternSpeed), 255);
    setPatternSpeedWithFactor(factor);
}

//! Apply a factor to the pattern speed
void furSwarmPatterns::setPatternSpeedWithFactor(int factor) {
    if (patternSpeed < 0) {
        patternSpeed = patternSpeed / factor;
        if (patternSpeed == 0) {
            patternSpeed = -1;
        }
    } else if (patternSpeed > 0) {
        patternSpeed = patternSpeed / factor;
        if (patternSpeed == 0) {
            patternSpeed = 1;
        }
    }
}

//! Set pattern data in preparation for initialization
void furSwarmPatterns::setPatternData(uint8_t *data, uint8_t dataLength) {
    memcpy (lastData, data, dataLength);
    if (lastData [0] == pattern) {
        delayStopwatch = 0;
        initializePattern(data, dataLength);
        lastDataLength = 0;
        secondModStart = -1;
    } else if (0x80 & lastData[6]) {
        delayStopwatch = (((0x70 & lastData[6]) >> 4) & 0x0007) * FS_DELAY_FACTOR;
        secondModStart = 0x000F & lastData[6];
        lastDataLength = dataLength;
#ifdef SERIAL_DIAGNOSTICS
        Serial.print ("delayStopwatch: ");
        Serial.print (delayStopwatch);
        Serial.print (" lastData[6]: ");
        Serial.print (lastData[6]);
        Serial.print (" secondModStart: ");
        Serial.println (secondModStart);
#endif
    } else {
        delayStopwatch = lastData[6] * FS_DELAY_FACTOR + millis();
        secondModStart = -1;
        lastDataLength = dataLength;
    }
}

//! Check the delay stopwatch and initialize the last request
void furSwarmPatterns::checkLatestData() {
    if (secondModStart > 0) {
        if (clock.seconds % secondModStart == 0) {
            secondModStart = -1;
#ifdef SERIAL_DIAGNOSTICS
            Serial.print ("delayStopwatch: ");
            Serial.println (delayStopwatch);
#endif
            if (delayStopwatch != 0) {
                delayStopwatch += millis();
            } else {
                initializePattern(lastData, lastDataLength);
            }
        }
    } else if (delayStopwatch != 0) {
        if (millis() >= delayStopwatch) {
            initializePattern(lastData, lastDataLength);
            delayStopwatch = 0;
        }
    }
}

//! Continue pattern display
void furSwarmPatterns::continuePatternDisplay() {
    checkLatestData();
    // Continue with pattern display
    unsigned long hbTimeStamp = millis();
    switch (pattern) {
        case FS_ID_FULL_COLOR:
            setfullStrand(intensityLevel, redLevel, greenLevel, blueLevel, true);
            break;
        case FS_ID_SPARKLE:
            cycleData(patternSpeed, true, 0, 0);
            displayData(true, true, true);
            break;
        case FS_ID_DESCEND:
            descendData(patternSpeed);
            displayData(true, true, true);
            break;
        case FS_ID_OFF:
            break;
        case FS_ID_FLASH:
            descendPumpData(flashLedChangeRate);
            displayPumpData(true, true, true);
            break;
        case FS_ID_RANDOM_FLASH:
            heartbeatPumpShortStart--;
            if (heartbeatPumpShortStart <= 0) {
                initializeFlash(redLevel, greenLevel, blueLevel);
                heartbeatPumpShortStart = random (patternSpeed * 5);
            } else {
                descendPumpData(flashLedChangeRate);
            }
            displayPumpData(true, true, true);
            break;
        case FS_ID_HEART:
            if (heartbeatPumpShortStart > 0 && hbTimeStamp - heartbeatPumpShortStart > heartbeatPumpShortPeriod) {
                initializeHeartbeatPattern(redLevel, greenLevel, blueLevel);
                heartbeatPumpShortStart = 0;
                heartbeatPumpLongStart = hbTimeStamp;
            } else if (heartbeatPumpLongStart > 0 && hbTimeStamp - heartbeatPumpLongStart > heartbeatPumpShortPeriod * 2.5) {
                initializeHeartbeatPattern(redLevel, greenLevel, blueLevel);
                heartbeatPumpShortStart = hbTimeStamp;
                heartbeatPumpLongStart = 0;
            }
            descendPumpDataScaled(ledChangeRate);
            displayPumpData(true, true, true);
            break;
        case FS_ID_BREATHE:
            uint8_t stayBelow, stayAbove;
            float breatheCycle;
            breatheCycle = (float) breatheUpperLevel;
            breatheCycle = FS_BREATHE_MULTIPLIER * breatheCycle / 255.0;
            stayBelow = (uint8_t) breatheCycle;
            breatheCycle = (float) breatheUpperLevel;
            breatheCycle = FS_BREATHE_MULTIPLIER * breatheCycle / 255.0;
            stayAbove = (uint8_t) breatheCycle;
            cycleDataScaled(patternSpeed, false, stayBelow, stayAbove);
            displayData(true, true, true);		
            break;
        case FS_ID_ORGANIC:
            cycleData(patternSpeed, false, 50, 1);
            displayDataWithOverlay(true, false, false, 0, 5, 1);		
            break;
        case FS_ID_CYLON:
            displayCylon();
            break;
        case FS_ID_DROP:
            displayDrop();
            break;
        case FS_ID_CHARACTER:
            displayData(true, true, true);
            break;
        case FS_ID_CYLON_VERTICAL:
            displayBitmapPattern(FS_ID_CYLON_VERTICAL);
            break;
        case FS_ID_CYLON_PONG:
            displayBitmapPattern(FS_ID_CYLON_PONG);
            break;
        case FS_ID_SOUND_ACTIVATE:
            iterateSoundActivate();
            displaySoundActivate();
            break;
        case FS_ID_PRISM:
            iteratePrismColor(false);
            break;
        case FS_ID_MATRIX:
            displayMatrix();
            break;
        case FS_ID_RAINBOW_CHASE:
            iterateStrandByHSV();
            break;
        case FS_ID_STARFIELD:
            cycleData(patternSpeed, false, 50, 1);
            displayDataWithOverlay(true, true, true, 0, 0, 0);		
            break;
        case FS_ID_SPIRAL:
            displayBitmapPattern(FS_ID_SPIRAL);
            break;
        default:
            failedMessageCount++;
            break;
    }
}

//! Does the recently received data have a change to pattern or color?
bool furSwarmPatterns::patternOrColorChanged(uint8_t proposedData[]) {
    return pattern != proposedData [0] || proposedData [2] != redLevel || proposedData [3] != greenLevel || proposedData [4] != blueLevel;
}

//! Does the recently received data have no change to color?
bool furSwarmPatterns::colorNoChange(uint8_t proposedData[]) {
    return proposedData [2] == redLevel && proposedData [3] == greenLevel && proposedData [4] == blueLevel;
}

//! Appropriately advance the selected pattern intensity
void furSwarmPatterns::advancePatternIntensity(uint8_t pattern, bool continuous) {
    switch (pattern) {
        case FS_ID_HEART:
            if (continuous) heartbeatIntensity++;
            else heartbeatIntensity += 30;	
            if (heartbeatIntensity < 90) {
                heartbeatIntensity = 90;
            }
            break;
        case FS_ID_FULL_COLOR:
            if (continuous) intensityLevel++;
            else intensityLevel -= 40;
            if (intensityLevel > 200) {
                intensityLevel = 200;
            }
            frameRelease = patternSpeed * SMOOTH_FACTOR_INT;
            setfullStrand (intensityLevel, redLevel, greenLevel, blueLevel, true);
            break;
        case FS_ID_BREATHE:
            if (continuous) breatheUpperLevel++;
            else breatheUpperLevel += 20;
            if (breatheUpperLevel < 40) {
                breatheUpperLevel = 40;
            }
            break;
        case FS_ID_PRISM:
        case FS_ID_RAINBOW_CHASE:
            if (continuous) prismValue++;
            else prismValue -= 0.15;
            if (prismValue < 0.01) {
                prismValue = 0.75;
            }
            break;
        case FS_ID_MATRIX:
            if (continuous) intensityLevel++;
            else intensityLevel += 20;
            if (intensityLevel < 100) {
                intensityLevel = 100;
            }
            break;
        default:
            break;
    }
}

//! Move to the next pattern based on commanded button press
void furSwarmPatterns::triggerPatternChange(bool forward) {
    if (forward) {
        triggerPattern++;
        if (triggerPattern == triggerPatternsCount) {
            triggerPattern = 0;
        }
    } else {
        if (triggerPattern == 0) triggerPattern = triggerPatternsCount - 1;
        else triggerPattern--;
    }
    uint8_t triggerData[] = {triggerPattern,0,0,0,0,0};
    heartbeatPumpShortStart = millis();
    heartbeatPumpLongStart = 0;
    breatheUpperLevel = 0;
    for (int i = 0; i < 6; i++) {
      triggerData[i] = triggerPatterns[triggerPattern][i];
    }
    initializePattern(triggerData, 6);
}

//! Appropriately advance the selected pattern speed
void furSwarmPatterns::advancePatternSpeed(bool continuous, bool up) {
    uint8_t triggerData[] = {0,0,0,0,0,0};
    int direction = 1;
    if (!up) direction = -1;
    switch (pattern) {
        case FS_ID_FULL_COLOR:
            rgb toConvert;
            hsv converted;
            
            toConvert.r = (float) redLevel / 255.0;
            toConvert.g = (float) greenLevel / 255.0;
            toConvert.b = (float) blueLevel / 255.0;
            
            converted = rgb2hsv(toConvert);
            if (continuous) converted.h += 5.0 * direction;
            else converted.h += 30.0 * direction;
            if (converted.h > 360.0) {
                converted.h -= 360.0;
            }
            converted.s = 1.0;
            converted.v = 1.0;
            toConvert = hsv2rgb(converted);
            
            redLevel = toConvert.r * 255;
            greenLevel = toConvert.g * 255;
            blueLevel = toConvert.b * 255;
            
            frameRelease = patternSpeed * SMOOTH_FACTOR_INT;
            setfullStrand(intensityLevel, redLevel, greenLevel, blueLevel, true);
            break;
        case FS_ID_SPARKLE:
        case FS_ID_DESCEND:
            patternSpeed = patternSpeed * 20;
            patternSpeed += 20 * direction;
            if (patternSpeed > 155) {
                patternSpeed = 20;
            }
            setPatternSpeedWithFactor(20);
            break;
        case FS_ID_HEART:
            heartbeatPumpShortPeriod = 255 - heartbeatPumpShortPeriod / 4.0;
            heartbeatPumpShortPeriod -= 20 * direction;
            if (heartbeatPumpShortPeriod < 20) {
                heartbeatPumpShortPeriod = 210;
            }
            heartbeatPumpShortPeriod = (255 - heartbeatPumpShortPeriod) * 4.0;
            break;
        case FS_ID_BREATHE:
            if (continuous) patternSpeed++;
            else patternSpeed += 15 * direction;
            if (patternSpeed > 255) {
                patternSpeed = 50;
            }
            break;
        case FS_ID_ORGANIC:
            patternSpeed = patternSpeed * 10;
            if (continuous) patternSpeed -= direction;
            else patternSpeed -= 20 * direction;
            if (patternSpeed > 100) {
                patternSpeed = 100;
            }
            triggerData[0] = FS_ID_ORGANIC; // ID
            triggerData[1] = patternSpeed; // Speed
            initializePattern(triggerData, 2);
            break;
        case FS_ID_DROP:
            patternSpeed = patternSpeed * 10;
            if (continuous) patternSpeed -= direction;
            else patternSpeed -= 20 * direction;
            if (patternSpeed > 100) {
                patternSpeed = 100;
            }
            triggerData[0] = FS_ID_DROP; // ID
            triggerData[1] = patternSpeed; // Speed
            initializePattern(triggerData, 2);
            break;
        case FS_ID_CHARACTER:
            characterIndex++;
            if (characterIndex > characterIndexUpper) {
                characterIndex = 1;
            }
            initializeCharacter(intensityLevel, redLevel, greenLevel, blueLevel);
            break;
        case FS_ID_CYLON:
        case FS_ID_CYLON_VERTICAL:
        case FS_ID_CYLON_PONG:
        case FS_ID_SPIRAL:
        case FS_ID_MATRIX:
            if (continuous) {
                if (patternSpeed == 0) patternSpeed = 80;
                else patternSpeed -= direction;
            } else {
                if (patternSpeed < 12) {
                    patternSpeed -= 5 * direction;
                } else {
                    patternSpeed -= 10 * direction;
                }
            }
            if (patternSpeed == 0) {
                patternSpeed = 1;
            }
            if (patternSpeed > 80) {
                patternSpeed = 80;
            }
            break;
        case FS_ID_PRISM:
        case FS_ID_RAINBOW_CHASE:	
            if (continuous) patternSpeed++;
            else patternSpeed += 35 * direction;
            triggerData[0] = pattern; // ID
            triggerData[1] = patternSpeed; // Speed
            triggerData[5] = prismValue * 255; // Intensity
            initializePattern(triggerData, 6);
            break;
        default:
            if (continuous) patternSpeed += direction;
            else patternSpeed += 30 * direction;
    }
}


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

#ifndef soundActivation_h
#define soundActivation_h

#include<stdint.h>

// Sound Activate pattern variables
extern long saThreshold;
extern float saTargetThreshold;
extern long saSumOfSquare;
extern uint8_t audioAnalogPin;
extern bool saHighResetThreshold;
extern bool saMidResetThreshold;
extern bool saLowResetThreshold;
// Sound Activate - Volume
extern float saVolume;
extern float saVolumeIncrement;
extern float saVolumeUpper;
extern float saVolumeLower;
extern float saPressureMax;
extern float saThresholdAdjustment;
// Sound Activate - Sampling
extern int saMiddleValue;
extern int saNumberOfSamples; // Higher to make running average more peak'y
extern int saSample;
extern float saSignal; 
// Sound Activate - Moving Average
extern float saMovingAverage;
extern float saMovingAverageCount;
// Sound Activate - Running Average
extern float saRunningAverage;
extern uint8_t saAveragedOver;
extern float saAverageReading;

void updateSoundActivateParameters(long newThreshold, int newSampleNumber, int newAveragedOver);
void iterateSoundActivate();

#endif

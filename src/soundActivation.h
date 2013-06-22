/*

  soundActivation.h

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

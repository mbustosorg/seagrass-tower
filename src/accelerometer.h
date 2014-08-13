/*

  accelerometer.h

  Copyright (c) 2014, Mauricio Bustos
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

#ifndef accelerometer_h
#define accelerometer_h

#ifdef ADXL345
#include "adxl345.h"
#else
#include "adxl335.h"
#endif

#define TILT_THRESHOLD tiltThreshold (15)
#define TILT_CAL_THRESHOLD (5)
#define SHAKE_DETECTION_TIME (50)
#define SHAKE_LENGTH (4000)
#define SHAKE_THRESHOLD (50)

struct TiltVector {
  // Component G's
  float x;
  float y;
  float z;
};

class accelerometer {

 public:

  accelerometer();

  // Status
  TiltVector currentTilt();
  TiltVector filteredTilt();
  bool isShaking();
  void startup();
  void shutdown();

  // Configuration
  void calibrate();
  void resetShake();
  void setShakeThreshold(float newShakeThreshold);
  void setResetTime(long newResetTime);
  void setFilterLength(int newFilterLength);

 private:

#ifdef ADXL345
  adxl345 device;
#else
  adxl335 device;
#endif
  long shakeTimeStart;
  float shakeThreshold;
  long shakeResetTime;
  TiltVector filteredReadings;
  int filterLength = 0;
  // Shake detection
  TiltVector shakingMovingAverage;
  TiltVector lastTilt;
  int shakingMovingAverageLength = 100;

};

#endif

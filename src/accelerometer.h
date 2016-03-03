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

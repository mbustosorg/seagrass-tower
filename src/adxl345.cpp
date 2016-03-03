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

#include "adxl345.h"
#include <Wire.h>

//! Create the device object
adxl345::adxl345() {
  range = 2.0; // Default range is +/- 2.0g
}

//! Start the accelerometer
void adxl345::startup() {
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.send(ADXL345_POWER_CTL);  
  Wire.send(0x08);  // Measurement mode
  Wire.endTransmission();
}

//! Normalized read for 2 byte set starting at `byteStart'
float adxl345::normalizedRead(int byteStart) {
  int16_t reading;
  Wire.beginTransmission(ADXL345_ADDR);
  Wire.send(byteStart);  
  Wire.endTransmission();
  Wire.requestFrom(ADXL345_ADDR, 2);
  reading = Wire.receive() | Wire.receive() << 8;
  return (float)reading * (range * 2) / pow (2, 10);
}

//! X acceleration value
float adxl345::x() {
  return normalizedRead(ADXL345_DATAX0);
}

//! Y acceleration value
float adxl345::y() {
  return normalizedRead(ADXL345_DATAY0);
}

//! Z acceleration value
float adxl345::z() {
  return normalizedRead(ADXL345_DATAZ0);
}


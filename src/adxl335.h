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

#ifndef adxl335_h
#define adxl335_h

#define X_TILT_PIN (18)
#define Y_TILT_PIN (17)
#define Z_TILT_PIN (16)

#define TILT_BOUND (255.0)

class adxl335 {

 public:
  adxl335();

  void startup();

  float range;

  float x();
  float y();
  float z();

 private:
  float normalizedRead(int pin);

};

#endif
